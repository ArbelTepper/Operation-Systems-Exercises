#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>



int main(int argc, char *argv[]) {
    if (argc != 5) {
    fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>", argv[0]);    
    return 1;
    }

    char *parent_message = argv[1];
    char *child1_message = argv[2];
    char *child2_message = argv[3];
    int num = atoi(argv[4]);

    // Create a file called output.txt
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to create output.txt\n");
        return 1;
    }
    
    // Fork the process to create two child processes
    pid_t child1_pid, child2_pid;
    child2_pid = fork();

    if (child2_pid < 0) {
        perror("Failed to fork child process 2\n");
        return 1;
    } else if (child2_pid == 0) {
        // Child process 2
        //printf("Child 2\n");
        child1_pid = fork();
        // Child process 1
        //printf("Child 1\n");
        if (child1_pid < 0) {
            perror("Failed to fork child process 1\n");
            return 1;
        } else if (child1_pid == 0) {
            for (int i = 0; i < num; i++) {
                fprintf(file, "%s\n", child1_message);
            }
            //printf("Child 1 finished\n");
            exit(0);
        }
        waitpid(child1_pid, NULL, 0);
        for (int i = 0; i < num; i++) {
        fprintf(file, "%s\n", child2_message);
        }
        //printf("Child 2 finished\n");
        exit(0);
    }
    waitpid(child1_pid, NULL, 0);
    // Parent process
    //printf("Parent\n");
    for (int i = 0; i < num; i++) {
        fprintf(file, "%s\n", parent_message);
    }
    //printf("Parent finished\n");
    fclose(file);
}