#ifndef PARSE_H
#define PARSE_H

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define SECTION_TEXT_SIZE 20
#define TEXT_SIZE 200

typedef struct item {
  char *text;
} item;

void open_item(item *it);
void read_string(int fd, item *it);
#endif
