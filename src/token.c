#include "token.h"

#include <stdlib.h>
#include <string.h>

Token *create_token(Input *in, int pos, Type type) {

    Token *tok = malloc(sizeof(Token));
    size_t len = strlen(in->sep_cmd[pos]);

    tok->type = type;

    tok->text = malloc(sizeof(char) * (len + 1));
    strcpy(tok->text, in->sep_cmd[pos]);

    tok->size = len;

    return tok;
}

void free_token(Token *tok) {

    free(tok->text);
    free(tok);
}

void free_tokens(Tokens *tokens) {

    for (int i = 0; i < tokens->num; ++i) {
        free_token(tokens->token_list[i]);
    }

    free(tokens);
}

Tokens *tokenize(Input *in) {

    Tokens *tokens = malloc(sizeof(Tokens));
    int i = 0, is_cmd = 1, is_redir = 0;

    tokens->token_list = malloc(sizeof(Token *) * (size_t)in->num_string);

    while (i < in->num_string) {

        if (is_cmd) {
            tokens->token_list[i] = create_token(in, i, TOKEN_CMD);
            is_cmd = 0;
        } else if (strcmp(in->sep_cmd[i], "|") == 0) {
            tokens->token_list[i] = create_token(in, i, TOKEN_PIPE);
            is_cmd = 1;
        } else if (is_redir) {
            tokens->token_list[i] = create_token(in, i, TOKEN_FILE);
            is_redir = 0;
        } else if (strcmp(in->sep_cmd[i], ">") == 0) {
            tokens->token_list[i] = create_token(in, i, TOKEN_REDIR);
            is_redir = 1;
        } else {
            tokens->token_list[i] = create_token(in, i, TOKEN_ARG);
        }

        ++i;
    }

    tokens->num = in->num_string;

    return tokens;
}
