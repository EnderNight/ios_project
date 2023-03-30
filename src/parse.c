#include <stdlib.h>

#include "defines.h"

#include "parse.h"

void open_item(Item *it) {

    char text[SECTION_TEXT_SIZE];
    int item_file;
    int save_sec_text = 0;
    int len = 0;

    char buf;

    item_file = open("tests/testdirs/inventory/gasoline.item", O_RDONLY);

    while (read(item_file, &buf, 1) && len < SECTION_TEXT_SIZE &&
           (save_sec_text == 0 || save_sec_text == 1)) {

        if (buf == ']')
            save_sec_text = 2;

        if (save_sec_text) {
            text[len] = buf;
            ++len;
        }

        if (buf == '[') {
            save_sec_text = 1;
        }
    }

    if (len == TEXT_SIZE)
        printf("Error: text too long!");

    text[len] = '\0';
    read_string(item_file, it);
    close(item_file);
}

void read_string(int fd, Item *it) {

    int i = 0;

    while (read(fd, it->text + i, 1)) {
        ++i;
    }

    it->text[i] = '\0';
}
