#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void readMan(char *path)
{
    char buffer[4096];
    int length;

    int file = open(path, O_RDONLY);
    if (file == -1) {
        perror("Error opening file");
        return;
    }

    while ((length = read(file, buffer, sizeof(buffer))) > 0)
    {
        write(STDOUT_FILENO, buffer, length);
    }

    close(file);
}

int main (int argc, char *argv[])
{
    char path[512];
    if (getcwd(path, sizeof(path)) == NULL) {
        perror("Error getting current directory");
        return 1;
    }

    strcat(path, "/ios_project/manuals/");
    if (argc == 1 || argv[1] == NULL || strlen(argv[1]) == 0)
    {
        write(STDOUT_FILENO, "Here is what you can do:\n", 25);
        write(STDOUT_FILENO, "\tcd\n\tls\n\tinventory\n\twhereami\n\n", 32);
        return 0;
    }

    // Find the last word in the input string
    char *lastSpace = strrchr(argv[1], ' ');
    char *lastWord = lastSpace == NULL ? argv[1] : lastSpace + 1;

    // Get the last character of the last word
    char lastChar = lastWord[strlen(lastWord) - 1];

    switch (lastChar)
    {
        case 'd':
            strcat(path, "cd.txt");
            readMan(path);
            break;
        case 's':
            strcat(path, "ls.txt");
            readMan(path);
            break;
        case 'y':
            strcat(path, "inventory.txt");
            readMan(path);
            break;
        case 'i':
            strcat(path, "whereami.txt");
            readMan(path);
            break;
        default:
            write(STDOUT_FILENO, "Here is what you can do:\n", 25);
            write(STDOUT_FILENO, "\tcd\n\tls\n\tinventory\n\twhereami\n\n", 32);
            return 1;
    }

    return 0;
}
