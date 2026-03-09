#include <stdio.h>
#include <stdlib.h>


typedef struct AST AST; // Forward reference

struct AST {
    enum {
        AST_ADD,
        AST_NUMBER,
        AST_PRODUCT,
        AST_DIVISION,
        AST_SUB,
    } tag;
    union {
        struct { double number; } number;
        struct { struct AST *left; struct AST *right; } bin_op;
    } data;
};
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

int main(){
    AST* ast = NEW_AST(AST_ADD, .bin_op = { 
                       .left = NEW_AST(AST_PRODUCT, .bin_op = {
                                       .left = NEW_AST(AST_NUMBER, .number = {40}),
                                       .right = NEW_AST(AST_NUMBER, .number = {3})
                                       }), 
                       .right = NEW_AST(AST_NUMBER, .number = {10}) 
                       });

    printf("valor total: %f", eval(ast));
    return 1;
}
