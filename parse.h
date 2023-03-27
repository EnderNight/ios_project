#ifndef PARSE_H
#define PARSE_H

typedef struct item {
    char* text;
}item;

item open_item();
void read_string(int fd, item *it);
#endif