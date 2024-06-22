#include "buffered_open.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    // Open a file with the O_PREAPPEND flag

    buffered_file_t *bf = buffered_open("test.txt", O_CREAT | O_PREAPPEND | O_RDWR, 0644);
    if (bf == NULL) {
        perror("buffered_open");
        return 1;
    }

    // Write to the file
    const char *msg = "Hello, World!\n";
    ssize_t bytes_written = buffered_write(bf, msg, strlen(msg));
    if (bytes_written == -1) {
        perror("buffered_write");
        return 1;
    }
/*
    // Read from the file
        char buf[4096];
        ssize_t bytes_read = buffered_read(bf, buf, sizeof(buf));
*/
    
    // Write to the file
    const char *msg2 = "This should be first || ";
    bytes_written = buffered_write(bf, msg2, strlen(msg2));
    if (bytes_written == -1) {
        perror("buffered_write");
        return 1;
    }

    // Flush the buffer
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        return 1;
    }

    // Write to the file
    const char *msg3 = "Now this has to be first \n\n";
    bytes_written = buffered_write(bf, msg3, strlen(msg3));
    if (bytes_written == -1) {
        perror("buffered_write");
        return 1;
    }

    buffered_close(bf);
    
    return 0;
}

