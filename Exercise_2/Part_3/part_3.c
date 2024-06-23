#include "buffered_open.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    // Open a file without the O_PREAPPEND flag

    buffered_file_t *bf = buffered_open("test.txt", O_CREAT | O_RDWR | O_PREAPPEND, 0644);
    if (bf == NULL) {
        perror("buffered_open");
        return 1;
    }

    // Read from the file
    char buf[100];
    ssize_t bytes_read = buffered_read(bf, buf, sizeof(buf));
    printf("Read %ld bytes\n", bytes_read);
    printf("Theposition of the read buffer is: %ld\n", bf->read_buffer_pos);
    printf("The read content is:\n %s\n", buf);

    bytes_read = buffered_read(bf, buf, sizeof(buf));
    printf("Read %ld bytes\n", bytes_read);
    printf("Theposition of the read buffer is: %ld\n", bf->read_buffer_pos);
    printf("The read content is:\n %s\n", buf);

    // Write to the file
    char *str = "\n\nHello, World!\n\n";
    ssize_t bytes_written = buffered_write(bf, str, strlen(str));
    printf("Wrote %ld bytes\n", bytes_written);
    printf("The position of the write buffer is: %ld\n", bf->write_buffer_pos);

    bytes_read = buffered_read(bf, buf, sizeof(buf));
    printf("Read %ld bytes\n", bytes_read);
    printf("Theposition of the read buffer is: %ld\n", bf->read_buffer_pos);
    printf("The read content is:\n %s\n", buf);

    buffered_close(bf);
    
    return 0;
}

