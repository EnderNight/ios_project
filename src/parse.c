#include "parse.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define SECTION_TEXT_SIZE 100



item open_item() {

    item it;
    char text[SECTION_TEXT_SIZE];
    int item_file;
    int save_sec_text = 0;
    int len = 0;

    char buf;


    item_file = open("tests/testdirs/inventory/gasoline.item", O_RDONLY);

    while (read(item_file, &buf, 1) && len < SECTION_TEXT_SIZE && save_sec_text == 0 || save_sec_text == 1) {

        if (buf == ']')
            save_sec_text = 2;

        if (save_sec_text) {
            text[len] = buf;
            ++len;
        }

        if (buf == '[')
        {
            save_sec_text = 1;
        }
    }

    if (len == SECTION_TEXT_SIZE)
        printf("Error: text too long!");

    text[len] = '\0';
    read_string(item_file, &it);
    close(item_file);

    return it;
}



void read_string(int fd, item *it){

    char* text;
    char buf;
    int num = 0;

    while (read(fd, &buf, 1)){
        text[num]=buf;
        ++num;
    }

    text[num] = '\0';

    it-> text = text;
}
