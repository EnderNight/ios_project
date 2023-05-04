#ifndef STACK_H
#define STACK_H

#include "token.h"

typedef struct Stack_ele {

    Token *token;
    struct Stack_ele *prev;

} Stack_ele;

typedef struct Stack {

    Stack_ele *cur;
    int len;

} Stack;

void free_stack(Stack *stack);
Stack *create_stack(void);
void push(Stack *stack, Token *token);
Stack_ele *pull(Stack *stack);

#endif // STACK_H
