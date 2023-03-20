#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>




#define INVENTORY_SIZE 5
#define ITEM_NAME_LENGTH 20


typedef struct itemList {

    char **list;
    int len;

} itemList;





itemList* get_item_list() {
    
    int items = open("testdirs/inventory/items", O_RDONLY);
    int numChar;
    int numItem;

    char buf;
    char **list;
    char item_name[ITEM_NAME_LENGTH];

    list = malloc(sizeof(char*) * INVENTORY_SIZE);
    numChar = numItem = 0;

    while (read(items, &buf, 1) > 0) {

        if (buf == '\n') {
            list[numItem] = malloc(sizeof(char) * numChar);
            item_name[numChar] = '\0';
            strcpy(list[numItem], item_name);
            ++numItem;
            numChar = 0;
        } else {
            item_name[numChar] = buf;
            ++numChar;
        }

    }

    itemList *res = malloc(sizeof(itemList));
    res->list = list;
    res->len = numItem;

    return res;

}





void print_items(itemList *items) {

    for (int i = 0; i < items->len; ++i) {
        printf("%i - %s\n", i, items->list[i]);
    }

}
