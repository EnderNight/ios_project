//
// Created by Sergio on 24/03/2023.
//
#include "inventory.h"
#include "item.h"
#include "utils.h"

#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h> // For exit()
#include <string.h>
#include <unistd.h>

void inventory(void) {

    char dir_name[100]; // variable for the directory
    strcpy(dir_name,
           "tests/testdirs/inventory/"); // directory where all .items will be
    DIR *dir;                            // idk
    struct dirent *entry;                // idk
    Item *item;
    char *ext; // idk

    if ((dir = opendir(dir_name)) == NULL) {
        perror("opendir error");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if ((ext = strstr(entry->d_name, ".item")) !=
            NULL) { // looks for a sub-string inside a string

            strcat(dir_name, entry->d_name);

            item = load_item(dir_name);

            if (item != NULL) {
                print("Name: %s\n", item->name);
                print("Desc: %s\n", item->desc);
                print("Usage: %s\n", item->usage);
                free_item(item);
            }
            strcpy(dir_name, "tests/testdirs/inventory/");
        }
    }
    closedir(dir);
}

int main(void) {
    inventory();

    return 0;
}
