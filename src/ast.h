#include <stdio.h>
#include <stdlib.h>

#define NEW_AST(t, ...) \
new_ast((AST) { .tag = t, .data = { __VA_ARGS__ }})

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
AST* new_ast(AST ast);

double eval(AST* ast);
