#include "shell.h"
#include "ast.h"
#include "defines.h"
#include "input.h"
#include "parser.h"
#include "token.h"
#include "utils.h"

#include "cd.h"
#include "item.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h> // waitpid
#include <termios.h>
#include <unistd.h>

#define PROMPT "Outside"
#define BINDIR "bin"
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

    var = malloc(sizeof(Variable));
    if (var == NULL) {
        perror("Create variable");
    } else {
        var->name = malloc(sizeof(char) * (strlen(name) + 1));
        if (var->name == NULL) {
            perror("Create variable name");
            var = NULL;
        } else {
            var->value = malloc(sizeof(char) * (strlen(value) + 1));
            if (var->value == NULL) {
                perror("Create variable value");
                var = NULL;
            } else {
                strcpy(var->name, name);
                strcpy(var->value, value);
            }
        }
    }

    return var;
}

int export(Shell *shell, char *name, char *value) {

    Variable *var;

    var = create_variable(name, value);
    if (var == NULL) {
        print_err("Export error: cannot create variable");
        return 0;
    } else {
        if (shell->env->size == shell->env->num) {
            shell->env->size *= 2;
            shell->env->list =
                realloc(shell->env->list,
                        sizeof(Variable *) * (size_t)shell->env->size);

            if (shell->env->list == NULL)
                error("Export error:");
        }

        shell->env->list[shell->env->num] = var;
        ++shell->env->num;
    }

    return 1;
}

int init_history(Shell *shell) {

    int res1, res2,
        hist_fd = open("story/.history", O_CREAT | O_TRUNC | O_WRONLY,
                       S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    char *hist_path;

    if (hist_fd == -1)
        error("init_history: create .history");

    close(hist_fd);

    hist_path = realpath("story/.history", NULL);

    if (hist_path == NULL)
        error("init_history");

    res1 = export(shell, "HISTORY", hist_path);
    free(hist_path);

    res2 = export(shell, "NUM_CMD", "0");

    return res1 && res2;
}

int init_inventory(Shell *shell) {

    int res;
    char *inv_path;

    inv_path = realpath("game_directories/inventory", NULL);

    if (inv_path == NULL) 
        error("init_inventory");

    res = export(shell, "INVENTORY", inv_path);
    free(inv_path);

    return res;
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
        error("Init env");
    } else {
        shell->env->list = malloc(sizeof(Variable *));
        if (shell->env->list == NULL) {
            error("Init env");
        } else {
            shell->env->num = 0;
            shell->env->size = 1;

            // Init path
            export(shell, "PATH", bin_path);

            // Init prompt
            export(shell, "PROMPT", PROMPT);

            // Init mulitline prompt
            export(shell, "MULTI_PROMPT", "> ");

            // User rank
            export(shell, "USER_RANK", "0");

            // History
            init_history(shell);

            // Inventory
            init_inventory(shell);

            free(bin_path);

            return 0;
        }
    }

    return 1;
}

