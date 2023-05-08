#include "shell.h"
#include "ast.h"
#include "defines.h"
#include "input.h"
#include "parser.h"
#include "token.h"
#include "utils.h"

#include "cd.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
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
#define ReadEnd 0
#define WriteEnd 1

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

            // Init mulitline prompt
            export(shell, "MULTI_PROMPT", "> ");

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

/*
 * Search for a command inside PATH
 *
 * @shell: the current shell state
 * @cmd_name: the string command name
 *
 * Return: - NULL if the command is not found or there is an error
 *         - A pointer to the full command path
 */
char *find_command(Shell *shell, char *cmd_name) {

    char *res = NULL;
    int bin_index = find_variable("PATH", shell->env->list, shell->env->num);
    DIR *bindir;
    struct dirent *dent;

    if (bin_index != -1) {

        bindir = opendir(shell->env->list[bin_index]->value);
        if (bindir != NULL) {

            while ((dent = readdir(bindir)) != NULL &&
                   strcmp(dent->d_name, cmd_name) != 0) {
            }

            if (dent != NULL) {

                res = malloc(sizeof(char) *
                                 (strlen(shell->env->list[bin_index]->value) +
                                  strlen(cmd_name)) +
                             2);
                if (res == NULL)
                    perror("Find command");

                strcpy(res, shell->env->list[bin_index]->value);
                strcat(res, "/");
                strcat(res, cmd_name);
            }

            closedir(bindir);
        }
    }

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

    shell->is_running = 1;

    return shell;
}

// BUILTINS
char *builtin_str[] = {"cd", "env", "exit"};
int sh_num_builtins(void) { return sizeof(builtin_str) / sizeof(char *); }

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

int (*builtin_func[])(Shell *, int, char **) = {&sh_cd, &sh_env, &sh_exit};

int check_builtin(char *cmd) {

    int i = 0, found = 0;

    while (i < sh_num_builtins() && !found) {
        found = strcmp(cmd, builtin_str[i]) == 0;
        ++i;
    }

    return found;
}

int check_ast(Shell *shell, AST *ast, int *num_cmd) {

    char *cmd;

    if (ast->token->type == TOKEN_CMD) {
        if ((cmd = find_command(shell, ast->token->command[0])) != NULL ||
            check_builtin(ast->token->command[0])) {
            free(cmd);
            ++*num_cmd;
            return 1;
        }
        print_err("Syntax error: unknown command %s\n", ast->token->command[0]);
        return 0;
    }

    if (ast->token->type == TOKEN_REDIR)
        return check_ast(shell, ast->children[1], num_cmd);

    return check_ast(shell, ast->children[0], num_cmd) &&
           check_ast(shell, ast->children[1], num_cmd);
}

/*
 * Get the enterred command in stdin.
 * cmd must be freed after this function.
 *
 * Parameters:
 * @cmd: a string containing the command.
 *
 * Return: - NULL if there is an error, res is then equal to -1
 *         - A pointer to the command string, res is then equal to
 *               - 0 if read has reached EOF
 *               - 1 otherwise
 */
char *read_cmd(Shell *shell, int *res) {

    size_t i = 0, size = 1;
    ssize_t ret;
    unsigned char end = 0;
    char *cmd = malloc(sizeof(char));

    while (!end && (ret = read(STDIN_FILENO, cmd + i, sizeof(char))) == 1) {

        if (cmd[i] == '\n') {
            if (!i || cmd[i - 1] != '\\') {
                if (!i) {
                    cmd[i] = '\0';
                }
                end = 1;
            } else {
                print("%s", shell->env->list[2]->value);
                i -= 2;
            }
        }

        ++i;

        if (i == size) {
            size *= 2;
            cmd = realloc(cmd, sizeof(char) * size);
            if (cmd == NULL) {
                perror("read_cmd realloc");
                end = 1;
            }
        }
    }

    switch (ret) {
    case 0:
    case 1:
        cmd[i] = '\0';
        cmd = realloc(cmd, sizeof(char) * (i + 1));
        if (cmd == NULL) {
            perror("read_cmd realloc");
        }
        break;

    case -1:
        perror("read_cmd");
        free(cmd);
        cmd = NULL;
        break;
    }

    *res = (int)ret;
    return cmd;
}

int execute(Shell *shell, int argc, char *argv[], int p[2], int end) {

    if (argc > 0) {

        pid_t child;
        char *cmd = NULL;

        child = fork();

        switch (child) {

        case -1:
            error("fork");
            break;
        case 0:
            cmd = find_command(shell, argv[0]);

            if (p != NULL) {
                if (end == ReadEnd)
                    close(p[WriteEnd]);
                else
                    close(p[ReadEnd]);
            }

            execvp(cmd, argv);
            break;
        default:
            break;
        }
    }

    return 0;
}

