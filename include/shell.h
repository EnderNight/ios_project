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

int sh_execute(int argc, char *argv[]);
int execute(int argc, char *argv[]);
int read_args(int *argcp, char *args[], int max, int *eofp);

#endif // SHELL_H
