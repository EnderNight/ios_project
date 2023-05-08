
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int user_rank = 0;

void takeItem() {
    const char *initial_path = "./";            // Path (cwd)
    const char *dest_path = "story/inventory/"; // Path to inventory

    DIR *dir;
    struct dirent *entry;
    char filename[20];
    char source_path[100];
    char dest_item_path[100];

    // open initial path
    dir = opendir(initial_path);
    if (dir == NULL) {
        fprintf(stderr, "Error opening the initial path\n");
        return;
    }

    bool found_item = false;

    // iterate over the files in the directory
    while ((entry = readdir(dir)) != NULL) {
        strcpy(filename, entry->d_name);

        // check if the file is an item
        if (strcmp(filename, "knife.item") == 0 ||
            strcmp(filename, "flamethrower.item") == 0 ||
            strcmp(filename, "car_battery.item") == 0) {
            snprintf(source_path, sizeof(source_path), "%s%s", initial_path,
                     filename);
            snprintf(dest_item_path, sizeof(dest_item_path), "%s%s", dest_path,
                     filename);

            // move the item to the inventory
            if (rename(source_path, dest_item_path) == 0) {
                found_item = true;
                printf("You took: %s\n", filename);
                // Increment user rank
                if (strcmp(filename, "knife.item") == 0 && user_rank == 0) {
                    user_rank = 1;
                } else if (strcmp(filename, "flamethrower.item") == 0 &&
                           user_rank == 1) {
                    user_rank = 2;
                } else if (strcmp(filename, "car_battery.item") == 0 &&
                           user_rank == 2) {
                    user_rank = 3;
                }
            } else {
                fprintf(stderr, "Error moving item: %s\n", filename);
            }
        }
    }

    closedir(dir);

    // if no items were found
    if (!found_item) {
        printf("There is nothing here.\n");
    }
}