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

    while ((entry = readdir(dir)) != NULL) {
        if ((ext = strstr(entry->d_name, ".item")) != NULL) {
            fprintf(stderr, "%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

