#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_ERR "print_fd: write error: write did not print the whole text."




/*
* Print @text to the corresponding file descriptor.
*
* @fildes: the file descriptor that receive @text
* @text: the text to be print out.
*
* Returns: EXIT
*/
int print_fd(int filedes, char *text) {

    ssize_t text_len = (ssize_t) strlen(text);

    if (write(filedes, text, (size_t) text_len) < text_len) {
        write(STDERR_FILENO, PRINT_ERR, strlen(PRINT_ERR));

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
