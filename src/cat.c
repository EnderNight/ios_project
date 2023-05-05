#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define UNUSED(x) (void)(x);

int error(char *msg) {
    perror(msg);
    exit(-1);
}

int main(int argc, char **argv) {

    printf("%d\n", argc);

    if (argc < 1)
        return EXIT_FAILURE;

    int fd;
    char *res;
    size_t i, size, len, eof = 0;
    ssize_t ret = 0;

    for (int j = -1; j < argc -1; ++j) {

        if (argc == 1)
            fd = STDIN_FILENO;
        else {
            if (j == -1)
                j = 0;
            fd = open(argv[j + 1], O_RDONLY);
        }

        if (fd == -1)
            error("Cat");

        while (!eof) {

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

            if (ret == -1 || res == NULL)
                error("Cat");

            ++i;
            res = realloc(res, sizeof(char) * (i + 1));
            if (res == NULL)
                error("Cat");
            res[i] = '\0';

            len = strlen(res);
            ret = write(STDOUT_FILENO, res, len);
            if (ret != (ssize_t)len)
                error("Cat");

            free(res);
        }

        close(fd);
    }

    return EXIT_SUCCESS;
}
