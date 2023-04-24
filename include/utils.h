#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PRINT_ERR "print_fd: write error: write did not print the whole text."

int print(char *text);
int print_err(char *text);

#endif // UTILS_H
