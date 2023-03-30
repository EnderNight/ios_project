#ifndef PARSE_H
#define PARSE_H

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define SECTION_TEXT_SIZE 20 
#define TEXT_SIZE 200


typedef struct item {
    char* text;
}item;

item open_item(void);
void read_string(int fd, item *it);
#endif
