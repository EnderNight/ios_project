#ifndef AST_H
#define AST_H

#include "token.h"

typedef struct AST {

    Token *token;
    struct AST **children;
    int num_children;

} AST;

void free_ast(AST *ast);
void add_children(AST *ast, AST *child);
AST *create_ast(void);
void print_ast(AST *ast);

#endif // AST_H
