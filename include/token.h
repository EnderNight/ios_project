#ifndef TOKEN_H
#define TOKEN_H

#include "input.h"

#include <stddef.h>

typedef enum type {
    TOKEN_FILE = 1,
    TOKEN_CMD,
    TOKEN_REDIR,
    TOKEN_PIPE,
    TOKEN_AMPER,
    TOKEN_SEMI,
} Type;

typedef struct Token {
    Type type;
    char **command;
    size_t len;
} Token;

typedef struct Tokens {

    Token **token_list;
    int num;

} Tokens;

Tokens *tokenize(Input *in);
void print_tokens(Tokens *tokens);
void print_token(Token *token);
void free_token(Token *token);
void free_tokens(Tokens *token);

#endif // TOKEN_H
