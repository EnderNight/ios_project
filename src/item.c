#include "item.h"
#include "defines.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Item *create_item(void) {

    Item *new_item = malloc(sizeof(Item));
    new_item->name = malloc(sizeof(char) * TEXT_SIZE);
    new_item->desc = malloc(sizeof(char) * TEXT_SIZE);
    new_item->usage = malloc(sizeof(char) * TEXT_SIZE);

    return new_item;
}

void free_item(Item *item) {

    free(item->name);
    free(item->desc);
    free(item->usage);
    free(item);
}

void read_string(Item *it, int fd, int section) {

    int i = 0;
    char *text;

    switch (section) {
    case 0:
        text = it->name;
        break;

    case 1:
        text = it->desc;
        break;

    case 2:
        text = it->usage;
        break;
    }

    while (read(fd, text + i, 1)) {

        if (text[i] == '\n')
            break;
        ++i;
    }
    text[i] = '\0';
}

void load_item(Item *it, char *path) {

    int item_fd, i = 0, section;
    char text[SECTION_TEXT_SIZE];

    item_fd = open(path, O_RDONLY);
    fprintf(stderr, "%s\n", path);

    if (item_fd == -1) {
        fprintf(stderr, "Parser: Error when opening file %s\n", path);
    }

    while (read(item_fd, text + i, 1)) {

        if (i >= SECTION_TEXT_SIZE)
            break;

        if (text[i] == '\n') {
            if (i > 0 && text[i - 1] == ']') {
                text[i] = '\0';
                if (strcmp(text, "[name]") == 0)
                    section = 0;
                else if (strcmp(text, "[desc]") == 0)
                    section = 1;
                else if (strcmp(text, "[usage]") == 0)
                    section = 2;
                else {
                    fprintf(
                        stderr,
                        "Parser: Error while parsing %s, Unknown section %s\n",
                        path, text);
                    exit(1);
                }

                for (int j = 0; j < i; ++j) {
                    text[j] = '\0';
                }
                i = 0;

                read_string(it, item_fd, section);
            } else {
                i = 0;
                for (int j = 0; j < i; ++j)
                    text[j] = '\0';
            }
        } else
            ++i;
    }

    close(item_fd);
}
