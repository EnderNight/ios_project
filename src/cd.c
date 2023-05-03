#include "cd.h"
#include <stdio.h>
#include <unistd.h>

#include <utils.h>

int cd(int argc, char *argv[]) {
    char s[100];

    // look at argv[1] and see if it is a directory
    // if it is a directory, then change to that directory
    // if it is not a directory, then print an error message
    // and return 1
    change_color("red");
    if (argc != 2) {
        print("Where are you going ?\n");
        change_color("white");
        return 1;
    }

    if (chdir(argv[1]) != 0) {
        print("This place does not exist !\n");
        change_color("white");
        return 1;
    }

    return 0;
}
