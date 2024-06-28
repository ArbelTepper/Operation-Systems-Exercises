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

#endif // BUFFERED_OPEN_H