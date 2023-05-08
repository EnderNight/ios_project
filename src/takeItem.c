
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

void move_item_file(const char* source_dir, const char* dest_dir) {
    char cwd[1024];
    cwd = getcwd(cwd, sizeof(cwd));
    DIR* dir;
    struct dirent* entry;
    char filename[20];
    char source_path[100];
    char dest_path[100];

    // Open the source directory
    dir = opendir(source_dir);
    if (dir == NULL) {
        fprintf(stderr, "Error opening directory\n");
        return;
    }

    // Iterate over the directory entries
    while ((entry = readdir(dir)) != NULL) {
        strcpy(filename, entry->d_name);
        snprintf(source_path, sizeof(source_path), "%s/%s", source_dir, filename);

        if (strcmp(filename, "flamethrower") == 0 ||
            strcmp(filename, "knife") == 0 ||
            strcmp(filename, "car_battery") == 0) {
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, filename);
            rename(source_path, dest_path);
        }
    }

    closedir(dir);
}