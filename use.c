#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

void list_items() {
    
    int items = open("testdirs/inventory/items", O_RDONLY);
    char buf;

    while (read(items, &buf, 1) > 0)
        printf("%c", buf);

}
