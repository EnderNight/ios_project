#ifndef PARSE_H
#define PARSE_H

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "item.h"

void open_item(Item *it, char *path);
#endif
