// see if all the costum made commands need ti be in a separete file, probably yes because of the exec function
// that needs to get a path or file to execute.

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define COMMAND_LENGTH_AND_NUMBER 100
#define DIRECTORY_LENGTH 1024

char history_array [COMMAND_LENGTH_AND_NUMBER][COMMAND_LENGTH_AND_NUMBER];
char *command_ptr = history_array;

int main(int argc, char *argv[]) {
    //change path file to include all the argv's
    char *currentPath = getenv("PATH");
    if (currentPath == NULL) {
        perror("getenv failed");
        return 1;
    }
    // Backing up the old path, in case it is not reset when the program ends.
    char oldPath;
    strcpy(oldPath, currentPath);
    char newPath[1024];
    // Concatenating the old path with the new path variables passed through the command line arguments.
    for (int i = 1; i < argc; i++) // Start at 1 to skip the program name, which is always at argv[0].
    {
        snprintf(newPath, sizeof(newPath), "%s:%s", oldPath, argv[i]);
    }
    int set_env_status = setenv("PATH", newPath, 1);
    if (set_env_status == -1) {
        perror("setenv failed");
        return 1;
    }

    // Main loop
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
            char command[100];
            if (fgets(command, sizeof(command), stdin) != NULL) {
            // Remove the trailing newline character that fgets adds when the user presses Enter.
            command[strcspn(command, "\n")] = 0;
            // move each command to the command history array, while making sure that the array does not exceed 100 commands.
            if (command_ptr != &history_array[100][100]) {
                strcpy(*command_ptr++, command);
            }
                if (strcmp(command, "history") == 0) {
                    printf("history\n"); // -------------------------------------> Remove this line after testing.
                    for (int i = 0; i < 100; i++) {
                        printf("%s\n", history_array[i]);
                    }
                    return 0;
                    }
                if (strcmp(command, "cd") == 0) {
                    printf("cd\n"); // -------------------------------------> Remove this line after testing.
                    if (chdir("/new/path") != 0) {
                    perror("cd error");
                    return 1;
}
                    return 0;
                }
                if (strcmp(command, "pwd") == 0) {
                    printf("pwd\n"); // -------------------------------------> Remove this line after testing.
                    char current_directory[DIRECTORY_LENGTH];
                    getcwd(current_directory, DIRECTORY_LENGTH);
                    printf("%s\n", current_directory);
                    return 0;
                }
                if (strcmp(command, "exit") == 0) {
                    printf("exit\n"); // -------------------------------------> Remove this line after testing.
                    // Reset the path to the old path before exiting the program.
                    setenv("PATH", oldPath, 1);
                    exit(0);
                }
                else {
                    execlp(command, command, NULL);
                    // If execlp returns, then it must have failed.
                    char error_msg = strcat(command, " failed");
                    perror(error_msg);
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