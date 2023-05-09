#include "parser.h"
#include "ast.h"
#include "stack.h"
#include "token.h"

#include <stdlib.h>

Tokens *_to_rpn(Tokens *infix_tok) {

    Tokens *rpn_tok = malloc(sizeof(Tokens));
    Stack_ele *temp;
    Stack *stack = create_stack();
    int infix_index = 0, rpn_index = 0;

    rpn_tok->num = infix_tok->num;
    rpn_tok->token_list = malloc(sizeof(Token *) * (size_t)infix_tok->num);

    while (infix_index < infix_tok->num) {

        if (infix_tok->token_list[infix_index]->type == TOKEN_CMD) {
            rpn_tok->token_list[rpn_index] =
                copy_token(infix_tok->token_list[infix_index]);
            ++rpn_index;
        } else {

            while (stack->cur != NULL &&
                   stack->cur->token->type >
                       infix_tok->token_list[infix_index]->type &&
                   (temp = pull(stack)) != NULL) {
                rpn_tok->token_list[rpn_index] = copy_token(temp->token);
                free(temp);
                ++rpn_index;
            }
            push_tok(stack, infix_tok->token_list[infix_index]);
        }

        ++infix_index;
    }

    while ((temp = pull(stack)) != NULL) {
        rpn_tok->token_list[rpn_index] = copy_token(temp->token);
        free(temp);
        ++rpn_index;
    }

    // free_tokens(infix_tok);
    free_stack(stack);

    return rpn_tok;
}

Cmd *to_rpn(Tokens *infix_tok) {

    Cmd *res = malloc(sizeof(Cmd));
    int len = 1, is_end = 0;
    Tokens *tmp = malloc(sizeof(Tokens));

    res->tokens = malloc(sizeof(Tokens *));
    tmp->token_list = malloc(sizeof(Token *) * (size_t)infix_tok->num);
    tmp->num = 0;

    for (int i = 0; i < infix_tok->num; ++i) {

        if (is_end)
            is_end = 0;

        if (infix_tok->token_list[i]->type == TOKEN_AMPER ||
            infix_tok->token_list[i]->type == TOKEN_SEMI) {
            tmp->token_list[tmp->num] = copy_token(infix_tok->token_list[i]);
            ++tmp->num;

            res->tokens[len - 1] = _to_rpn(tmp);
            ++len;
            res->tokens = realloc(res->tokens, sizeof(Tokens *) * (size_t)len);

            free_tokens(tmp);
            tmp = malloc(sizeof(Tokens));
            tmp->token_list = malloc(sizeof(Token *) * (size_t)infix_tok->num);
            tmp->num = 0;

            is_end = 1;
        } else {
            tmp->token_list[tmp->num] = copy_token(infix_tok->token_list[i]);
            ++tmp->num;
        }
    }

    if (!is_end)
        res->tokens[len - 1] = _to_rpn(tmp);
    else
        --len;

    res->len = len;
    free_tokens(tmp);

    return res;
}

int _check_rpn(Tokens *rpn_tokens) {

    int size = 0, err = 0, i = 0;

    while (i < rpn_tokens->num && !err) {
        size += 1 - rpn_tokens->token_list[i]->valence;
        if (size <= 0)
            err = 1;
        ++i;
    }

    if (err || size != 1)
        return 0;

    return 1;
}

int check_rpn(Cmd *rpn_tokens) {

    int i = 0;

    while (i < rpn_tokens->len && _check_rpn(rpn_tokens->tokens[i])) {
        ++i;
    }

    return i == rpn_tokens->len;
}

AST *_rpn_to_ast(Tokens *rpn_tokens) {

    AST *ast = NULL;
    Stack *stack = create_stack();
    Stack_ele *temp;
    int i = 0;

    while (i < rpn_tokens->num) {

        if (rpn_tokens->token_list[i]->type == TOKEN_CMD) {
            ast = create_ast();
            ast->token = copy_token(rpn_tokens->token_list[i]);
            push_ast(stack, ast);
            ast = NULL;
        } else {
            ast = create_ast();
            ast->token = copy_token(rpn_tokens->token_list[i]);

            temp = pull(stack);
            add_children(ast, temp->ast);
            free(temp);

            if (rpn_tokens->token_list[i]->type != TOKEN_AMPER &&
                rpn_tokens->token_list[i]->type != TOKEN_SEMI) {
                temp = pull(stack);
                add_children(ast, temp->ast);
                free(temp);
            }

            push_ast(stack, ast);

            ast = NULL;
        }

        ++i;
    }

    temp = pull(stack);
    ast = temp->ast;
    free(temp);

    free_stack(stack);
    return ast;
}

AST *rpn_to_ast(Cmd *rpn_tokens) {

    int i = rpn_tokens->len - 2;
    AST *res = _rpn_to_ast(rpn_tokens->tokens[rpn_tokens->len - 1]), *tmp;

    while (i >= 0) {
        tmp = _rpn_to_ast(rpn_tokens->tokens[i]);
        add_children(tmp, res);
        res = tmp;
        --i;
    }

    return res;
}
