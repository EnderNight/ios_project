#include "parser.h"
#include "stack.h"
#include "token.h"
#include "ast.h"

#include <stdlib.h>

Tokens *to_rpn(Tokens *infix_tok) {

    Tokens *rpn_tok = malloc(sizeof(Tokens));
    Stack_ele *temp;
    Stack *stack = create_stack();
    int infix_index = 0, rpn_index = 0;

    rpn_tok->num = infix_tok->num;
    rpn_tok->token_list = malloc(sizeof(Token *) * (size_t)infix_tok->num);

    while (infix_index < infix_tok->num) {

        if (infix_tok->token_list[infix_index]->type == TOKEN_CMD) {
            rpn_tok->token_list[rpn_index] = infix_tok->token_list[infix_index];
            ++rpn_index;
        } else {

            while (stack->cur != NULL &&
                   stack->cur->token->type >
                       infix_tok->token_list[infix_index]->type &&
                   (temp = pull(stack)) != NULL) {
                rpn_tok->token_list[rpn_index] = temp->token;
                free(temp);
                ++rpn_index;
            }
            push_tok(stack, infix_tok->token_list[infix_index]);
        }

        ++infix_index;
    }

    while ((temp = pull(stack)) != NULL) {
        rpn_tok->token_list[rpn_index] = temp->token;
        free(temp);
        ++rpn_index;
    }

    // free_tokens(infix_tok);
    free_stack(stack);

    return rpn_tok;
}


AST *rpn_to_ast(Tokens *rpn_tokens) {

    AST *ast = NULL;
    Stack *stack = create_stack();
    Stack_ele *temp;
    int i = 0;

    while (i < rpn_tokens->num) {

        if (rpn_tokens->token_list[i]->type == TOKEN_CMD) {
            ast = create_ast();
            ast->token = rpn_tokens->token_list[i];
            push_ast(stack, ast);
            ast = NULL;
        } else {
            ast = create_ast();
            ast->token = rpn_tokens->token_list[i];

            temp = pull(stack);
            add_children(ast, temp->ast);
            free(temp);
            
            temp = pull(stack);
            add_children(ast, temp->ast);
            free(temp);

            push_ast(stack, ast);

            ast = NULL;
        }

        ++i;
    }

    temp = pull(stack);
    ast = temp->ast;
    free(temp);

    return ast;
}