// Main loop

int sh_execute(Shell *shell, int argc, char *argv[], int p[2], int end) {
    if (argc == 0)
        return 1;

    for (int i = 0; i < sh_num_builtins(); ++i) {
        if (strcmp(argv[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(shell, argc, argv);
    }

    return execute(shell, argc, argv, p, end);
}

int executor(AST *ast, Shell *shell, int init_in, int init_out, int fdin,
             int fdout, int p[2], int end) {

    if (ast->token->type == TOKEN_CMD) {

        // Redirection
        if (fdin == -1)
            fdin = dup(init_in);
        if (fdout == -1)
            fdout = dup(init_out);

        dup2(fdin, STDIN_FILENO);
        close(fdin);
        dup2(fdout, STDOUT_FILENO);
        close(fdout);

        return sh_execute(shell, (int)ast->token->len, ast->token->command, p,
                          end);
    }

    else if (ast->token->type == TOKEN_PIPE) {

        if (ast->children[1]->token->type == TOKEN_REDIR) {
            executor(ast->children[1], shell, init_in, init_out, fdin, fdout, p,
                     end);
            executor(ast->children[0], shell, init_in, init_out, fdin, fdout, p,
                     end);
        } else {
            int pipeFD[2];
            pipe(pipeFD);

            executor(ast->children[1], shell, init_in, init_out, fdin,
                     pipeFD[WriteEnd], pipeFD, WriteEnd);
            executor(ast->children[0], shell, init_in, init_out,
                     pipeFD[ReadEnd], fdout, pipeFD, ReadEnd);
        }
    }

    else if (ast->token->type == TOKEN_REDIR) {

        if (ast->children[0]->token->type == TOKEN_CMD) {
            int fd = open(ast->children[0]->token->command[0],
                          O_CREAT | O_TRUNC | O_WRONLY,
                          S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd == -1)
                error("Executor");

            executor(ast->children[1], shell, init_in, init_out, fdin, fd, p,
                     end);

            close(fd);
        }
    }

    return 0;
}

int init_executor(AST *ast, Shell *shell, int num_cmd) {

    int tmpin, tmpout, num = 0, status;

    tmpin = dup(STDIN_FILENO);
    tmpout = dup(STDOUT_FILENO);

    executor(ast, shell, tmpin, tmpout, -1, -1, NULL, -1);

    while (num < num_cmd) {
        wait(&status);
        ++num;
    }

    dup2(tmpin, STDIN_FILENO);
    dup2(tmpout, STDOUT_FILENO);
    close(tmpin);
    close(tmpout);

    return 0;
}

// TODO: check for errors
// TODO: free everything
int sh_loop(Shell *shell, int debug) {

    char *command = NULL;
    int res, num_cmd = 0;
    Input *in;
    Tokens *tokens, *rpn_tokens;
    AST *ast;

    while (shell->is_running) {
        print("%s", shell->env->list[1]->value);
        if ((command = read_cmd(shell, &res)) != NULL && res) {

            if (debug) {

                print("\nInit command: %s\n", command);

                in = init_input(command);
                if (separate(in)) {
                    print_input(in);

                    print("\nTokenization\n");
                    tokens = tokenize(in);
                    print_tokens(tokens);

                    print("\nRPN\n");
                    rpn_tokens = to_rpn(tokens);
                    if (check_rpn(rpn_tokens)) {
                        print_tokens(rpn_tokens);

                        print("\nAST\n");
                        ast = rpn_to_ast(rpn_tokens);
                        if (check_ast(shell, ast, &num_cmd)) {
                            print_ast(ast);

                            print("\nExecuting\n");
                            init_executor(ast, shell, num_cmd);
                        }
                        free_ast(ast);

                    } else
                        print_err("Error: invalid command.\n");

                    free_tokens(rpn_tokens);
                    free_tokens(tokens);
                }
                free_input(in);
                free(command);
            }

            else {

                in = init_input(command);
                if (separate(in)) {

                    tokens = tokenize(in);
                    rpn_tokens = to_rpn(tokens);

                    if (check_rpn(rpn_tokens)) {
                        ast = rpn_to_ast(rpn_tokens);

                        if (check_ast(shell, ast, &num_cmd))
                            init_executor(ast, shell, num_cmd);

                        free_ast(ast);

                    } else
                        print_err("Semantic error: invalid command.\n");

                    free_tokens(rpn_tokens);
                    free_tokens(tokens);
                }

                free_input(in);
                free(command);
            }
        }
        if (!res) {
            if (command != NULL)
                free(command);
            sh_exit(shell, 0, NULL);
        }
    }

    return 0;
}

// Exit
void sh_end(Shell *shell) { free_shell(shell); }
