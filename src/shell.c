#include "shell.h"
#include "utils.h"

#include "cd.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define PROMPT "Outside> "
#define BINDIR "bin"
#define VAR_NAME_MAX_LENGTH 20
#define VAR_VALUE_MAX_LENGTH 100

// Utils

void free_variable(Variable *var) {

    free(var->name);
    free(var->value);

    free(var);
}



void free_env(ENV *env) {

    for (int i = 0; i < env->num; ++i) {
        free_variable(env->list[i]);
    }

    free(env->list);
    free(env);
}

void free_shell(Shell *shell) {

    free_env(shell->env);

    free(shell);
}



Variable* create_variable(char *name, char *value) {

    Variable *var; 

    if (strlen(name) > VAR_NAME_MAX_LENGTH || strlen(value) > VAR_VALUE_MAX_LENGTH) {
        var = NULL;
    } else {

        var = malloc(sizeof(Variable));

        var->name = malloc(sizeof(char) * VAR_NAME_MAX_LENGTH +1);
        var->value = malloc(sizeof(char) * VAR_VALUE_MAX_LENGTH +1);

        strcpy(var->name, name);
        strcpy(var->value, value);
    }
    
    return var;

}



int init_env(Shell *shell) {

    // Get absolute path of bin/ dir 
    char *bin_path;
    bin_path = realpath(BINDIR, NULL);

    if (bin_path == NULL) {
        perror("Init env");
        return -1;
    }

    // Init env
    shell->env = malloc(sizeof(ENV));
    shell->env->list = malloc(sizeof(Variable *) * 4);

    shell->env->num = 2;
    shell->env->size = 4;

    // Init path
    Variable *path = create_variable("PATH", bin_path);
    shell->env->list[0] = path;

    // Init prompt
    Variable *prompt = create_variable("PROMPT", PROMPT);
    shell->env->list[1] = prompt;

    free(bin_path);

    return 0;
}



// Boot
Shell* sh_init(void) {

    Shell *shell = malloc(sizeof(Shell));
    int ret;

    ret = init_env(shell);

    if (ret < 0) {
        return NULL;
    }

    return shell;
}











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




// Main loop

int sh_execute(int argc, char *argv[]) {
    if (argc == 0)
        return 1;

    for (int i = 0; i < sh_num_builtins(); ++i) {
        if (strcmp(argv[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(argc, argv);
    }

    return execute(argc, argv);
}



int sh_loop(Shell *shell) {

    int eof = 0;
    int argc;
    char *args[MAXARGS];
    int active = 1;

    while (active) {
        print("%s", shell->env->list[1]->value);
        if (read_args(&argc, args, MAXARGS, &eof) && argc > 0) {
            if (strcmp(args[0], "exit") == 0) {
                free_shell(shell);
                active = 0;
            } else {
                sh_execute(argc, args);
            }
        }
        if (eof)
            active = 0;
    }

    return 1;
}







// Exit

int sh_exit(Shell *shell) {

    free_shell(shell);

    return 0;
}
