#ifndef SHELL_H
#define SHELL_H

#define error(a)                                                               \
    {                                                                          \
        perror(a);                                                             \
        exit(1);                                                               \
    };
#define MAXLINE 200
#define MAXARGS 20
#define MAXCMDLEN 100

typedef struct Variable {
    char *name;
    char *value;
} Variable;

typedef struct ENV {
    Variable **list;
    int size;
    int num;
} ENV;

typedef struct Shell {
    ENV *env;
    int is_running;
} Shell;

Shell *sh_init(void);
int sh_loop(Shell *shell, int debug);
void sh_end(Shell *shell);

#endif // SHELL_H
