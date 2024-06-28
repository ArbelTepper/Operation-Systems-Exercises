#include "buffered_open.h"

// Function to wrap the original open function ************************************************************************88
buffered_file_t *buffered_open(const char *pathname, int flags, ...) {

    int normal_flags = flags & ~O_PREAPPEND;
    va_list args;
    va_start(args, flags);

    buffered_file_t *bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));
    if (!bf) {
        perror("buffered_open: malloc");
        return NULL;
    }
    mode_t mode = va_arg(args, mode_t);
    if (flags & O_CREAT) {
        // If O_CREAT is set in flags, we expect an additional argument for mode
        bf->fd = open(pathname, normal_flags, mode);
    } else {
        bf->fd = open(pathname, flags, mode);
    }
    va_end(args);

    if (bf->fd == -1) {
        perror("buffered_open: open");
        return NULL;
    }

    bf->read_buffer = (char *)malloc(BUFFER_SIZE);
    bf->write_buffer = (char *)malloc(BUFFER_SIZE);
    
    bf->read_buffer_size = BUFFER_SIZE;
    bf->write_buffer_size = BUFFER_SIZE;

    bf->read_buffer_pos = 0;
    bf->write_buffer_pos = 0;

    bf->flags = flags;
    bf->preappend = (flags & O_PREAPPEND) ? 1 : 0;

    return bf;
}

// Function to flush the buffer to the file **************************************************************************
int buffered_flush(buffered_file_t *bf) {
    if (bf->preappend && bf->write_buffer_pos > 0) {
        // Get the size of the original file
        off_t file_size = lseek(bf->fd, 0, SEEK_END);
        if (file_size == -1) {
            perror("buffered_flush: lseek to end");
            buffered_close(bf);
            return -1;
        }

        // Allocate memory for the original content
        char *temp_buffer = malloc(file_size);
        if (!temp_buffer) {
            perror("buffered_flush: malloc");
            buffered_close(bf);
            return -1;
        }

        // Read the original content into memory
        if (lseek(bf->fd, 0, SEEK_SET) == -1) { // Go back to the start of the file
            perror("buffered_flush: lseek to start");
            free(temp_buffer);
            buffered_close(bf);
            return -1;
        }
        if (read(bf->fd, temp_buffer, file_size) != file_size) {
            perror("buffered_flush: read from file");
            free(temp_buffer);
            buffered_close(bf);
            return -1;
        }

        // Go back to the start of the file for writing
        if (lseek(bf->fd, 0, SEEK_SET) == -1) {
            perror("buffered_flush: lseek to start for writing");
            free(temp_buffer);
            buffered_close(bf);
            return -1;
        }

        // Write the new content to the file
        ssize_t new_content_written = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
        if (new_content_written != bf->write_buffer_pos) {
            perror("buffered_flush: write new content");
            free(temp_buffer);
            buffered_close(bf);
            return -1;
        }

        // Write the original content back to the file
        ssize_t original_content_written = write(bf->fd, temp_buffer, file_size);
        if (original_content_written != file_size) {
            perror("buffered_flush: write original content");
            free(temp_buffer);
            buffered_close(bf);
            return -1;
        }

        // Free the temporary buffer
        free(temp_buffer);
    } else if (bf->write_buffer_pos > 0) {
        ssize_t bytes_written = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
        if (bytes_written != bf->write_buffer_pos) {
            perror("buffered_flush: write");
            return -1;
        }
    }

    // Reset the write buffer position
    bf->write_buffer_pos = 0;

    return 0;
}

