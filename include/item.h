#ifndef ITEM_H
#define ITEM_H

#include <stdlib.h>

typedef struct Item {
    char *name;
    char *desc;
    char *usage;
} Item;

Item *create_item(void);
void clear_item(Item *item);
void free_item(Item *item);

#endif // !ITEM_H