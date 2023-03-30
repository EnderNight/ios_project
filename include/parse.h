#ifndef PARSE_H
#define PARSE_H

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "item.h"

void open_item(Item *it);
void read_string(int fd, Item *it);
#endif
