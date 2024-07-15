#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


void write_message(const char *message, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

void shuffle(char *array[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        char *tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 6) {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <order> <count>", argv[0]);
        return 1;
    }

    int lockfile;
    int num_children = argc - 2;
    int count = atoi(argv[argc - 1]);

    // Allocate memory for the messages array
    char **messages = malloc((argc - 2) * sizeof(char *));

    // Copy the messages from argv to the new array
    for (int i = 1; i < argc - 1; i++) {
        messages[i - 1] = argv[i];
    }

    srand(time(NULL)); // Seed the random number generator

    // Shuffle the array
    shuffle(messages, num_children); // Exclude the program name and the last two arguments
    
    pid_t pid;
    for (int i = 0; i < num_children; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            //printf("I am child number %d\n", i);

            // Try to create the lock file until successful
            while ((lockfile = open("lockfile.lock", O_CREAT | O_EXCL, 0644)) < 0) {
                // The lock file already exists, another process is running
                usleep(1000000); // Sleep for 1 second
            }
            // This process now has the lock
            write_message(messages[i], count); // Print the unique message for this child

            // Remove the lock file when done
            close(lockfile);
            unlink("lockfile.lock");
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            return 1;
        }
    }
    for (int i = 0; i < num_children; i++) {
        wait(NULL);
    }
    return 0;
}
