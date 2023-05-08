#include "cd.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void readScenario() {
    // We get the current directory
    char path[456];
    getcwd(path, sizeof path);

    // We take the directory in which the player is
    char *directory = strdup(strrchr(path, '/'));
    directory++;

    // We get the path of the scenario file
    char *p = strstr(path, "ios_project");
    p[0] = 0;
    strcat(path, "ios_project/data/scenario.txt");

    int file = open(path, O_RDONLY);
    if (file == -1)
        perror("Is the project called ios_project or ios_project-main ?");

    // We use lseek to go to the right place in the file
    // The offset is the number of characters in the file
    // In order to get the offset, we execute this command on a terminal :
    // grep -b -o 'Directory' scenario.txt | awk '{print $1+length("Directory")
    // + 1}' We get the number of bytes before the word
    if (strcmp(directory, "start") == 0) {
        lseek(file, 6, SEEK_SET);
    }
    if (strcmp(directory, "roundabout") == 0) {
        lseek(file, 453, SEEK_SET);
    }
    if (strcmp(directory, "friends_house") == 0) {
        lseek(file, 1130, SEEK_SET);
    }
    if (strcmp(directory, "living_room") == 0) {
        lseek(file, 3907, SEEK_SET);
    }
    if (strcmp(directory, "bedroom") == 0) {
        lseek(file, 2011, SEEK_SET);
    }
    if (strcmp(directory, "do_nothing") == 0) {
        lseek(file, 2983, SEEK_SET);
    }
    if (strcmp(directory, "kitchen") == 0) {
        lseek(file, 4637, SEEK_SET);
    }
    if (strcmp(directory, "kill_him") == 0) {
        lseek(file, 5142, SEEK_SET);
    }
    if (strcmp(directory, "investigate") == 0) {
        lseek(file, 5860, SEEK_SET);
    }
    if (strcmp(directory, "parking") == 0) {
        lseek(file, 6686, SEEK_SET);
    }
    if (strcmp(directory, "road") == 0) {
        lseek(file, 7030, SEEK_SET);
    }
    if (strcmp(directory, "mall") == 0) {
        lseek(file, 7263, SEEK_SET);
    }
    if (strcmp(directory, "1f") == 0) {
        lseek(file, 7521, SEEK_SET);
    }
    if (strcmp(directory, "2f") == 0) {
        lseek(file, 7954, SEEK_SET);
    }
    if (strcmp(directory, "3f") == 0) {
        lseek(file, 8460, SEEK_SET);
    }
    // ADD OTHER DIRECTORIES HERE

    // Now, we read at the offset until we see an empty line
    change_color("green");
    char c;

    // Here is the logic for reading
    // We read character by character
    // First, we check if the player has the right group
    // Example : group 4 is required to read text, so we check if the player has
    // the group 4 If not, we keep reading until the group is 4, or we see a /
    // If there is no text with the group 4, we display nothing

    // If we have the right group, we print taking into account special
    // characters If we see a %, we change the color If we see a \n, we wait a
    // bit If we see a /, we stop reading Normal read
    while (read(file, &c, 1) == 1 && c != '/') {
        if (c == '%') {
            fflush(stdout);
            usleep(9000);
            change_color("yellow");
        } else {
            putchar(c);
            fflush(stdout);
            // If there is a newline, we wait
            if (c == '\n')
                // TODO : change this line when tests are gooood
                sleep(0.8);
            usleep(9000);
        }
    }
    change_color("white");

    close(file);
}

int cd(int argc, char *argv[]) {

    // look at argv[1] and see if it is a directory
    // if it is a directory, then change to that directory
    // if it is not a directory, then print an error message
    // and return 1
    if (argc != 2) {
        change_color("red");
        print("Where am I going ? 0_0\n");
        change_color("white");
        return 1;
    }

    // See if the user wants to go back when starting
    // He cannot, so block
    if (argv[1][0] == '.') {
        char cwd[456];
        getcwd(cwd, sizeof(cwd));
        char *last = strrchr(cwd, '/');
        if (strcmp(last + 1, "start") != 0 && strcmp(last + 1, "story") != 0) {
            chdir("..");
            // We display the story of the last directory
            // Put the pointer at the top
            print("\033[H");
            // Clear the shell screen
            print("\033[2J");
            readScenario();
            return 0;
        } else {
            change_color("red");
            print("I can't go back anymore!\n");
            change_color("white");
            return 0;
        }
    }

    // Check if the player has the right to access the directory
    // If not, print a small text
    // print("I can't do that !\n Maybe I'm missing something...\n")
    // The player can only access the directory if he has the right group
    // We use the stat function to get the group id

    if (chdir(argv[1]) != 0) {
        change_color("red");
        print("This place does not exist !\n");
        change_color("white");
        return 0;
    }

    // Here, the access to the directory is a success
    // So we display the story with lseek depending on the directory
    // Only way is to do switch case with each directory
    // We implement it this way just to use lseek
    readScenario();

    return 0;
}
