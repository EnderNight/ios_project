#include "input.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Input *init_input(char *command) {

    Input *in = malloc(sizeof(Input));
    size_t len = strlen(command);

    in->single_cmd = malloc(sizeof(char) * (len + 1));
    strcpy(in->single_cmd, command);

    in->size = len;

    return in;
}

void free_input(Input *in) {

    free(in->single_cmd);

    // for (int i = 0; i < in->num_string; ++i) {
    //     free(in->sep_cmd[i]);
    // }
    free(in->sep_cmd);

    free(in);
}

void separate(Input *in) {

    size_t size = 1, i = 0;
    char *cmdp = in->single_cmd;

    in->sep_cmd = malloc(sizeof(char *));

    while ((in->sep_cmd[i] = strtok(cmdp, " \t\n")) != NULL) {
        ++i;
        cmdp = NULL;

        if (i == size) {
            size *= 2;
            in->sep_cmd = realloc(in->sep_cmd, sizeof(char *) * size);
        }
    }

    in->sep_cmd = realloc(in->sep_cmd, sizeof(char *) * (i + 1));
    in->num_string = (int)i;
}
