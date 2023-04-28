#ifndef INPUT_H
#define INPUT_H

#define EOF -1
#define INIT_POS -2

char delimiters[] = { ' ', '&', '|', ';' };
int num_delimiters = sizeof(delimiters) / sizeof(char);

typedef struct input {

    char *buf;
    long size;
    long pos;

} input;

char next(input *in);
char look(input *in);
int is_delimiter(char c);
void skip(input *in);


#endif // INPUT_H
