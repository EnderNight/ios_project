#include "cd.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int cd(int argc, char *argv[]) {

    // look at argv[1] and see if it is a directory
    // if it is a directory, then change to that directory
    // if it is not a directory, then print an error message
    // and return 1
    if (argc != 2) {
        change_color("red");
        print("Where are you going ?\n");
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
        if(strcmp(last+1,"story")!=0){
            chdir("..");
            return 0;
        }else{
            change_color("red");
            print("I can't go back anymore!\n");
            change_color("white");
            return 1;
        }
    }

    if (chdir(argv[1]) != 0) {
        change_color("red");
        print("This place does not exist !\n");
        change_color("white");
        return 1;
    }

    //Here, the access to the directory is a success
    //So we display the story with lseek depending on the directory
    //Only way is to do switch case with each directory
    //We implement it this way just to use lseek


    return 0;
}
