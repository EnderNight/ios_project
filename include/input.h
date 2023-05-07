#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

typedef struct Input {
    char *single_cmd;
    char **sep_cmd;
    size_t size;
    int num_string;
} Input;

Input *init_input(char *command);
int separate(Input *in);
void free_input(Input *in);
void print_input(Input *in);

#endif // INPUT_H
