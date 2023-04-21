// test_shell
//////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "cd.h"

#define error(a)                                                               \
    {                                                                          \
        perror(a);                                                             \
        exit(1);                                                               \
    };
#define MAXLINE 200
#define MAXARGS 20
#define MAXCMDLEN 100

// If you are on the server, remove the argument '-fsanitize=address' inside the
// Makefile file!!

/////////// reading commands:

int read_args(int *argcp, char *args[], int max, int *eofp) {
    static char cmd[MAXLINE];
    char *cmdp;
    int i;
    ssize_t ret;

    *argcp = 0;
    *eofp = 0;
    i = 0;
    while ((ret = read(0, cmd + i, 1)) == 1) {
        if (cmd[i] == '\n')
            break; // correct line
        i++;
        if (i >= MAXLINE) {
            ret = -2; // line too long
            break;
        }
    }
    switch (ret) {
    case 1:
        cmd[i + 1] = '\0'; // correct reading
        break;
    case 0:
        *eofp = 1; // end of file
        return 0;
        break;
    case -1:
        *argcp = -1; // reading failure
        fprintf(stderr, "Reading failure \n");
        return 0;
        break;
    case -2:
        *argcp = -1; // line too long
        fprintf(stderr, "Line too long -- removed command\n");
        return 0;
        break;
    }
    // Analyzing the line
    cmdp = cmd;
    for (i = 0; i < max; i++) { /* to show every argument */
        if ((args[i] = strtok(cmdp, " \t\n")) == (char *)NULL)
            break;
        cmdp = NULL;
    }
    if (i >= max) {
        fprintf(stderr, "Too many arguments -- removed command\n");
        return 0;
    }
    *argcp = i;
    return 1;
}

/////////// change shell color with system calls:
void change_color(char *color) {
    if (strcmp(color, "red") == 0) {
        write(0, "\033[0;31m", 7);
    } else if (strcmp(color, "green") == 0) {
        write(0, "\033[0;32m", 7);
    } else if (strcmp(color, "yellow") == 0) {
        write(0, "\033[0;33m", 7);
    } else if (strcmp(color, "blue") == 0) {
        write(0, "\033[0;34m", 7);
    } else if (strcmp(color, "magenta") == 0) {
        write(0, "\033[0;35m", 7);
    } else if (strcmp(color, "cyan") == 0) {
        write(0, "\033[0;36m", 7);
    } else if (strcmp(color, "white") == 0) {
        write(0, "\033[0;37m", 7);
    } else {
        fprintf(stderr, "Color not found\n");
    }
}

///////////////////////////////////////


int execute(int argc, char *argv[]);

// Builtin functions

int sh_cd(char **args);

char *builtin_str[] = {
    "cd"
};

int (*builtin_func[]) (char **) = {
    &sh_cd
};

int sh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}




int sh_cd(char **args)
{
    return cd(2, args);
}

int sh_execute(int argc, char *argv[])
{
    if (argc == 0)
        return 1;

    for (int i = 0; i < sh_num_builtins(); ++i) {
        if (strcmp(argv[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(argv);
    }

    return execute(argc, argv);
}





int execute(int argc, char *argv[]) {

    if (argc > 0) {

        pid_t child;
        int status;
        char *cmd;

        child = fork();
        cmd = malloc(sizeof(char) * MAXCMDLEN);

        switch (child) {

        case -1:
            error("fork");
            break;
        case 0:
            if (strcmp(argv[0], "inventory") == 0) {
                cmd = "bin/inventory";
            } else if (strcmp(argv[0], "where_am_i") == 0) {
                cmd = "bin/where_am_i";
            } else if (strcmp(argv[0], "ls") == 0) {
                cmd = "bin/ls";
            } else if (strcmp(argv[0], "cd") == 0) {

                char **test = malloc(sizeof(char *) * 2);
                test[0] = "cd";
                test[1] = "bin";

                cd(2, test);

                free(test);
                exit(0);
            } else {
                fprintf(stdout, "Command not found\n : %s", argv[0]);
                exit(1);
            }

            execlp(cmd, *argv);
            break;
        default:
            wait(&status);
            break;
        }

        free(cmd);
    }

    return 0;
}

int main(void) {
    //Clear the shell screen
    write(0, "\033[2J", 4);
    write(0, "\033[H", 3);

    //Change the shell color
    change_color("green");
    write(STDOUT_FILENO, "You woke up at 13 pm and were ready to start your day.\n", 56);
    sleep(2);
    write(STDOUT_FILENO, "As you listen to the news you realise that you have 24 hours left before the city gets nuked.\n", 94);
    sleep(2);
    write(STDOUT_FILENO, "“Sure whatever” you think.\n", 32);
    sleep(2);
    write(STDOUT_FILENO, "You go outside and realize that there is not a single person around.\n", 69);
    sleep(2);
    write(STDOUT_FILENO, "By the time you fully wake up, you witness bodies, blood, and fire. Nothing like usual.\n", 88);
    sleep(2);
    write(STDOUT_FILENO, "Despite feeling lost and confused, you tell yourself one thing :\n", 66);
    sleep(2);
    write(STDOUT_FILENO, "\n“I have to get out of here.”\n\n",36);

    //Execute the man page, to tell player what he can do
    //TODO

    //Change the shell color
    change_color("white");
    
    char *Prompt = "Outside> ";
    int eof = 0;
    int argc;
    char *args[MAXARGS];

    while (1) {
        write(0, Prompt, strlen(Prompt));
        if (read_args(&argc, args, MAXARGS, &eof) && argc > 0) {
            execute(argc, args);
        }
        if (eof)
            exit(0);
    }
}
