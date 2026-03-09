#include <stdlib.h>
#include "./ast.h"

// to heap allocate a ast
AST* new_ast(AST ast){
    AST *ptr = malloc(sizeof(AST));
    if(ptr != NULL) *ptr = ast;
    return ptr;
}

#define NEW_AST(t, ...) \
new_ast((AST) { .tag = t, .data = { __VA_ARGS__ }})

double eval(AST* ast){
    if(ast->tag == AST_NUMBER) return ast->data.number.number;
    double left = eval(ast->data.bin_op.left);
    double right = eval(ast->data.bin_op.right);
    switch(ast->tag){
        case AST_ADD: return left + right;
        case AST_SUB: return left - right;
        case AST_PRODUCT: return left * right;
        case AST_DIVISION: return left / right;
        default: return 0;
    }
}
