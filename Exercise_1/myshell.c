#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

int main(int argc, char *argv[]) {
    while (1) {
        printf("$ ");
        fflush(stdout);
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            return 1;
        }
        if (pid == 0) {
            // Child process
            char command[256];
            if (fgets(command, sizeof(command), stdin) != NULL) {
            // Remove the trailing newline character that fgets adds when the user presses Enter.
            command[strcspn(command, "\n")] = 0;
            
                if (strcmp(command, "history") == 0) {
                    printf("history\n");
                    return 0;
                    }
                if (strcmp(command, "cd") == 0) {
                    printf("cd\n");
                    return 0;
                }
                if (strcmp(command, "pwd") == 0) {
                    printf("pwd\n");   
                    return 0;
                }
                if (strcmp(command, "exit") == 0) {
                    printf("exit\n");
                    return 0;
                }
                else {
                    execlp(command, command, NULL);
                    perror(command);
                    return 1;
                }   
            }      
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    return 0;
    }
}