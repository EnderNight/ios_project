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
        print("\n");
        print("\n");
        print("MMMMMMMM               MMMMMMMM                                                                       lllllll                  \n");
        print("M:::::::M             M:::::::M                                                                       l:::::l                  \n");
        print("M::::::::M           M::::::::M                                                                       l:::::l                  \n");
        print("M:::::::::M         M:::::::::M                                                                       l:::::l                  \n");
        print("M::::::::::M       M::::::::::M  aaaaaaaaaaaaa  nnnn  nnnnnnnn    uuuuuu    uuuuuu    aaaaaaaaaaaaa    l::::l     ssssssssss   \n");
        print("M:::::::::::M     M:::::::::::M  a::::::::::::a n:::nn::::::::nn  u::::u    u::::u    a::::::::::::a   l::::l   ss::::::::::s  \n");
        print("M:::::::M::::M   M::::M:::::::M  aaaaaaaaa:::::an::::::::::::::nn u::::u    u::::u    aaaaaaaaa:::::a  l::::l ss:::::::::::::s \n");
        print("M::::::M M::::M M::::M M::::::M           a::::ann:::::::::::::::nu::::u    u::::u             a::::a  l::::l s::::::ssss:::::s\n");
        print("M::::::M  M::::M::::M  M::::::M    aaaaaaa:::::a  n:::::nnnn:::::nu::::u    u::::u      aaaaaaa:::::a  l::::l  s:::::s  ssssss \n");
        print("M::::::M   M:::::::M   M::::::M  aa::::::::::::a  n::::n    n::::nu::::u    u::::u    aa::::::::::::a  l::::l    s::::::s     \n");
        print("M::::::M    M:::::M    M::::::M a::::aaaa::::::a  n::::n    n::::nu::::u    u::::u   a::::aaaa::::::a  l::::l       s::::::s  \n");
        print("M::::::M     MMMMM     M::::::Ma::::a    a:::::a  n::::n    n::::nu:::::uuuu:::::u  a::::a    a:::::a  l::::l ssssss   s:::::s\n");
        print("M::::::M               M::::::Ma::::a    a:::::a  n::::n    n::::n u:::::::::::::::ua::::a    a:::::a l::::::ls:::::ssss::::::s\n");
        print("M::::::M               M::::::Ma:::::aaaa::::::a  n::::n    n::::n  uu::::::::uu:::u a:::::aaaa::::::a l::::::ls::::::::::::::s \n");
        print("M::::::M               M::::::M a::::::::::aa:::a n::::n    n::::n    uuuuuuuu  uuuu  aaaaaaaaaa  aaaallllllll  sssssssssss   \n");
        print("\n\n\n\n\n\n\n");

        //List all the manuals
        print("cd\n");
        print("ls\n");
        print("inventory\n");
        print("whereami\n");
        print("env\n");
        print("cat\n");
        print("rename\n");
        print("take\n");

        change_color("white");

        return 0;
    }

    // We match the command with the manual
    switch (argv[1][0]) {
    case 'c':
        strcat(path, "cd.txt");
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
    default:
        write(STDOUT_FILENO, "What is this ?\n", strlen("What is this ?\n"));
        return 0;
    }
}
