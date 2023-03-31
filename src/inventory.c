//
// Created by Sergio on 24/03/2023.
//


#include "inventory.h"

void inventory() {
    char *dir_name = "tests/testdirs/inventory";
    DIR *dir;
    struct dirent *entry;
    char *ext;

    if ((dir = opendir(dir_name)) == NULL) {
        perror("opendir error");
        return;
    }

<<<<<<< HEAD
    char buf[BUF_SIZE];
    int bytes_read;

    while ((bytes_read = read(fd, buf, BUF_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, bytes_read) != bytes_read) {
            perror("write");
            exit(1);
=======
    while ((entry = readdir(dir)) != NULL) {
        if ((ext = strstr(entry->d_name, ".item")) != NULL) {
            fprintf(stderr, "%s\n", entry->d_name);
>>>>>>> 2336d84d130b98c86f8840c534fe20bd2c4265a4
        }
    }

    closedir(dir);
}

