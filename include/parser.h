#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "token.h"

Cmd *to_rpn(Tokens *infix_tok);
int check_rpn(Cmd *rpn_tokens);
AST *rpn_to_ast(Cmd *rpn_tok);

#endif // PARSER_H
