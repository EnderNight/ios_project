#include <unistd.h>
#include <stdio.h>

void getlocation() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("You are in the street: %s\n", cwd);
}

