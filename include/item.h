#ifndef ITEM_H
#define ITEM_H

typedef struct Item {
    char *name;
    char *desc;
    char *usage;
} Item;

Item *create_item(void);
void load_item(Item *it, char *path);
void free_item(Item *item);

#endif // !ITEM_H
