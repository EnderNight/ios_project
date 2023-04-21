#include<stdio.h> 
#include<unistd.h>
#include "cd.h"

int cd(int argc, char *argv[])
{
    char s[100];
 
    // printing current working directory
    printf("%s\n", getcwd(s, 100));
 
    //look at argv[1] and see if it is a directory
    //if it is a directory, then change to that directory
    //if it is not a directory, then print an error message
    //and return 1

    if (argc != 2) {
        printf("Error: cd takes one argument\n");
        return 1;
    }
    
    if (chdir(argv[1]) != 0) {
        printf("Error: %s is not a directory or \"..\"\n", argv[1]);
        return 1;
    }
    

    // printing current working directory
    printf("%s\n", getcwd(s, 100));
 
    // after chdir is executed
    return 0;
}
