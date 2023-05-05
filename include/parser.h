#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "token.h"

Tokens *to_rpn(Tokens *infix_tok);
AST *rpn_to_ast(Tokens *rpn_tok);

#endif // PARSER_H