int env(Shell *shell, int argc) {

    if (argc != 1) {
        print_err("Env error: env can not take arguements.\n");
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

int change_variable_value(Shell *shell, char *new_value, int index) {

    shell->env->list[index]->value =
        realloc(shell->env->list[index]->value, strlen(new_value) + 1);
    strcpy(shell->env->list[index]->value, new_value);

    return 1;
}

int history(Shell *shell) {

    int num_cmd = 0, eof = 0, size = 1, i = 0, hist_fd,
        hist_ind = find_variable("HISTORY", shell->env->list, shell->env->num),
        cur_num_cmd_ind =
            find_variable("NUM_CMD", shell->env->list, shell->env->num),
        cur_num_cmd;
    char *buf;
    ssize_t ret_read;

    if (hist_ind != -1 && cur_num_cmd_ind != -1) {
        hist_fd = open(shell->env->list[hist_ind]->value, O_APPEND | O_RDONLY);
        cur_num_cmd = atoi(shell->env->list[cur_num_cmd_ind]->value);
        buf = malloc(sizeof(char));
        if (buf == NULL)
            error("history");
        if (cur_num_cmd > 10) {
            while (cur_num_cmd > 10 && (ret_read = read(hist_fd, buf, 1)) > 0) {
                if (buf[0] == '\n')
                    --cur_num_cmd;
            }
            if (ret_read == -1)
                error("history");
        }
        while (!eof) {
            while (num_cmd < 10 && (ret_read = read(hist_fd, buf + i, 1)) > 0) {
                if (buf[i] == '\n') {
                    ++i;
                    buf = realloc(buf, sizeof(char) * (size_t)(i + 1));
                    buf[i] = '\0';
                    print("%d - %s", num_cmd, buf);
                    free(buf);
                    buf = malloc(sizeof(char));
                    i = 0;
                    size = 1;
                    ++num_cmd;
                }

                else {
                    ++i;
                    if (i == size) {
                        size *= 2;
                        buf = realloc(buf, sizeof(char) * (size_t)size);
                    }
                }
            }

            if (num_cmd >= 10 || ret_read == 0)
                eof = 1;
            if (ret_read == -1)
                error("history");
        }

        close(hist_fd);
        free(buf);
    }

    return 0;
}

int take(Shell *shell) {
    int user_rank_index =
        find_variable("USER_RANK", shell->env->list, shell->env->num);
    int user_rank = atoi(shell->env->list[user_rank_index]->value);

    //fprintf(stderr, "user_rank: %d\n", user_rank);

    char *initial_path;
    if ((initial_path = getcwd(NULL, 0)) != NULL) {
        //char *dir_name = strrchr(initial_path, '/');
        //change_color("cyan");
        //print("I am in : %s\n", dir_name ? dir_name + 1 : initial_path);
        //change_color("white");
    } else {
        error("getcwd() error");
    }

    //fprintf(stderr, "initial_path: %s\n", initial_path);

    int inv_index =
        find_variable("INVENTORY", shell->env->list, shell->env->num);
    char *dest_path = shell->env->list[inv_index]->value;

    DIR *dir;
    struct dirent *entry;
    char *filename, *source_path, *final_path;

    // open initial path
    dir = opendir(initial_path);
    if (dir == NULL) {
        fprintf(stderr, "Error opening the initial path\n");
        return -1;
    }

    bool found_item = false;

    // iterate over the files in the directory
    while ((entry = readdir(dir)) != NULL) {
        filename = malloc(sizeof(char) * (strlen(entry->d_name) + 1));
        strcpy(filename, entry->d_name);

        // check if the file is an item
        if (strcmp(filename, "knife.item") == 0 ||
            strcmp(filename, "flamethrower.item") == 0 ||
            strcmp(filename, "car_battery.item") == 0) {

            source_path = malloc(sizeof(char) *
                                 (strlen(initial_path) + strlen(filename) + 2));
            final_path = malloc(sizeof(char) *
                                (strlen(dest_path) + strlen(filename) + 2));

            strcpy(source_path, initial_path);
            strcpy(final_path, dest_path);

            strcat(source_path, "/");
            strcat(source_path, filename);

            strcat(final_path, "/");
            strcat(final_path, filename);

            // move the item to the inventory

            int link_res, unlink_res;
            struct stat sb1;

            // fprintf(stderr, "source_path: %s\n", source_path); //variable
            // check fprintf(stderr, "dest_item_path: %s\n", final_path);
            // //variable check

            if (stat(source_path, &sb1) == -1) {
                perror("couldn't get source_path stat");
                return 1;
            }

            if (S_ISREG(sb1.st_mode)) {
                link_res = link(source_path, final_path);
                if (link_res == -1) {
                    perror("couldn't link");
                    return 1;
                }

                unlink_res = unlink(source_path);
                if (unlink_res == -1) {
                    perror("couldn't unlink");
                    return 1;
                }
            }

            found_item = true;
            change_color("cyan");
            printf("You took: %s\n", filename);
            change_color("white");

            // Increment user rank
            if (strcmp(filename, "knife.item") == 0 && user_rank == 0) {
                user_rank = 1;
            } else if (strcmp(filename, "flamethrower.item") == 0 &&
                       user_rank == 1) {
                user_rank = 2;
            } else if (strcmp(filename, "car_battery.item") == 0 &&
                       user_rank == 2) {
                user_rank = 3;
            }

            free(source_path);
            free(final_path);
        }
        free(filename);
    }

    //user_rank to char
    char rank_str[2];
    sprintf(rank_str, "%d", user_rank);
    change_variable_value(shell, rank_str, user_rank_index);
    closedir(dir);

    // if no items were found
    if (!found_item) {
        printf("There is nothing here.\n");
    }
    free(initial_path);
    return 0;
}

int inventory(Shell *shell) {

    int inv_index =
        find_variable("INVENTORY", shell->env->list, shell->env->num);
    char *dir_name = shell->env->list[inv_index]->value;

    DIR *dir;
    struct dirent *entry;
    Item *item;
    char *ext, *item_path;

    if ((dir = opendir(dir_name)) == NULL) {
        perror("opendir error");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if ((ext = strstr(entry->d_name, ".item")) !=
            NULL) { // looks for a sub-string inside a string

            item_path = malloc(sizeof(char) * (strlen(dir_name) + strlen(entry->d_name) + 2));
            strcpy(item_path, dir_name);
            strcat(item_path, "/");
            strcat(item_path, entry->d_name);

            item = load_item(item_path);

            if (item != NULL) {
                print("Name: %s\n", item->name);
                print("Desc: %s\n", item->desc);
                print("Usage: %s\n", item->usage);
                free_item(item);
            }

            free(item_path);
        }
    }
    closedir(dir);
    return 0;
}

int num_digits(int num) {

    int res = 1;

    while (num != 0) {
        num /= 10;
        ++res;
    }

    return res;
}

int save_history(Shell *shell, char *cmd) {

    int hist_fd,
        hist_ind = find_variable("HISTORY", shell->env->list, shell->env->num),
        cur_num_cmd_ind =
            find_variable("NUM_CMD", shell->env->list, shell->env->num),
        cur_num_cmd;
    char *buf;

    if (hist_ind != -1 && cur_num_cmd_ind != -1) {

        hist_fd = open(shell->env->list[hist_ind]->value, O_APPEND | O_WRONLY);
        cur_num_cmd = atoi(shell->env->list[cur_num_cmd_ind]->value);

        if (hist_fd != -1) {
            if (write(hist_fd, cmd, strlen(cmd)) < (ssize_t)strlen(cmd)) {
                error("save_history");
            } else {
                ++cur_num_cmd;

                buf = malloc(sizeof(char) *
                             (size_t)(num_digits(cur_num_cmd) + 1));
                sprintf(buf, "%d", cur_num_cmd);

                change_variable_value(shell, buf, cur_num_cmd_ind);
                free(buf);
            }
        }
        return 0;
    }

    print_err("HISTORY variable not found.\n");
    return 1;
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

void print_prompt(Shell *shell, int prompt) {

    int index;
    char *prmt;

    if (prompt == 1)
        prmt = "PROMPT";
    else
        prmt = "MULTI_PROMPT";

    index = find_variable(prmt, shell->env->list, shell->env->num);
    if (index != -1) {
        change_color("bigcyan");
        print("%s", shell->env->list[index]->value);
        change_color("white");
        print("> ");
    }

}

int change_prompt(Shell *shell) {

    int index = find_variable("PROMPT", shell->env->list, shell->env->num);
    char *cwd, *cwd_;

    if (index != -1) {
        cwd = getcwd(NULL, 0);

        if (cwd != NULL) {
            cwd_ = strrchr(cwd, '/');
            if (cwd_ != NULL) {
                int res = change_variable_value(shell, cwd_ + 1, index);
                free(cwd);
                return res;
            }
            free(cwd);
        }
    }
    perror("change_prompt");
    return 1;
}


// BUILTINS
char *builtin_str[] = {"cd",      "env",  "exit",     "export",
                       "history", "take", "inventory"};
int sh_num_builtins(void) { return sizeof(builtin_str) / sizeof(char *); }

int sh_cd(Shell *shell, int argc, char **args) {

    // Check if the player has the right to access the directory
    // If not, print a small text
    // print("I can't do that !\n Maybe I'm missing something...\n")
    // The player can only access the directory if he has the right group

    // kill_him needs to have user_rank == 1
    // 3f needs to have user_rank == 2
    // fix_car needs to have user_rank == 3

    int user_rank_index =
        find_variable("USER_RANK", shell->env->list, shell->env->num);
    int user_rank = atoi(shell->env->list[user_rank_index]->value);

    //check if the player tries to go in kill_him
    if (strcmp(args[1], "kill_him") == 0) {
        if (user_rank != 1) {
            print("I have no weapons !\n");
            change_color("magenta");
            print("Maybe I'm missing something...\n");
            change_color("white");
            return 1;
        }
    }

    //check if the player tries to go in 3f
    if (strcmp(args[1], "3f") == 0) {
        if (user_rank != 2) {
            print("Too many zombies, I don't want to die !\n");
            change_color("magenta");
            print("Maybe I'm missing something...\n");
            change_color("white");
            return 1;
        }
    }

    //check if the player tries to go in fix_car
    if (strcmp(args[1], "fix_car") == 0) {
        if (user_rank != 3) {
            print("I can't do that !\n");
            change_color("magenta");
            print("Maybe I'm missing something...\n");
            change_color("white");
            return 1;
        }
    }

    if (args[1][0] == '.') 
    {
        //get path if we go back
        char cwd[456];
        getcwd(cwd, sizeof(cwd));
        char *last = strrchr(cwd, '/');
        //if the player is in investigate, when going back to kill_him, we will not print text
        //to check that, we see if the player is rank 2
        if (strcmp(last, "/investigate") == 0) 
        {
            change_color("green");
            print("He is still there...\n");
            sleep(1);
            print("Lying on the ground...\n");
            //go to start
            chdir("..");
            change_color("white");
            return 1;
        }

        if (strcmp(last, "/kill_him") == 0 || strcmp(last, "/3f") == 0 || strcmp(last, "/fix_car") == 0)
        {
            chdir("..");
            return 1;
        }
        if (strcmp(last, "/friends_house") == 0 || strcmp(last, "/roundabout") == 0)
        {
            change_color("green");
            print("I'm back to where it started\n");
            change_color("white");
            chdir("..");
            return 1;
        }
    }

    return !cd(argc, args) && change_prompt(shell);
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

int sh_export(Shell *shell, int argc, char **argv) {

    if (argc == 3) {
        int index;
        if ((index = find_variable(argv[1], shell->env->list,
                                   shell->env->num)) != -1)
            return change_variable_value(shell, argv[2], index);
        return export(shell, argv[1], argv[2]);
    }

    print_err("Incorrect number of parameters. Usage: export <VAR_NAME> "
              "<VAR_VALUE>\n");
    return 1;
}

int sh_history(Shell *shell, int argc, char **argv) {

    if (argc == 1) {
        UNUSED(argv);
        return history(shell);
    }

    print_err("History does not take arguments. Usage: history\n");
    return 1;
}

int sh_take(Shell *shell, int argc, char **argv) {

    if (argc == 1) {
        UNUSED(argv);
        return take(shell);
    }

    print_err("take doesnt have items. Usage: take\n");
    return 1;
}

int sh_inventory(Shell *shell, int argc, char **argv) {

    if (argc == 1) {
        UNUSED(argv);
        return inventory(shell);
    }

    print_err("inventory error\n");
    return 1;
}

int (*builtin_func[])(Shell *, int, char **) = {
    &sh_cd,      &sh_env,  &sh_exit,     &sh_export,
    &sh_history, &sh_take, &sh_inventory};

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
    int res;

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

    if (ast->token->type == TOKEN_AMPER) {
        res = check_ast(shell, ast->children[0], num_cmd);
        if (ast->num_children > 1) {
            res = res && check_ast(shell, ast->children[1], num_cmd);
        }
        return res;
    }

    return check_ast(shell, ast->children[0], num_cmd) &&
           check_ast(shell, ast->children[1], num_cmd);
}

int is_cmd(Shell *shell, char *cmd) {

    int res = 0;
    char *tmp;

    if ((tmp = find_command(shell, cmd)) != NULL) {
        free(tmp);
        res = 1;
    } else if (check_builtin(cmd))
        res = 1;

    return res;
}

void parse_input(Shell *shell, int cur_prompt, char *input, size_t start_index,
                 size_t cmd_len, int mutli) {

    char tmp[cmd_len + 1];
    strncpy(tmp, input, cmd_len);
    tmp[cmd_len] = '\0';

    change_color("bigcyan");
    print("\33[2K\r%s", shell->env->list[cur_prompt]->value);
    change_color("white");
    print("> ");

    if (!mutli) {
        if (is_cmd(shell, tmp)) {
            change_color("green");
        } else
            change_color("red");

        print("%s", tmp);
    }

    change_color("white");
    // print("%d - %d", start_index, cmd_len);

    if (mutli)
        print("%s", input + start_index);
    else
        print("%s", input + cmd_len);
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

    int cur_prompt = 1, sep = 0, i = 0, mutli = 0;
    size_t size = 1, multi_index = 0, cmd_len = 0;
    ssize_t ret;
    unsigned char end = 0;
    char *cmd = malloc(sizeof(char));

    while (!end) {

        ret = read(STDIN_FILENO, cmd + i, sizeof(char));

        if (ret) {
            if (cmd[i] == '\n') {
                if (!i || cmd[i - 1] != '\\') {
                    if (!i) {
                        cmd[i] = '\0';
                    }
                    end = 1;
                } else {
                    print("\n%s", shell->env->list[2]->value);
                    cur_prompt = 2;
                    i -= 2;
                    multi_index = (size_t)i;
                    cmd_len = (size_t)i;
                    mutli = 1;
                }
            }

            if (iscntrl(cmd[i]) && cmd[i] != '\n' && !end) {
                if (cmd[i] == 127 && i > 0) {
                    if (i > 1)
                        cmd[i - 1] = '\0';

                    cmd[i] = '\0';
                    i -= 2;
                    if ((size_t)i == cmd_len)
                        sep = 0;
                } else {
                    cmd[i] = '\0';
                    --i;
                }
            }

            if (i >= 0 && cmd[i] == ' ' && !sep)
                sep = 1;

            ++i;

            if ((size_t)i == size) {
                size *= 2;
                cmd = realloc(cmd, sizeof(char) * size);
                if (cmd == NULL)
                    error("read_cmd realloc");
            }

            cmd[i] = '\0';
            if (!sep)
                cmd_len = (size_t)i;

            if (!end)
                parse_input(shell, cur_prompt, cmd, multi_index, cmd_len, mutli);
        }

        if (ret == -1)
            end = 1;
    }

    switch (ret) {
    case 0:
    case 1:
        cmd[i] = '\0';
        cmd = realloc(cmd, sizeof(char) * ((size_t)i + 1));
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

    if (cmd[0] == '\0') {
        free(cmd);
        cmd = NULL;
    }

    print("\n");
    *res = (int)ret;
    return cmd;
}

int check_cmd(char *cmd) {

    int num_quotes = 0;

    while (*cmd != '\0') {
        if (*cmd == '"')
            ++num_quotes;
        ++cmd;
    }

    return num_quotes % 2 == 0;
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

    if (ast->token->type == TOKEN_AMPER || ast->token->type == TOKEN_SEMI) {

        pid_t child = fork();
        int i = num_cmd, status;

        if (child == 0) {
            num_cmd = 0;
            check_ast(shell, ast->children[0], &num_cmd);

            if (check_builtin(ast->children[0]->token->command[0]))
                exit(0);
            exit(init_executor(ast->children[0], shell, num_cmd));
        } else if (child == -1) {
            error("init_executor");
        } else {
            if (ast->token->type == TOKEN_SEMI) {

                if (check_builtin(ast->children[0]->token->command[0])) {
                    status = init_executor(ast->children[0], shell, num_cmd);
                } else {
                    while (i > 0) {
                        wait(&status);
                        --i;
                    }
                }
                if (ast->num_children != 2)
                    return status;
            }

            if (ast->token->type == TOKEN_AMPER) {
                if (check_builtin(ast->children[0]->token->command[0]))
                    status = init_executor(ast->children[0], shell, num_cmd);
            }

            if (ast->num_children == 2) {
                num_cmd = 0;
                ast = ast->children[1];
                check_ast(shell, ast, &num_cmd);
                return init_executor(ast, shell, num_cmd);
            }
        }
    }

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
    Tokens *tokens;
    Cmd *rpn_tokens;
    AST *ast;

    while (shell->is_running) {
        print_prompt(shell, 1);
        if ((command = read_cmd(shell, &res)) != NULL && res) {


            if (check_cmd(command)) {

                save_history(shell, command);

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
                        print_cmd(rpn_tokens);
                        if (check_rpn(rpn_tokens)) {

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

                        free_cmd(rpn_tokens);
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

                        free_cmd(rpn_tokens);
                        free_tokens(tokens);
                    }

                    free_input(in);
                    free(command);
                }
            } else {
                print_err("Syntax error: incorrect number of quotes\n");
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
