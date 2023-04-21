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

///////////////////////////////////////

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
                char *test = "bin";
                cd(2, &test);
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
    char *Prompt = "myShell0> ";
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
