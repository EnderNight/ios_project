#include "utils.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Print @text to stdout.
 *
 * @format: the text to be printed out, in printf like format.
 *
 * Returns: EXIT_SUCCESS if everything was printed out
 *          EXIT_FAILURE if there was a problem
 */
int print_fd(int filedes, const char *fmt, va_list args) {

    va_list args_copy;
    int text_len, res;
    char *buf;

    va_copy(args_copy, args);

    text_len = vsnprintf(NULL, 0, fmt, args_copy) + 1;
    va_end(args_copy);
    if (text_len < 0) {
        write(STDERR_FILENO, PRINT_ERR, strlen(PRINT_ERR));

        return EXIT_FAILURE;
    }
    buf = malloc(sizeof(char) * (size_t)text_len);
    res = vsnprintf(buf, (size_t)text_len, fmt, args);

    va_end(args);

    if (res < 0 || write(filedes, buf, (size_t)text_len) < (ssize_t)text_len) {
        write(STDERR_FILENO, PRINT_ERR, strlen(PRINT_ERR));

        return EXIT_FAILURE;
    }

    free(buf);

    return EXIT_SUCCESS;
}

/*
 * Print @text to stdout.
 *
 * @format: the text to be printed out, in printf like format.
 *
 * Returns: EXIT_SUCCESS if everything was printed out
 *          EXIT_FAILURE if there was a problem
 */
int print(const char *format, ...) {

    va_list args;
    int res;

    va_start(args, format);

    res = print_fd(STDOUT_FILENO, format, args);

    va_end(args);

    return res;
}

/*
 * Print @text to stderr.
 *
 * @format: the text to be print out, in printf like format.
 *
 * Returns: EXIT_SUCCESS if everything was printed out
 *          EXIT_FAILURE if there was a problem
 */
int print_err(const char *format, ...) {

    va_list args;
    int res;

    va_start(args, format);

    res = print_fd(STDERR_FILENO, format, args);

    va_end(args);

    return res;
}

/*
 * Change character colors
 */
void change_color(char *color) {
    if (strcmp(color, "red") == 0) {
        write(0, "\033[0;31m", 7);
    } else if (strcmp(color, "green") == 0) {
        write(0, "\033[0;32m", 7);
    } else if (strcmp(color, "yellow") == 0) {
        write(0, "\033[0;33m", 7);
    } else if (strcmp(color, "blue") == 0) {
        write(0, "\033[0;34m", 7);
    } else if (strcmp(color, "magenta") == 0) {
        write(0, "\033[0;35m", 7);
    } else if (strcmp(color, "cyan") == 0) {
        write(0, "\033[0;36m", 7);
    } else if (strcmp(color, "white") == 0) {
        write(0, "\033[0;37m", 7);
    } else {
        fprintf(stderr, "Color not found\n");
    }
}
