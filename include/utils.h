#ifndef UTILS_H
#define UTILS_H

#define PRINT_ERR "print_fd: write error: write did not print the whole text."

int print(const char *format, ...);
int print_err(const char *format, ...);

#endif // UTILS_H
