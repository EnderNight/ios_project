#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void readMan(char *path) {
    char buffer[4096];
    ssize_t length;
    int file = open(path, O_RDONLY);
    if (file == -1) {
        perror("Error opening file");
    }

    while ((length = read(file, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, (size_t)length);
    }

    close(file);
}

int main(int argc, char *argv[]) {
    char path[456];
    getcwd(path, sizeof path);

    char *p = strstr(path, "ios_project");
    p[0] = 0;
    strcat(path, "ios_project/data/manuals/");
    change_color("magenta");
    // We check if the user has entered a command
    if (argc != 2 || argv[1] == NULL || strlen(argv[1]) == 0) {
        print( "  __  __                         _     \n");
        print( " |  \\/  |                       | |    \n");
        print( " | \\  / | __ _ _ __  _   _  __ _| |___ \n");
        print( " | |\\/| |/ _\\` | '_ \\| | | |/ _\\`  |/__|\n");
        print( " | |  | | (_| | | | | |_| | (_| | \\__\\\n");
        print( " |_|  |_|\\__,_|_| |_|\\__,_|\\__,_|_|___/\n");
        print("\n");

        //List all the manuals
        print("Here is what you can do :\n");
        print("--   cd\n");
        print("--   ls\n");
        print("--   inventory\n");
        print("--   whereami\n");
        print("--   env\n");
        print("--   cat\n");
        print("--   rename\n");
        print("--   take\n");
        print("--   history\n");

        change_color("white");

        return 0;
    }

    // We match the command with the manual
    switch (argv[1][0]) {
    case 'c':
        //There is cd and cat, so we check
        if (argv[1][1] == 'd')
            strcat(path, "cd.txt");
        else
            strcat(path, "cat.txt");
        readMan(path);
        change_color("white");
        break;
    case 'l':
        strcat(path, "ls.txt");
        readMan(path);
        change_color("white");
        break;
    case 'i':
        strcat(path, "inventory.txt");
        readMan(path);
        change_color("white");
        break;
    case 'w':
        strcat(path, "whereami.txt");
        readMan(path);
        change_color("white");
        break;
    case 'e':
        strcat(path, "env.txt");
        readMan(path);
        change_color("white");
        break;
    case 'r':
        strcat(path, "rename.txt");
        readMan(path);
        change_color("white");
        break;
    case 't':
        strcat(path, "take.txt");
        readMan(path);
        change_color("white");
        break;
    case 'h':
        strcat(path, "history.txt");
        readMan(path);
        change_color("white");
        break;
    default:
        write(STDOUT_FILENO, "What is this ?\n", strlen("What is this ?\n"));
        return 0;
    }
}
