#include "utils.h"
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#define UNUSED(x) (void)(x);

struct termios orig_termios;

void disableRawMode(void) {

    struct termios role_back = orig_termios;

    tcgetattr(STDIN_FILENO, &role_back);
    role_back.c_lflag |= (ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &role_back);
}

void enableRawMode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int error(char *msg) {
    perror(msg);
    enableRawMode();
    exit(-1);
}


int is_directory(char *path) {

    struct stat sb;
    int res = 0, ret;

    if ((ret = stat(path, &sb)) == 0 && S_ISDIR(sb.st_mode))
        res = 1;

    if (ret == -1) {
        print_err("Error with file '%s'\n", path);
        error("cat: cannot get file info");
    }

    return res;
}

int main(int argc, char **argv) {

    if (argc < 1)
        return EXIT_FAILURE;

    tcgetattr(STDIN_FILENO, &orig_termios);
    disableRawMode();

    int fd;
    char *res;
    size_t i, size, len, eof;
    ssize_t ret = 0;

    for (int j = -1; j < argc - 1; ++j) {

        eof = 0;

        if (argc == 1)
            fd = STDIN_FILENO;
        else {
            if (j == -1)
                j = 0;

            if (!is_directory(argv[j + 1]))
                fd = open(argv[j + 1], O_RDONLY);
            else
                fd = -2;
        }

        if (ret == -1) {
            print_err("Error with file '%s'\n", argv[j + 1]);
            error("cat: cannot open file");
        }

        while (!eof && fd != -2) {

            res = malloc(sizeof(char));
            i = len = 0;
            size = 1;

            while (res != NULL && (ret = read(fd, res + i, sizeof(char))) > 0 &&
                   res[i] != '\n') {
                ++i;
                if (i == size) {
                    size *= 2;
                    res = realloc(res, sizeof(char) * (size + 1));
                }
            }

            if (ret == 0)
                eof = 1;

            if (ret == -1)
                error("cat: error while reading input");

            if (res == NULL)
                error("cat: not enough memory");

            ++i;
            res = realloc(res, sizeof(char) * (i + 1));
            if (res == NULL)
                error("cat: not enough memory");
            res[i] = '\0';

            len = strlen(res);
            ret = write(STDOUT_FILENO, res, len);
            if (ret != (ssize_t)len)
                error("cat: could not write whole text");

            free(res);
        }

        close(fd);
    }

    enableRawMode();
    return EXIT_SUCCESS;
}
