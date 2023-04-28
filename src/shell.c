#include "shell.h"
#include "defines.h"
#include "utils.h"

#include "cd.h"

#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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

Variable *create_variable(char *name, char *value) {

    Variable *var = NULL;

    if (strlen(name) <= VAR_NAME_MAX_LENGTH &&
        strlen(value) <= VAR_VALUE_MAX_LENGTH) {
        var = malloc(sizeof(Variable));
        if (var == NULL) {
            perror("Create variable");
        } else {
            var->name = malloc(sizeof(char) * VAR_NAME_MAX_LENGTH + 1);
            if (var->name == NULL) {
                perror("Create variable name");
                var = NULL;
            } else {
                var->value = malloc(sizeof(char) * VAR_VALUE_MAX_LENGTH + 1);
                if (var->value == NULL) {
                    perror("Create variable value");
                    var = NULL;
                } else {
                    strcpy(var->name, name);
                    strcpy(var->value, value);
                }
            }
        }
    }

    return var;
}

void export(Shell *shell, char *name, char *value) {

    int ofm = 0;
    Variable *var;

    var = create_variable(name, value);
    if (var == NULL) {
        print_err("Export error: cannot create variable");
    } else {
        if (shell->env->size == shell->env->num) {
            shell->env->size *= 2;
            shell->env->list =
                realloc(shell->env->list,
                        sizeof(Variable *) * (size_t)shell->env->size);

            if (shell->env->list == NULL) {
                perror("Export error:");
                ofm = 1;
            }
        }

        if (!ofm) {
            shell->env->list[shell->env->num] = var;
            ++shell->env->num;
        }
    }
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
    if (shell->env == NULL) {
        perror("Init env");
    } else {
        shell->env->list = malloc(sizeof(Variable *) * 1);
        if (shell->env->list == NULL) {
            perror("Init env");
        } else {
            shell->env->num = 0;
            shell->env->size = 1;

            // Init path
            export(shell, "PATH", bin_path);

            // Init prompt
            export(shell, "PROMPT", PROMPT);

            free(bin_path);

            return 0;
        }
    }

    return 1;
}

int env(Shell *shell, int argc) {

    if (argc != 1) {
        print_err("Env error: env can not take arguements");
        return 1;
    }

    for (int i = 0; i < shell->env->num; ++i) {
        print("%s = %s\n", shell->env->list[i]->name,
              shell->env->list[i]->value);
    }

    return 0;
}

int terminate(Shell *shell) {
    shell->is_running = 0;

    return 0;
}

int find_variable(char *name, Variable **list, int num) {

    int i = 0;

    while (i < num && strcmp(name, list[i]->name) != 0) {
        ++i;
    }

    if (i >= num)
        return -1;

    return i;
}

char *find_command(Shell *shell, char *cmd_name) {

    char *res = NULL;
    int bin_index = find_variable("PATH", shell->env->list, shell->env->num);
    DIR *bindir;
    struct dirent *dent;

    if (bin_index == -1) {
        print_err("Find command error: PATH not found");
        return NULL;
    }

    bindir = opendir(shell->env->list[bin_index]->value);
    if (bindir == NULL) {
        perror("Find command");
        return NULL;
    }

    while ((dent = readdir(bindir)) != NULL &&
           strcmp(dent->d_name, cmd_name) != 0) {
    }

    if (dent == NULL) {
        return NULL;
    }

    res = malloc(sizeof(char) * (strlen(shell->env->list[bin_index]->value) +
                                 strlen(cmd_name)) +
                 2);
    if (res == NULL) {
        perror("Find command");
        return NULL;
    }

    strcpy(res, shell->env->list[bin_index]->value);
    strcat(res, "/");
    strcat(res, cmd_name);

    return res;
}

// Boot
Shell *sh_init(void) {

    Shell *shell = malloc(sizeof(Shell));
    int ret;

    ret = init_env(shell);

    if (ret < 0) {
        return NULL;
    }

    return shell;
}

// BUILTINS

char *builtin_str[] = {"cd", "env", "exit"};

int sh_cd(Shell *shell, int argc, char **args) {
    UNUSED(shell);
    return cd(argc, args);
}

int sh_env(Shell *shell, int argc, char **argv) {
    UNUSED(argv);
    return env(shell, argc);
}

int sh_exit(Shell *shell, int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    return terminate(shell);
}

int sh_num_builtins(void) { return sizeof(builtin_str) / sizeof(char *); }

int (*builtin_func[])(Shell *, int, char **) = {&sh_cd, &sh_env, &sh_exit};

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

int execute(Shell *shell, int argc, char *argv[]) {

    if (argc > 0) {

        pid_t child;
        int status;
        char *cmd = NULL;

        child = fork();

        switch (child) {

        case -1:
            error("fork");
            break;
        case 0:
            cmd = find_command(shell, argv[0]);

            if (cmd == NULL) {
                print_err("Command not found\n");
                break;
            }

            execvp(cmd, argv);
            free(cmd);
            break;
        default:
            wait(&status);
            break;
        }
    }

    return 0;
}

// Main loop

int sh_execute(Shell *shell, int argc, char *argv[]) {
    if (argc == 0)
        return 1;

    for (int i = 0; i < sh_num_builtins(); ++i) {
        if (strcmp(argv[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(shell, argc, argv);
    }

    return execute(shell, argc, argv);
}

int sh_loop(Shell *shell) {

    // int eof = 0;
    // int argc;
    // char *args[MAXARGS];
    //
    // while (shell->is_running) {
    //     print("%s", shell->env->list[1]->value);
    //     if (read_args(&argc, args, MAXARGS, &eof) && argc > 0)
    //             sh_execute(shell, argc, args);
    //     }
    //     if (eof)
    //         sh_exit(shell, argc, args);
    // 
    //
    // return 1;
    
    char buf[MAXLINE];
    int i = 0;

    while (read(STDIN_FILENO, buf + i, sizeof(char))) {
        ++i;
    }



    return 0;
}

// Exit
void sh_end(Shell *shell) {

    free_shell(shell);

}
