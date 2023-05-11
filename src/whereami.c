#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void whereami(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char *dir_name = strstr(cwd, "story");
        change_color("cyan");
        print("I am in : %s\n", dir_name ? dir_name : cwd);
        change_color("white");
    } else {
        perror("getcwd() error");
    }
}

int main(void) { whereami(); }
