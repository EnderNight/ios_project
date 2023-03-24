#include "parse.c"
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


    item_file = open("testdirs/inventory/gasoline.item", O_RDONLY);

    while (read(item_file, &buf, 1) && len < SECTION_TEXT_SIZE) {

        if (buf == ']')
            save_sec_text = 0;

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

    printf("%s\n", text);

    it.text = text;

    printf("%s\n", it.text);

    close(item_file);

    return it;
}


void print_item_text(item it) {

    printf("%s\n", it.text);

}
