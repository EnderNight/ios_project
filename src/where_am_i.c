#include <stdio.h>
#include <unistd.h>

void where_am_i(void) {
    char cwd[1024];
    chdir("/path/to/change/directory/to");
    getcwd(cwd, sizeof(cwd));
    printf("Current working dir: %s\n", cwd);
}

int main(void)
{
    where_am_i();
}
