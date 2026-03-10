#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./lexer.h"
#include "./ast.h"

Token current;

extern Lexer lx;

void error(char *msg){
    printf("\x1b[1;31m");
    printf("%s", msg);
    printf("\x1b[0m");
    exit(1);
}
AST* parse_expr();
AST* parse_term();
AST* parse_factor();

void advance(){
    current = next_token(&lx);
}

void expect(TokenType t){
    if(current.type != t){
        error("Unexpected token");
    }
    advance();
}

AST* parse_expr(){
    AST* left = parse_term();

    while(current.type == TOK_PLUS || current.type == TOK_MINUS){
        if(current.type == TOK_PLUS) left = NEW_AST(AST_ADD, .bin_op = { .left = left, .right = parse_term() });
        else left = NEW_AST(AST_SUB, .bin_op = { .left = left, .right = parse_term() });

    }
    return left;
}

AST* parse_term(){
    AST* left = parse_factor();
    while(current.type == TOK_STAR || current.type == TOK_SLASH){
        if(current.type == TOK_STAR) left = NEW_AST(AST_PRODUCT, .bin_op = { .left = left, .right = parse_factor() });
        else left = NEW_AST(AST_DIVISION, .bin_op = { .left = left, .right = parse_factor() });
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
            error("Expected token");
        }
        return ast;
    }
    error("Expected token");
    return NULL;
}
