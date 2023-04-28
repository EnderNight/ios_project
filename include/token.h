#ifndef TOKEN_H
#define TOKEN_H

#include "input.h"

typedef struct Token {

    long len;
    char *text;

} Token;

Token **tokenize(input *in);
void free_token(Token *tok);

#endif // TOKEN_H
