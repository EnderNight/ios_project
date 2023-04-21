//
// Created by Sergio on 24/03/2023.
//

#include <libgen.h>
#include "inventory.h"
#include "parse.h"

void inventory() {

    char dir_name[100];                                         //variable for the directory
    strcpy(dir_name, "tests/testdirs/inventory/");  //directory where all .items will be
    DIR *dir;                                                   //idk
    struct dirent *entry;                                       //idk
    char *ext;                                                  //idk
    Item* item;                                                 //define the item
    item = create_item();

    if ((dir = opendir(dir_name)) == NULL) {
        perror("opendir error");
        return;
    }



    while ((entry = readdir(dir)) != NULL) {
        if ((ext = strstr(entry->d_name, ".item")) != NULL) { //looks for a sub-string inside a string

            strcat(dir_name, entry->d_name);

            clear_item(item);
            fprintf(stderr, "%s\n", dir_name);
            open_item(item, dir_name);
            strcpy(dir_name, "tests/testdirs/inventory/");
        }
    }
    closedir(dir);
    free_item(item);
}

int main(void)
{
    inventory();
}
