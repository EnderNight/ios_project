#include "item.h"
#include "defines.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *section_titles[] = {"name", "desc", "usage"};

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

    int i = 0, end = 1;
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

    while (end >= 0 && i < TEXT_SIZE && read(fd, text + i, 1)) {

        if (text[i] == '\n') {
            if (end) {
                end = 0;
            } else {
                text[i - 1] = '\0';
                end = -1;
            }
        }
        ++i;
    }

    if (i == TEXT_SIZE) {
        print_err("Item parser: section text is too long.");
    }
    if (!end) {
        text[i - 1] = '\0';
        text[i] = '\0';
    }
}

// TODO: use errno
int parse_title(int fd) {

    int section = -2, i = 0, j, num_t = num_titles();
    char buf[SECTION_TITLE_SIZE];
    ssize_t ret;

    while (section == -2 && i < SECTION_TITLE_SIZE &&
           (ret = read(fd, buf + i, sizeof(char)))) {

        if (buf[i] == ']') {
            j = 0;
            buf[i] = '\0';
            while (j < num_t && section == -2) {
                if (strcmp(buf, section_titles[j]) == 0) {
                    section = j;
                }
                ++j;
            }

            if (section == -2) {
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
        print_err(
            "Item parser error: incomplete title (missing ']' bracket).\n");
        section = -1;
    }

    if (ret > 0) {
        read(fd, buf, sizeof(char));
    }

    return section;
}

/*
 * Load '.item' file at path to an Item
 *
 * @path: '.item' file path
 *
 * return: - NULL in case of a parsing error
 *         - Item* with loaded text
 */
Item *load_item(char *path) {

    int item_fd, section = 0;
    char buf;
    Item *it;

    it = create_item();
    item_fd = open(path, O_RDONLY);

    if (item_fd == -1)
        print_err("Item parser: Error when opening file %s\n", path);

    while (section >= 0 && read(item_fd, &buf, sizeof(char))) {

        if (buf == '[') {
            section = parse_title(item_fd);

            if (section > -1) {
                load_section_text(it, item_fd, section);
            } else {
                free_item(it);
                it = NULL;
            }
        }
    }

    close(item_fd);

    return it;
}
