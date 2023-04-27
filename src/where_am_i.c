#include <stdio.h>
#include <unistd.h>

#include <utils.h>

void where_am_i(void) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    print("You are in the street: %s\n", cwd);
}

int main(void) { where_am_i(); }
