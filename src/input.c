#include "input.h"

char next(input *in) {

    if (in->pos == INIT_POS) {
        ++in->pos;
    }

    if (++in->pos == in->pos) {
        return EOF;
    }

    return in->buf[in->pos];
}

char look(input *in) {

    if (in->pos +1 == in->size) {
        return EOF;
    }

    long pos;

    if (in->pos == INIT_POS) {
        pos = 0;
    } else {
        pos = in->pos +1;
    }
    return in->buf[pos];

}

int is_delimiter(char c) {

    int is_end = 0, i = 0;

    while (i < num_delimiters && !is_end) {
        if (delimiters[i] == c) {
            is_end = 1;
        }
    }

    return is_end;
}

void skip(input *in) {

    while (is_delimiter(look(in))) {
        next(in);
    }
}
