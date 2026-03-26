#include "./ast.h"

typedef struct {
    int col; 
    int line;
    char* msg;
    enum {
        LEXICAL_ERROR,
        PARSER_ERROR,
    } tag;
} ParseError;

AST* parse_expr();
AST* parse_term();
AST* parse_factor();
