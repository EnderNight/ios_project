#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void readMan(char *path) {
    char buffer[4096];
    int length;
    int file = open(path, O_RDONLY);
    if (file == -1) {
        perror("Error opening file");
        return;
    }

    while ((length = read(file, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, length);
    }

    close(file);
}

int main(int argc, char *argv[]) {
    char path[512];
    if (getcwd(path, sizeof(path)) == NULL) {
        perror("Error getting current directory");
        return 1;
    }
    // We go to where all manuals are stored
    strcat(path, "/manuals/");

    // We check if the user has entered a command
    if (argc != 2 || argv[1] == NULL || strlen(argv[1]) == 0) {
        write(STDOUT_FILENO, "Here is what you can do:\n",
              strlen("Here is what you can do:\n"));
        write(STDOUT_FILENO, "\tcd\n\tls\n\tinventory\n\twhereami\n\n",
              strlen("\tcd\n\tls\n\tinventory\n\twhereami\n"));
        return 0;
    }

    // We match the command with the manual
    switch (argv[1][0]) {
    case 'c':
        strcat(path, "cd.txt");
        readMan(path);
        break;
    case 'l':
        strcat(path, "ls.txt");
        readMan(path);
        break;
    case 'i':
        strcat(path, "inventory.txt");
        readMan(path);
        break;
    case 'w':
        strcat(path, "whereami.txt");
        readMan(path);
        break;
    default:
        write(STDOUT_FILENO, "What is this ?\n", strlen("What is this ?\n"));
        return 0;
    }
}
