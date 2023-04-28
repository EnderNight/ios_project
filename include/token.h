#ifndef TOKEN_H
#define TOKEN_H

typedef enum token_type {

    TOKEN_CMD = 1,
    TOKEN_PIPELINE,
    TOKEN_ARG,
    TOKEN_REDIR,

} token_type;

typedef struct Token {

    token_type type;
    char *value;

} Token;


void free_token(Token *tok);


#endif // TOKEN_H
