#ifndef TOKEN_H
#define TOKEN_H

#include "input.h"

#include <stddef.h>

typedef enum type {
    TOKEN_SEMI = 1,
    TOKEN_AMPER,
    TOKEN_PIPE,
    TOKEN_REDIR,
    TOKEN_CMD,
} Type;

typedef struct Token {
    Type type;
    char **command;
    size_t len;
    int valence;
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
Token *copy_token(Token *tok);

#endif // TOKEN_H
