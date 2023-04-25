#include "item.h"
#include "defines.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *section_titles[] = {"[name]", "[desc]", "[usage]"};

int num_titles(void) { return sizeof(section_titles) / sizeof(char *); }

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

/*
 * Load the corresponding text from fd inside it.
 *
 * @it: Item in which text is loaded
 * @fd: file descriptor of the .item file
 * @section: The section number where to save the text
 *
 */
void load_section_text(Item *it, int fd, int section) {

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

    while (read(fd, text + i, 1) && text[i] != '\n') {
        ++i;
    }
    text[i] = '\0';
}

// TODO: use errno
int parse_title(int fd) {

    int section = -3, i = 0, j, num_t = num_titles();
    char buf[SECTION_TITLE_SIZE];
    ssize_t ret;

    while ((ret = read(fd, buf + i, sizeof(char))) && section == -3 &&
           i < SECTION_TITLE_SIZE) {

        if (buf[i] == '\n') {
            i = 0;
        } else if (i > 0 && buf[i] == ']') {

            j = 0;
            buf[i + 1] = '\0';
            while (j < num_t && section == -3) {
                if (strcmp(buf, section_titles[j]) == 0) {
                    section = j;
                }
                ++j;
            }

            if (section == -3) {
                print_err(
                    "Item parser error: Unrecognized section title '%s'.\n",
                    buf);
                section = -1;
            }

        } else {
            ++i;
        }
    }

    if (ret == -1) {
        print_err("Item parser error: error while reading file.\n");
        section = -1;
    } else if (i >= SECTION_TITLE_SIZE) {
        print_err("Item parser error: Section title must be shorter than "
                  "SECTION_TITLE_SIZE: %d\n",
                  SECTION_TITLE_SIZE);
        section = -1;
    } else if (ret == 0) {
        section = -2;
    }

    if (section > -1) {
        lseek(fd, sizeof(char), SEEK_CUR);
    }

    return section;
}

void load_item(Item *it, char *path) {

    int item_fd, ret;

    item_fd = open(path, O_RDONLY);

    if (item_fd == -1)
        print_err("Parser: Error when opening file %s\n", path);

    while ((ret = parse_title(item_fd)) >= 0) {
        load_section_text(it, item_fd, ret);
    }

    close(item_fd);
}
