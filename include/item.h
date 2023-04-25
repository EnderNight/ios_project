#ifndef ITEM_H
#define ITEM_H

typedef struct Item {
    char *name;
    char *desc;
    char *usage;
} Item;

Item *load_item(char *path);
void free_item(Item *item);

#endif // !ITEM_H
