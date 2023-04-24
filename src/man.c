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
        write(STDOUT_FILENO, "Here is what you can do:\n", strlen("Here is what you can do:\n"));
        write(STDOUT_FILENO, "\tcd\n\tls\n\tinventory\n\twhereami\n\n", strlen("\tcd\n\tls\n\tinventory\n\twhereami\n"));
        return 0;
    }
    printf("You have this argument: %s\n", argv[0]);
    printf("You have this argument: %s\n", argv[1]);
    printf("You have this argument: %s\n", argv[2]);
    printf("%d\n", argc);

    return 0;
}
