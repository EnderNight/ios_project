#include "ast.h"
#include "token.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

void free_ast(AST *ast) {

    if (ast->num_children > 0) {
        for (int i = 0; i < ast->num_children; ++i) {
            free_ast(ast->children[i]);
        }
        free(ast->children);
    }

    free_token(ast->token);
    free(ast);
}

AST *create_ast(void) {

    AST *ast = malloc(sizeof(AST));

    ast->token = NULL;
    ast->children = NULL;
    ast->num_children = 0;

    return ast;
}

void add_children(AST *ast, AST *child) {

    if (ast->num_children == 0)
        ast->children = malloc(sizeof(AST *));
    else
        ast->children = realloc(
            ast->children, sizeof(AST *) * ((size_t)ast->num_children + 1));

    ++ast->num_children;
    ast->children[ast->num_children - 1] = child;
}

void print_ast(AST *ast) {

    print("Tree %d, num_children = %d\n", ast->token->type, ast->num_children);
    print_token(ast->token);
    if (ast->num_children != 0) {
        for (int i = 0; i < ast->num_children; ++i) {
            print_ast(ast->children[i]);
        }
    }
    print("End of tree %d\n", ast->token->type);
}
