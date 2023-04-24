#include "shell.h"
#include "utils.h"

#include "cd.h"

// BUILTINS

char *builtin_str[] = {"cd"};

int sh_cd(int argc, char **args) { return cd(argc, args); }

int sh_num_builtins(void) { return sizeof(builtin_str) / sizeof(char *); }

int (*builtin_func[])(int, char **) = {&sh_cd};

// Input parsing
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
        print_err("Line too long -- removed command\n");
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
            } else if (strcmp(argv[0], "man") == 0) {
                cmd = "bin/man";
            } else {
                print_err("Command not found\n");
                exit(1);
            }

            execvp(cmd, argv);
            break;
        default:
            wait(&status);
            break;
        }

        free(cmd);
    }

    return 0;
}

int sh_execute(int argc, char *argv[]) {
    if (argc == 0)
        return 1;

    for (int i = 0; i < sh_num_builtins(); ++i) {
        if (strcmp(argv[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(argc, argv);
    }

    return execute(argc, argv);
}
