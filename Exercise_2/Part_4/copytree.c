#include "copytree.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define PATH_MAX 4096


int is_symlink(const char *path) {
    struct stat path_stat;
    lstat(path, &path_stat);
    return S_ISLNK(path_stat.st_mode);
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
int copy_contents(const char *src, const char *dest) {
    // Open the source file
    int src_fd = open(src, O_RDONLY);

    // Open the destination file
    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC);

    // Copy the contents of the source file to the destination file
    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
    }

    // Close the source and destination files
    close(src_fd);
    close(dest_fd);  
}

// Function to create directories recursively
int create_directories(const char *path) {
    char temp[PATH_MAX];
    snprintf(temp, sizeof(temp), "%s", path);
    for (char *p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(temp, S_IRWXU) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }
    if (mkdir(temp, S_IRWXU) != 0 && errno != EEXIST) {
        return -1;
    }
    return 0;
}

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    // The file is a symbolic link
    if (is_symlink(src)) {
        char target_path[PATH_MAX];
        ssize_t len = readlink(src, target_path, sizeof(target_path) - 1);
        if (len == -1) {
            perror("readlink");
            return;
        }
        target_path[len] = '\0';

        // If we want to copy it as a link
        if (copy_symlinks) {
            int result = symlink(target_path, dest);
            if (result == -1) {
                perror("symlink");
                return;
            }
        // If we want to copy the target of the link
        } else {
            copy_contents(target_path, dest);
        }       
    // The file is a regular file
    } else if (is_regular_file(src)) {
        copy_contents(src, dest);
        if (copy_permissions) {
            struct stat st;
            if (stat(src, &st) == -1) {
                // Handle error
                perror("stat");
            }
            mode_t permissions = st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
            if (chmod(dest, permissions) == -1) {
                // Handle error
                perror("chmod");
            }
        }
    }
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(src)) == NULL) {
        perror("opendir");
        return;
    }
    // Create the destination directory if it doesn't exist (for the subdirectories)
    if (create_directories(dest) == -1) {
        perror("create_directories");
        closedir(dir);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // Construct the source and destination paths for the current entry
        char src_path[PATH_MAX];
        char dest_path[PATH_MAX];

        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        // Check if the current entry is a directory
        if (entry->d_type == DT_DIR) {
            // Recursively copy the directory
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
            // Copy the file or link
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }
    }
    closedir(dir);
}