#ifndef STACK_H
#define STACK_H

#include "ast.h"
#include "token.h"

typedef struct Stack_ele {

    union {
        Token *token;
        AST *ast;
    };
    struct Stack_ele *prev;

} Stack_ele;

typedef struct Stack {

    Stack_ele *cur;
    int len;

} Stack;

void free_stack(Stack *stack);
Stack *create_stack(void);
void push_tok(Stack *stack, Token *token);
void push_ast(Stack *stack, AST *ast);
Stack_ele *pull(Stack *stack);

#endif // STACK_H
