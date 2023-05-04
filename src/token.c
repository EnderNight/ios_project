#include "token.h"
#include "input.h"
#include "utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Token *create_token(Input *in, int from, int to, Type type) {

    Token *tok = malloc(sizeof(Token));
    size_t len = (size_t)(to - from + 1);

    tok->type = type;

    tok->command = malloc(sizeof(char *) * len);

    for (size_t i = 0; i < len; ++i) {
        tok->command[i] =
            malloc(sizeof(char) * (strlen(in->sep_cmd[(size_t)from + i]) + 1));
        strcpy(tok->command[i], in->sep_cmd[(size_t)from + i]);
    }

    tok->len = len;

    return tok;
}

void free_token(Token *tok) {

    for (size_t i = 0; i < tok->len; ++i) {
        free(tok->command[i]);
    }

    free(tok->command);
    free(tok);
}

void free_tokens(Tokens *tokens) {

    for (int i = 0; i < tokens->num; ++i) {
        free_token(tokens->token_list[i]);
    }

    free(tokens);
}

void add_cmd_token(int *i, int *cmd_index, int *tok_index, Tokens *tokens,
                   Type type, Input *in) {
    if (*i != *cmd_index && *i > 0) {
        tokens->token_list[*tok_index] =
            create_token(in, *cmd_index, *i - 1, TOKEN_CMD);
        ++*tok_index;
    }
    tokens->token_list[*tok_index] = create_token(in, *i, *i, type);
    *cmd_index = *i + 1;
    ++*tok_index;
}

Tokens *tokenize(Input *in) {

    Tokens *tokens = malloc(sizeof(Tokens));
    int i = 0, is_redir = 0, cmd_index = 0, tok_index = 0;

    tokens->token_list = malloc(sizeof(Token *) * (size_t)in->num_string);

    while (i < in->num_string) {

        if (strcmp(in->sep_cmd[i], "|") == 0) {
            add_cmd_token(&i, &cmd_index, &tok_index, tokens, TOKEN_PIPE, in);
        } else if (is_redir) {
            tokens->token_list[tok_index] = create_token(in, i, i, TOKEN_FILE);
            is_redir = 0;
            ++tok_index;
            cmd_index = i + 1;
        } else if (strcmp(in->sep_cmd[i], ">") == 0) {
            add_cmd_token(&i, &cmd_index, &tok_index, tokens, TOKEN_REDIR, in);
            is_redir = 1;
        } else if (strcmp(in->sep_cmd[i], "&") == 0) {
            add_cmd_token(&i, &cmd_index, &tok_index, tokens, TOKEN_AMPER, in);
        } else if (strcmp(in->sep_cmd[i], ";") == 0) {
            add_cmd_token(&i, &cmd_index, &tok_index, tokens, TOKEN_SEMI, in);
        }

        ++i;
    }

    if (i != cmd_index && i > 0) {
        tokens->token_list[tok_index] =
            create_token(in, cmd_index, i - 1, TOKEN_CMD);
        ++tok_index;
    }

    tokens->num = tok_index;

    return tokens;
}

void print_token(Token *token) {

    print("Token: '");
    for (size_t i = 0; i < token->len - 1; ++i) {
        print("%s ", token->command[i]);
    }
    print("%s', type: %d\n", token->command[token->len - 1], token->type);
}

void print_tokens(Tokens *tokens) {

    for (int j = 0; j < tokens->num; ++j) {
        print("Token %d: '", j);
        for (size_t i = 0; i < tokens->token_list[j]->len - 1; ++i) {
            print("%s ", tokens->token_list[j]->command[i]);
        }
        print("%s', type: %d\n",
              tokens->token_list[j]->command[tokens->token_list[j]->len - 1],
              tokens->token_list[j]->type);
    }
}
