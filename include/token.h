#ifndef TOKEN_H
#define TOKEN_H

typedef struct Token {

    enum type {
        TOKEN_CMD = 1,
        TOKEN_ARG,
        TOKEN_PIPE,
        TOKEN_REDIR,

    } type;

    char *text;
    int size;

} Token;

typedef struct Tokens {

    Token **token_list;
    int size;

} Tokens;

#endif // TOKEN_H
