<<<<<<< HEAD
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define COMMAND_NUMBER 100
#define COMMAND_LENGTH 100
#define DIRECTORY_LENGTH 1024
#define PATH_LENGTH 4096

char history_array [COMMAND_NUMBER][COMMAND_LENGTH];
char (*end_ptr)[COMMAND_LENGTH] = history_array + (COMMAND_NUMBER * COMMAND_LENGTH); // Pointer to the end of the history array.
char (*command_ptr)[COMMAND_LENGTH] = history_array; // Pointer to the current command in the history array.

int main(int argc, char *argv[]) {
    //change path file to include all the argv's
    char *originalPath = getenv("PATH");
    if (originalPath == NULL) {
        perror("getenv failed");
        return 1;
    }
    // Backing up the old path, in case it is not reset when the program ends.
    //char *oldPath;
    //strcpy(oldPath, currentPath);
    char newPath[PATH_LENGTH];
    snprintf(newPath, sizeof(newPath), "%s", originalPath);

    for (int i = 1; i < argc; i++) { // Start at 1 to skip the program name
        strncat(newPath, ":", sizeof(newPath) - strlen(newPath) - 1);
        strncat(newPath, argv[i], sizeof(newPath) - strlen(newPath) - 1);
    }

    int set_env_status = setenv("PATH", newPath, 1);
    if (set_env_status == -1) {
        perror("setenv failed");
        return 1;
    }
    char *cmd = malloc(COMMAND_LENGTH + 1);

    // Main loop
    while (1) {
        printf("$ ");
        fflush(stdout);

        char command[COMMAND_LENGTH];

        if (fgets(command, sizeof(command), stdin) != NULL) {
        // Remove the trailing newline character that fgets adds when the user presses Enter.
        command[strcspn(command, "\n")] = 0;
        }

        // move each command to the command history array, while making sure that the array does not exceed 100 commands.
        if (command_ptr < end_ptr) {
            strcpy(*command_ptr++, command);
        }

        if (strcmp(command, "history") == 0) {
            char (*print_ptr)[COMMAND_LENGTH] = history_array;
            while (print_ptr != command_ptr) {
                printf("%s\n", *print_ptr++);
            }
            print_ptr = history_array;
        }
        else if (strcmp(command, "exit") == 0) {
            // Reset the path to the old path before exiting the program.
            //setenv("PATH", oldPath, 1);
            free(cmd);
            exit(0);
        }
    
        else if (strcmp(command, "pwd") == 0) {
            char current_directory[DIRECTORY_LENGTH];
            getcwd(current_directory, DIRECTORY_LENGTH);
            if (current_directory == NULL) {
                perror("pwd failed");
                exit (1);
            }
            printf("%s\n", current_directory);
        } 
        else {
            // Split the command into the command and the arguments.
            char *token = strtok(command, " ");
            strcpy(cmd, token);
            char *arguments[COMMAND_LENGTH];
            int i = 0;

            while (token != NULL) {
                arguments[i] = token;
                token = strtok(NULL, " ");
                i++;
            }
            arguments[i] = NULL;

            if (strcmp(command, "cd") == 0) {
                if (chdir(arguments[1]) != 0) {
                    perror("cd failed");
                    exit (1);
                }
            } else {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork failed");
                    exit (1);
                }
                else if (pid == 0) {
                    // Child process

                    char *error_msg = strcat(cmd, " failed");
                    if (cmd[0] == '/') {
                        // If the command is an absolute path, then execute it directly.
                        execv(command, arguments);
                        // If execv returns, then it must have failed.
                        perror(error_msg);
                        exit (1);
                    }
                    else {
                        // Its a relative path, so search for the command in the PATH.
                        execvp(command, arguments);
                        // If execvp returns, then it must have failed.
                        perror(error_msg);
                        exit (1);
                    }
                } 
                else {
                    // Parent process
                    int status;
                    waitpid(pid, &status, 0);
                }
            }
        }              
    }        
}
=======
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define COMMAND_NUMBER 100
#define COMMAND_LENGTH 100
#define DIRECTORY_LENGTH 1024
#define PATH_LENGTH 4096


