#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

void where_am_i(void) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char *current = basename(cwd);
    printf("You are at: %s\n", current);
}

int main(void) { where_am_i(); }
