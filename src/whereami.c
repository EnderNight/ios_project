#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void whereami(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char *dir_name = strrchr(cwd, '/');
        change_color("cyan");
        print("I am in : %s\n", dir_name ? dir_name + 1 : cwd);
        change_color("white");
    } else {
        perror("getcwd() error");
    }
}

int main(void) { whereami(); }
