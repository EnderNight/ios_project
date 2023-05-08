#ifndef UTILS_H
#define UTILS_H

#define PRINT_ERR "print_fd: write error: write did not print the whole text.\n"

int print(const char *format, ...);
int print_err(const char *format, ...);
void change_color(char * color);
#endif // UTILS_H
