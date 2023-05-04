#include "cd.h"
#include "utils.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void readScenario() {
    //We get the current directory
    char path [456];
    getcwd(path,sizeof path);

    //We take the directory in which the player is
    char *directory = strdup(strrchr(path, '/'));
    directory++;

    //We get the path of the scenario file
    char* p = strstr(path, "ios_project");
    p[0] = 0;
    strcat(path, "ios_project/data/scenario.txt");

    int file = open(path, O_RDONLY);
    if (file == -1)
        perror("Is the project called ios_project or ios_project-main ?");

    //We use lseek to go to the right place in the file
    //The offset is the number of characters in the file
    //In order to get the offset, we execute this command on a terminal :
    //grep -b -o 'Directory' scenario.txt | awk '{print $1+length("Directory") + 1}'
    //We get the number of bytes before the word
    if (strcmp(directory, "start") == 0)
    {
        lseek(file, 6, SEEK_SET);
    }
    if (strcmp(directory, "roundabout") == 0)
    {
        lseek(file, 453, SEEK_SET);
    }
    if (strcmp(directory, "friends_house") == 0)
    {
        lseek(file, 1128, SEEK_SET);
    }
    
    //ADD OTHER DIRECTORIES HERE
    
    //Now, we read at the offset until we see an empty line
    change_color("green");
    char c;
    
    //Normal read
    while (read(file, &c, 1) == 1 && c != '/') {
        if (c == '%') 
        {
            fflush(stdout);
            usleep(9000);
            change_color("yellow");
        }
        else {
            putchar(c);
            fflush(stdout);
            //If there is a newline, we wait
            if (c == '\n')
                //TODO : change this line when tests are gooood
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
    
    //See if the user wants to go back when starting
    //He cannot, so block
    if (argv[1][0] == '.')
    {
        char cwd[456];
        getcwd(cwd, sizeof(cwd));
        char *last = strrchr(cwd, '/');
        if(strcmp(last+1,"start")!=0 && strcmp(last+1,"story")!=0){
            chdir("..");
            //We display the story of the last directory
            //Put the pointer at the top
            print("\033[H");
            //Clear the shell screen
            print("\033[2J");
            readScenario();
            return 0;
        }else{
            change_color("red");
            print("I can't go back anymore!\n");
            change_color("white");
            return 0;
        }
    }

    //Check if the player has the right to access the directory
    //If not, print a small text
    //The player can only access the directory if he has the right group
    //We use the stat function to get the group id
    struct stat fileStat;
    //Print content of fileStat



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
    readScenario();
    


    return 0;
}