// Function to write to the buffered file ****************************************************************************
ssize_t buffered_write(buffered_file_t *bf, const char *buf, size_t count) {
    int buf_len = strlen(buf);
    if (count > buf_len) {
        count = buf_len;
    }

    const char *char_buf = (const char *)buf;
    size_t bytes_written = 0;

    // Debug: Print initial state
    printf("Initial buffer pos: %zu, count: %zu\n", bf->write_buffer_pos, count);

    // Flush the buffer if there isn't enough space to hold the new data
    if (bf->write_buffer_pos + count > bf->write_buffer_size) {
        if (buffered_flush(bf) == -1) {
            perror("buffered_write: buffered_flush");
            return -1;
        }
    }

    if (bf->preappend && bf->write_buffer_pos > 0) {
        // Allocate a temporary buffer to hold the combined data
        size_t new_buffer_size = bf->write_buffer_pos + count;
        char *temp_buffer = (char *)malloc(new_buffer_size);
        if (!temp_buffer) {
            perror("buffered_write: malloc");
            return -1;
        }

        // Copy new data followed by existing buffer data into the temporary buffer
        memcpy(temp_buffer, char_buf, count);
        memcpy(temp_buffer + count, bf->write_buffer, bf->write_buffer_pos);

        // Free the existing write buffer and update it to point to the temporary buffer
        free(bf->write_buffer);
        bf->write_buffer = temp_buffer;
        bf->write_buffer_size = new_buffer_size;
        bf->write_buffer_pos = new_buffer_size;

        bytes_written = count;

        // Debug: Print state after preappend
        printf("After preappend - buffer pos: %zu, new buffer size: %zu\n", bf->write_buffer_pos, new_buffer_size);
    } else {
        while (count > 0) {
            size_t space_left = bf->write_buffer_size - bf->write_buffer_pos;
            size_t bytes_to_write = (count < space_left) ? count : space_left;

            // Copy data to the write buffer
            memcpy(bf->write_buffer + bf->write_buffer_pos, char_buf, bytes_to_write);
            bf->write_buffer_pos += bytes_to_write;
            char_buf += bytes_to_write;
            count -= bytes_to_write;
            bytes_written += bytes_to_write;

            // Debug: Print state after each write iteration
            printf("Buffer pos: %zu, bytes_written: %zu, remaining count: %zu\n", bf->write_buffer_pos, bytes_written, count);

            // If the buffer is full, flush it
            if (bf->write_buffer_pos >= bf->write_buffer_size) {
                if (buffered_flush(bf) == -1) {
                    perror("buffered_write: buffered_flush");
                    return -1;
                }

                // Debug: Print state after flush
                printf("After flush - buffer pos: %zu\n", bf->write_buffer_pos);
            }
        }
    }

    return bytes_written;
}


// Function to read from the buffered file ****************************************************************************
ssize_t buffered_read(buffered_file_t *bf, char *buf, size_t count){

    // This flush operation ensures that the buffer's contents are correctly written to the file before any read occurs.
    buffered_flush(bf);

    size_t bytes_read = 0;
    // If the read buffer is empty, fill the buffer with data from the file
    if (bf->read_buffer_pos == 0) {
        bytes_read = read(bf->fd, bf->read_buffer, bf->read_buffer_size);
        if (bytes_read == -1) {
            perror("buffered_read: read");
            return -1;
        }
    }

    // If the user wants to read more data than the buffer can hold, read from the buffer to the end
    // and then fill the buffer with new data from the file and continue reading

    while (bf->read_buffer_pos + count > bf->read_buffer_size) {
        // Read the remaining data from the buffer
        size_t bytes_to_read = bf->read_buffer_size - bf->read_buffer_pos;
        memcpy(buf, bf->read_buffer + bf->read_buffer_pos, bytes_to_read);
        bf->read_buffer_pos = 0;

        // Update the buffer pointer and count
        buf += bytes_to_read;
        count -= bytes_to_read;

        // fill the read buffer with new data from the file
        bytes_read += read(bf->fd, bf->read_buffer, bf->read_buffer_size);
        if (bytes_read == -1) {
            perror("buffered_read: read");
            return -1;
        }
    }

    // Read the remaining data from the buffer
    memcpy(buf, bf->read_buffer + bf->read_buffer_pos, count);
    bf->read_buffer_pos += count;
    bf->read_buffer_pos = (bf->read_buffer_pos) % bf->read_buffer_size;
    //bytes_read += count;

    return bytes_read;
}


// Function to close the buffered file ********************************************************************************
int buffered_close(buffered_file_t *bf){
    // Flush the buffer before closing the file
    if (buffered_flush(bf) == -1) {
        perror("buffered_close: buffered_flush");
        return -1;
    }
    // Close the file descriptor
    if (close(bf->fd) == -1) {
        perror("buffered_close: close");
        return -1;
    }

    // Free the allocated memory and set pointers to NULL
    if (bf->read_buffer) {
        free(bf->read_buffer);
        bf->read_buffer = NULL;
    }
    if (bf->write_buffer) {
        free(bf->write_buffer);
        bf->write_buffer = NULL;
    }
    return 0;
}