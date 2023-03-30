#include <stdlib.h>

#include "defines.h"

#include "item.h"

Item *create_item(void) {

    Item *new_item = malloc(sizeof(Item));
    new_item->text = malloc(sizeof(char) * TEXT_SIZE);

    return new_item;
}

void clear_item(Item *item) { free(item->text); }

void free_item(Item *item) {

    free(item->text);
    free(item);
}
