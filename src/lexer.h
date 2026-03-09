#include "./string.h"
#include <stdio.h>
#include <ctype.h>

typedef enum
{
    TOK_NUMBER,
    TOK_STAR,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_SLASH,
    TOK_ERROR,
    TOK_EOF
} TokenType;

typedef struct
{
    char *src;
    size_t pos;
    int line, col;
} Lexer;

typedef struct
{
    char *lexeme;
    int line, col;
    TokenType type;
} Token;

void lexer_init(Lexer *lx, const char *src);
void free_token(Token *token);
static Token lex_number(Lexer *lx);
Token next_token(Lexer *lx);
const char *token_name(TokenType type);
