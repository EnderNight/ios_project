//
// Created by Sergio on 24/03/2023.
//

#include "inventory.h"

void inventory() {
  int fd = open("tests/testdirs/inventory/items", O_RDONLY);
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
