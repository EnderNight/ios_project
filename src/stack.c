#include "stack.h"
#include <stdlib.h>

void free_stack(Stack *stack) {

    Stack_ele *elem;

    while ((elem = pull(stack)) != NULL) {
        free(elem);
    }

    free(stack);
}

Stack *create_stack(void) {

    Stack *stack = malloc(sizeof(Stack));

    stack->len = 0;
    stack->cur = NULL;

    return stack;
}

Stack_ele *pull(Stack *stack) {

    Stack_ele *elem = NULL;

    if (stack->cur != NULL) {
        elem = stack->cur;
        stack->cur = elem->prev;
    }

    return elem;
}

void push_tok(Stack *stack, Token *token) {

    Stack_ele *elem = malloc(sizeof(Stack_ele));

    elem->token = token;
    elem->prev = stack->cur;
    stack->cur = elem;
}

void push_ast(Stack *stack, AST *ast) {

    Stack_ele *elem = malloc(sizeof(Stack_ele));

    elem->ast = ast;
    elem->prev = stack->cur;
    stack->cur = elem;

}
