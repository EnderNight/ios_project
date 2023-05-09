#include "utils.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        print_err("Incorrect number of arguments for rename. Usage: mv "
                  "<old_name> <new_name>\n");
        return 1;
    }

    int link_res, unlink_res;
    struct stat sb1;

    if (stat(argv[1], &sb1) == -1) {
        perror("rename");
        return 1;
    }

    if (S_ISREG(sb1.st_mode)) {
        link_res = link(argv[1], argv[2]);
        if (link_res == -1) {
            perror("rename");
            return 1;
        }

        unlink_res = unlink(argv[1]);
        if (unlink_res == -1) {
            perror("rename");
            return 1;
        }

        return 0;
    }

    print_err("rename can only rename files\n");
    return 1;
}
