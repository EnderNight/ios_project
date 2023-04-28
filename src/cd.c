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
    if (argv[1][0] == '.')
    {
        char cwd[456];
        getcwd(cwd, sizeof(cwd));
        char *last = strrchr(cwd, '/');
        if(strcmp(last+1,"tests")!=0){
            chdir("..");
            return 0;
        }else{
            puts("I can't go back anymore!");
            return 1;
        }
    }

    if (chdir(argv[1]) != 0) {
        change_color("red");
        print("This place does not exist !\n");
        change_color("white");
        return 1;
    }

    return 0;
}
