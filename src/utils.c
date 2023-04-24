#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Print @text to the corresponding file descriptor.
 *
 * @fildes: the file descriptor that receive @text
 * @text: the text to be print out.
 *
 * Returns: EXIT_SUCCESS if everything was printed out
 *          EXIT_FAILURE if there was a problem
 */
int print_fd(int filedes, char *text) {

    ssize_t text_len = (ssize_t)strlen(text);

    if (write(filedes, text, (size_t)text_len) < text_len) {
        write(STDERR_FILENO, PRINT_ERR, strlen(PRINT_ERR));

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
 * Print @text to stdout.
 *
 * @text: the text to be print out.
 *
 * Returns: EXIT_SUCCESS if everything was printed out
 *          EXIT_FAILURE if there was a problem
 */
int print(char *text) { return print_fd(STDOUT_FILENO, text); }

/*
 * Print @text to stderr.
 *
 * @text: the text to be print out.
 *
 * Returns: EXIT_SUCCESS if everything was printed out
 *          EXIT_FAILURE if there was a problem
 */
int print_err(char *text) { return print_fd(STDERR_FILENO, text); }

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