int main(int argc, char *argv[]) {

    char history_array [COMMAND_NUMBER][COMMAND_LENGTH];
    char (*end_ptr)[COMMAND_LENGTH] = history_array + (COMMAND_NUMBER * COMMAND_LENGTH); // Pointer to the end of the history array.
    char (*command_ptr)[COMMAND_LENGTH] = history_array; // Pointer to the current command in the history array.
    
    char *cmd2 = (char*)malloc(COMMAND_LENGTH + 1);
    free(cmd2);
    char *cmd = (char*)malloc(COMMAND_LENGTH + 1);

    //change path file to include all the argv's
    char *originalPath = getenv("PATH");
    if (originalPath == NULL) {
        perror("getenv failed");
        return 1;
    }
    // Backing up the old path, in case it is not reset when the program ends.
    //char *oldPath;
    //strcpy(oldPath, currentPath);
    char newPath[PATH_LENGTH];
    snprintf(newPath, sizeof(newPath), "%s", originalPath);

    for (int i = 1; i < argc; i++) { // Start at 1 to skip the program name
        strncat(newPath, ":", sizeof(newPath) - strlen(newPath) - 1);
        strncat(newPath, argv[i], sizeof(newPath) - strlen(newPath) - 1);
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

        char command[COMMAND_LENGTH];

        if (fgets(command, sizeof(command), stdin) != NULL) {
        // Remove the trailing newline character that fgets adds when the user presses Enter.
        command[strcspn(command, "\n")] = 0;
        }

        // move each command to the command history array, while making sure that the array does not exceed 100 commands.
        if (command_ptr < end_ptr) {
            strcpy(*command_ptr++, command);
        }

        if (strcmp(command, "history") == 0) {
            char (*print_ptr)[COMMAND_LENGTH] = history_array;
            while (print_ptr != command_ptr) {
                printf("%s\n", *print_ptr++);
            }
            print_ptr = history_array;
        }
        else if (strcmp(command, "exit") == 0) {
            // Reset the path to the old path before exiting the program.
            //setenv("PATH", oldPath, 1);
            exit(0);
        }
    
        else if (strcmp(command, "pwd") == 0) {
            char current_directory[DIRECTORY_LENGTH];
            getcwd(current_directory, DIRECTORY_LENGTH);
            if (current_directory == NULL) {
                perror("pwd failed");
                exit (1);
            }
            printf("%s\n", current_directory);
        } 
        else {
            // Split the command into the command and the arguments.
            char *token = strtok(command, " ");
            //char cmd[COMMAND_LENGTH];
            strcpy(cmd, token);
            char *arguments[COMMAND_LENGTH];
            int i = 0;

            while (token != NULL) {
                arguments[i] = token;
                token = strtok(NULL, " ");
                i++;
            }
            arguments[i] = NULL;

            if (strcmp(command, "cd") == 0) {
                if (chdir(arguments[1]) != 0) {
                    perror("cd failed");
                    exit (1);
                }
            } else {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork failed");
                    exit (1);
                }
                else if (pid == 0) {
                    // Child process

                    char *error_msg = strcat(cmd, " failed");
                    if (cmd[0] == '/') {
                        // If the command is an absolute path, then execute it directly.
                        execv(command, arguments);
                        // If execv returns, then it must have failed.
                        perror(error_msg);
                        exit (1);
                    }
                    else {
                        // Its a relative path, so search for the command in the PATH.
                        execvp(command, arguments);
                        // If execvp returns, then it must have failed.
                        perror(error_msg);
                        exit (1);
                    }
                } 
                else {
                    // Parent process
                    int status;
                    waitpid(pid, &status, 0);
                }
            }
        }              
    }        
}
>>>>>>> ad47245 (Updated to the latest verion which was able tu successfuly)
