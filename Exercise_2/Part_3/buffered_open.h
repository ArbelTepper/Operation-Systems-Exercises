#ifndef BUFFERED_OPEN_H
#define BUFFERED_OPEN_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

// Define a new flag that doesn't collide with existing flags
#define O_PREAPPEND 0x40000000
#define BUFFER_SIZE 4096

// Structure to hold the buffer and original flags
typedef struct {
    int fd;                     // File descriptor for the opened file

    char *read_buffer;          // Buffer for reading operations, holds data read from the file
    char *write_buffer;         // Buffer for writing operations, holds data to be written to the file

    size_t read_buffer_size;    // Size of the read buffer, indicating how much data it can hold
    size_t write_buffer_size;   // Size of the write buffer, indicating how much data it can hold

    size_t read_buffer_pos;     // Current position in the read buffer, indicating the next byte to be read
    size_t write_buffer_pos;    // Current position in the write buffer, indicating the next byte to be written

    int flags;                  // File flags used to control file access modes and options (like O_RDONLY, O_WRONLY)

    int preappend;              // Flag to remember if the O_PREAPPEND flag was used, indicating special handling for writes
} buffered_file_t;


buffered_file_t *buffered_open(const char *pathname, int flags, ...);
ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count);
ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count);
int buffered_flush(buffered_file_t *bf);
int buffered_close(buffered_file_t *bf);

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

    if (flags & O_CREAT) {
        // If O_CREAT is set in flags, we expect an additional argument for mode
        mode_t mode = va_arg(args, mode_t);
        bf->fd = open(pathname, normal_flags, mode);
    } else {
        bf->fd = open(pathname, normal_flags);
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
int buffered_flush(buffered_file_t *bf){
    // Write the contents of the write buffer to the file
    if (bf->preappend && bf->write_buffer_pos > 0) {
        // Get the size of the original file
        off_t file_size = lseek(bf->fd, 0, SEEK_END);

        // Allocate memory for the original content
        char *temp_buffer = malloc(file_size);

        // Read the original content into memory
        lseek(bf->fd, 0, SEEK_SET); // Go back to the start of the file
        if (read(bf->fd, temp_buffer, file_size) != file_size) {
            perror("read from file in buffered_flush failed");
            buffered_close(bf);
            return -1;
        }

        // Go back to the start of the file for writing
        lseek(bf->fd, 0, SEEK_SET);

        // Write the new content to the file
        ssize_t new_content_written = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
        if (new_content_written == -1) {
            perror("buffered_flush: write new content");
            buffered_close(bf);
            return -1;
        }

        // Write the original content back to the file
        ssize_t original_content_written = write(bf->fd, temp_buffer, file_size);
        if (original_content_written == -1) {
            perror("buffered_flush: write original content");
            buffered_close(bf);
            return -1;
        }
    } else {
        // Write the contents of the write buffer to the end of the file
        lseek(bf->fd, 0, SEEK_END);
        ssize_t bytes_written = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
        if (bytes_written == -1) {
            perror("buffered_flush: write");
            return -1;
        }

        // Reset the write buffer position
        bf->write_buffer_pos = 0;
    }
        

    // Reset the write buffer position
    bf->write_buffer_pos = 0;

    return 0;
}

// Function to write to the buffered file ****************************************************************************
ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count){
    if (bf->write_buffer_pos + count > bf->write_buffer_size) {
            // Flush the buffer before writing
            if (buffered_flush(bf) == -1) {
                perror("buffered_write: buffered_flush");
                return -1;
            }
        }
    // Now the write buffer has enough space to hold the new data
    if (bf->preappend && bf->write_buffer_pos > 0) {
        char *temp_buffer = (char *)malloc(BUFFER_SIZE);
        memcpy(temp_buffer, buf, count);
        memcpy(temp_buffer + count, bf->write_buffer, bf->write_buffer_pos);
        free(bf->write_buffer);
        bf->write_buffer = temp_buffer;
        bf->write_buffer_pos += count;
    } else {
        memcpy(bf->write_buffer + bf->write_buffer_pos, buf, count);
        bf->write_buffer_pos += count;
    }
    return count;
}

// Function to read from the buffered file ****************************************************************************
ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count){
    // This flush operation ensures that the buffer's contents are correctly written to the file before any read occurs.
    buffered_flush(bf);

    size_t original_count = count;
    size_t bytes_read = 0;
    // If the read buffer is empty, fill the buffer with data from the file
    if (bf->read_buffer_pos == 0) {
        bytes_read = read(bf->fd, bf->read_buffer, bf->read_buffer_size);
        if (bytes_read == -1) {
            perror("buffered_read: read");
            return -1;
        }
    }


    // If the user wants to read more data than the buffer can hold, read fro the buffer to the end
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
    bf->read_buffer_pos += bytes_read;
    bf->read_buffer_pos = (bf->read_buffer_pos) % bf->read_buffer_size;

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

    // Free the allocated memory
    free(bf->read_buffer);
    free(bf->write_buffer);
    free(bf);
    return 0;
}

#endif // BUFFERED_OPEN_H