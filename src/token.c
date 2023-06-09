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

    tok->command = malloc(sizeof(char *) * (len + 1));

    tok->command[len] = NULL;

    for (size_t i = 0; i < len; ++i) {
        tok->command[i] =
            malloc(sizeof(char) * (strlen(in->sep_cmd[(size_t)from + i]) + 1));
        strcpy(tok->command[i], in->sep_cmd[(size_t)from + i]);
    }

    tok->len = len;
    tok->assoc = LEFT;

    if (tok->type == TOKEN_CMD) {
        tok->valence = 0;
    } else if (tok->type == TOKEN_AMPER || tok->type == TOKEN_SEMI) {
        tok->valence = 1;
        tok->assoc = RIGHT;
    } else {
        tok->valence = 2;
    }

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

    free(tokens->token_list);
    free(tokens);
}

void free_cmd(Cmd *cmd) {

    for (int i = 0; i < cmd->len; ++i) {
        free_tokens(cmd->tokens[i]);
    }

    free(cmd->tokens);
    free(cmd);
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

Token *copy_token(Token *tok) {

    Token *res = malloc(sizeof(Token));

    res->type = tok->type;
    res->len = tok->len;

    res->command = malloc(sizeof(char *) * (res->len + 1));
    res->command[res->len] = NULL;
    for (size_t i = 0; i < res->len; ++i) {
        res->command[i] = malloc(sizeof(char) * (strlen(tok->command[i]) + 1));
        strcpy(res->command[i], tok->command[i]);
    }

    res->valence = tok->valence;

    return res;
}

Tokens *tokenize(Input *in) {

    Tokens *tokens = malloc(sizeof(Tokens));
    int i = 0, cmd_index = 0, tok_index = 0;

    tokens->token_list = malloc(sizeof(Token *) * (size_t)in->num_string);

    while (i < in->num_string) {

        if (strcmp(in->sep_cmd[i], "|") == 0) {
            add_cmd_token(&i, &cmd_index, &tok_index, tokens, TOKEN_PIPE, in);
        } else if (strcmp(in->sep_cmd[i], ">") == 0) {
            add_cmd_token(&i, &cmd_index, &tok_index, tokens, TOKEN_REDIR, in);
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
    print("%s', type: %d, valence: %d\n", token->command[token->len - 1],
          token->type, token->valence);
}

void print_tokens(Tokens *tokens) {

    for (int j = 0; j < tokens->num; ++j) {
        print("Token %d: '", j);
        for (size_t i = 0; i < tokens->token_list[j]->len - 1; ++i) {
            print("%s ", tokens->token_list[j]->command[i]);
        }
        print("%s', type: %d, valence: %d\n",
              tokens->token_list[j]->command[tokens->token_list[j]->len - 1],
              tokens->token_list[j]->type, tokens->token_list[j]->valence);
    }
}

void print_cmd(Cmd *cmd) {
    for (int i = 0; i < cmd->len; ++i)
        print_tokens(cmd->tokens[i]);
}
