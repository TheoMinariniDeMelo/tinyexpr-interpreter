#include <stdlib.h>
#include <setjmp.h>
#include "./lexer.h"
#include "./parser.h"

#define THROW(message, t) do {\
    error.col = current.col;\
    error.line = current.line;\
    error.msg = strdup(message);\
    error.tag = t;\
    longjmp(exception_env, 1);}\
    while(false);

Token current;

extern jmp_buf exception_env;
extern Lexer lx;
extern ParseError error;

void advance(){
    current = next_token(&lx);
}

void expect(Token t){
    if(current.type != t.type){
        THROW("Unexpected token", PARSER_ERROR);
    }
    advance();
}

AST* parse_expr(){
    AST* left = parse_term();

    while(current.type == TOK_PLUS || current.type == TOK_MINUS){
        AST* right = parse_term();
        if(right == NULL) THROW("Expected Token", PARSER_ERROR);

        if(current.type == TOK_PLUS) left = NEW_AST(AST_ADD, .bin_op = { .left = left, .right = right });
        else left = NEW_AST(AST_SUB, .bin_op = { .left = left, .right = right });

    }
    return left;
}

AST* parse_term(){
    AST* left = parse_factor();

    while(current.type == TOK_STAR || current.type == TOK_SLASH){
        AST* right = parse_term();
        if(right == NULL) THROW("Expected Token", PARSER_ERROR);

        if(current.type == TOK_STAR) left = NEW_AST(AST_PRODUCT, .bin_op = { .left = left, .right = right });
        else left = NEW_AST(AST_DIVISION, .bin_op = { .left = left, .right = right });
    }
    return left;
}

AST* parse_factor(){
    advance();
    if(current.type == TOK_EOF) return NULL;

    if(current.type == TOK_NUMBER){
        double value = atof(current.lexeme);
        advance();
        return NEW_AST(AST_NUMBER, .number = { .number = value });
    }
    if(current.type == TOK_LPAREN){
        AST* ast = parse_expr();
        if(current.type != TOK_RPAREN){
            THROW("Unexpected Token", PARSER_ERROR);
        }
        return ast;
    }
    if(current.type == TOK_ERROR){
        THROW("Invalid Token", LEXICAL_ERROR);
    }
    THROW("Unexpected Token", PARSER_ERROR);
    return NULL;
}
