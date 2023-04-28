#ifndef INPUT_H
#define INPUT_H

#define EOF -1
#define INIT_POS -2

typedef struct input {

    char *buf;
    long size;
    long pos;

} input;

char next(input *in);
char look(input *in);
void skip(input *in);


#endif // INPUT_H
