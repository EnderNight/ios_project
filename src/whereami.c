#include "utils.h"

#include <stdio.h>
#include <unistd.h>


void whereami(void) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    print("You are in the street: %s\n", cwd);
}

int main(void) { whereami(); }
