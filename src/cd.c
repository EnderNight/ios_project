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
    // grep -b -o 'Directory' scenario.txt | awk '{print
    // $1+length("Directory")}'
    // + 1}' We get the number of bytes before the word
    if (strcmp(directory, "start") == 0) {
        lseek(file, 6, SEEK_SET);
    }
    if (strcmp(directory, "roundabout") == 0) {
        lseek(file, 452, SEEK_SET);
    }
    if (strcmp(directory, "friends_house") == 0) {
        lseek(file, 1130, SEEK_SET);
    }
    if (strcmp(directory, "living_room") == 0) {
        lseek(file, 4156, SEEK_SET);
    }
    if (strcmp(directory, "bedroom") == 0) {
        lseek(file, 2011, SEEK_SET);
    }
    if (strcmp(directory, "do_nothing") == 0) {
        lseek(file, 2983, SEEK_SET);
    }
    if (strcmp(directory, "kitchen") == 0) {
        lseek(file, 4886, SEEK_SET);
    }
    if (strcmp(directory, "kill_him") == 0) {
        lseek(file, 5391, SEEK_SET);
    }
    if (strcmp(directory, "talk") == 0) {
        lseek(file, 3155, SEEK_SET);
    }
    if (strcmp(directory, "investigate") == 0) {
        lseek(file, 6109, SEEK_SET);
    }
    if (strcmp(directory, "parking") == 0) {
        lseek(file, 6910, SEEK_SET);
    }
    if (strcmp(directory, "mall") == 0) {
        lseek(file, 11245, SEEK_SET);
    }
    if (strcmp(directory, "1f") == 0) {
        lseek(file, 8732, SEEK_SET);
    }
    if (strcmp(directory, "2f") == 0) {
        lseek(file, 11504, SEEK_SET);
    }
    if (strcmp(directory, "3f") == 0) {
        lseek(file, 9672, SEEK_SET);
    }
    if (strcmp(directory, "car") == 0) {
        lseek(file, 7278, SEEK_SET);
    }
    if (strcmp(directory, "fix_car") == 0) {
        lseek(file, 10256, SEEK_SET);
    }
    if (strcmp(directory, "pass_them") == 0) {
        lseek(file, 7868, SEEK_SET);
    }
    // ADD OTHER DIRECTORIES HERE

    // Now, we read at the offset until we see an empty line
    change_color("green");
    char c;

    // Here is the logic for reading
    // We read character by character
    // If we see a %, we change the color If we see a \n, we wait
    // If we see a /, we stop reading
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
                sleep(0);
            usleep(0);
        }
    }

    // Here, we print the death messages, or win messages
    if (strcmp(directory, "do_nothing") == 0 ||
        strcmp(directory, "talk") == 0 || strcmp(directory, "pass_them") == 0) {
        change_color("red");
        print("\nYou died ! Go back with cd .. to try again !\n");
    }
    if (strcmp(directory, "fix_car") == 0) {
        change_color("red");
        print("\nYou won ! Good joob :)\n");
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

    if (argv[1][0] == '.') {
        char cwd[456];
        getcwd(cwd, sizeof(cwd));
        char *last = strrchr(cwd, '/');
        // We display the story of the last directory
        // Put the pointer at the top
        // See if the user wants to go back when starting
        // He cannot, so block
        if (strcmp(last + 1, "start") != 0 && strcmp(last + 1, "story") != 0) {
            chdir("..");
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

        // Before reading, we check the user's global rank
        // If he made a choice, check the directory he is in, and depending on
        // it we decide if we print the story or not

        // kill_him does not display if user_rank == 2
        // 2f, 1f does not display if user_rank == 3
    }


    if (chdir(argv[1]) != 0) {
        change_color("red");
        print("This place does not exist !\n");
        change_color("white");
        return 0;
    }

    // Here, the access to the directory is a success
    // So we display the story with lseek depending on the directory
    // We implement it this way just to use lseek
    readScenario();

    return 0;
}
