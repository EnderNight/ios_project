//
// Created by Sergio on 24/03/2023.
//

#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h> // For exit()
#include <string.h>
#include <unistd.h>

#include "inventory.h"
#include "item.h"

void inventory(void) {

    char dir_name[100]; // variable for the directory
    strcpy(dir_name,
           "tests/testdirs/inventory/"); // directory where all .items will be
    DIR *dir;                            // idk
    struct dirent *entry;                // idk
    Item *item;
    char *ext; // idk
    item = create_item();

    if ((dir = opendir(dir_name)) == NULL) {
        perror("opendir error");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if ((ext = strstr(entry->d_name, ".item")) !=
            NULL) { // looks for a sub-string inside a string

            strcat(dir_name, entry->d_name);

            free_item(item);
            item = create_item();

            load_item(item, dir_name);
            strcpy(dir_name, "tests/testdirs/inventory/");
        }
    }
    closedir(dir);
    free_item(item);
}

int main(void) {
    inventory();

    return 0;
}
