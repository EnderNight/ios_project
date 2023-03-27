//
// Created by Sergio on 24/03/2023.
//


#include <stdio.h>
#include <stdlib.h> // For exit()
#include "parse.h"
#include <fcntl.h>
#include <unistd.h>

#define SECTION_TEXT_SIZE 100
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 1024

void inventory()
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    char buf[BUF_SIZE];
    int bytes_read;

    while ((bytes_read = read(fd, buf, BUF_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, bytes_read) != bytes_read) {
            perror("write");
            exit(1);
        }
    }

    if (bytes_read == -1) {
        perror("read");
        exit(1);
    }

    if (close(fd) == -1) {
        perror("close");
        exit(1);
    }
}
