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

    // Read from the file
    char buf[4096*2];
    ssize_t bytes_read = buffered_read(bf, buf, sizeof(buf));
    printf("Read %ld bytes\n", bytes_read);

    printf("The read content is:\n\n %s\n", buf);

    buffered_close(bf);
    
    return 0;
}

