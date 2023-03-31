#include <stdlib.h>

#include "defines.h"

#include "item.h"

Item *create_item(void) {

    Item *new_item = malloc(sizeof(Item));
    new_item->name = malloc(sizeof(char) * TEXT_SIZE);
    new_item->desc = malloc(sizeof(char) * TEXT_SIZE);
    new_item->usage = malloc(sizeof(char) * TEXT_SIZE);

    return new_item;
}

void clear_item(Item *item) {
    free(item->name);
    free(item->desc);
    free(item->usage);
}

void free_item(Item *item) {

    free(item->name);
    free(item->desc);
    free(item->usage);
    free(item);
}
