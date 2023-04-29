#include "cd.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void readScenario(char *directory) {
    char path [456];
    getcwd(path,sizeof path);

    char* p = strstr(path, "ios_project");
    p[0] = 0;
    strcat(path, "ios_project/data/scenario.txt");
    
    char buffer[4096];
    ssize_t length;
    int file = open(path, O_RDONLY);
    if (file == -1)
        perror("Error opening file");


    //We use lseek to go to the right place in the file
    //The offset is the number of characters in the file
    if (strcmp(directory, "start") == 0)
        lseek(file, 7, SEEK_SET);
    //ADD OTHER DIRECTORIES HERE
    
    //Now, we read at the offset until we see an empty line
    change_color("green");
    while ((length = read(file, buffer, sizeof(buffer))) > 0) {
        print(buffer);
        if (buffer[0] == '\n')
            break;
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
    
    //See if the user wants to go back when starting
    //He cannot, so block
    if (argv[1][0] == '.')
    {
        char cwd[456];
        getcwd(cwd, sizeof(cwd));
        char *last = strrchr(cwd, '/');
        if(strcmp(last+1,"start")!=0 && strcmp(last+1,"story")!=0){
            chdir("..");
            return 0;
        }else{
            change_color("red");
            print("I can't go back anymore!\n");
            change_color("white");
            return 0;
        }
    }

    if (chdir(argv[1]) != 0) {
        change_color("red");
        print("This place does not exist !\n");
        change_color("white");
        return 0;
    }

    //Here, the access to the directory is a success
    //So we display the story with lseek depending on the directory
    //Only way is to do switch case with each directory
    //We implement it this way just to use lseek
    readScenario(argv[1]);
    
    return 0;
}

