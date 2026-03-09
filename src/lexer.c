#include "./string.h"
#include <stdio.h>
#include <ctype.h>
#include "./lexer.h"

void lexer_init(Lexer *lx, const char *src)
{
    lx->col = 1;
    lx->line = 1;
    lx->pos = 0;
    lx->src = strdup(src);
}
static char peek(Lexer *lx)
{
    return lx->src[lx->pos];
}
static char peek_next(Lexer *lx)
{
    char c = peek(lx);
    return c ? lx->src[lx->pos + 1] : '\0';
}
static char advance(Lexer *lx)
{
    char c = peek(lx);
    if (c == '\0')
        return '\0';
    lx->pos += 1;
    if (c == '\n')
    {
        lx->col = 1;
        lx->line += 1;
    }
    else
{
        lx->col += 1;
    }
    return c;
}
static int match(Lexer *lx, char expected)
{
    if (peek(lx) != expected)
    {
        return 0;
    }
    advance(lx);
    return 1;
}
static Token make_token(Lexer *lx, TokenType type, size_t start, size_t end, int line, int col)
{
    Token token = {
        .line = line,
        .col = col,
        .type = type,
    };
    token.lexeme = strsub_dup(lx->src, start, end);
    if (!token.lexeme)
    {
        token.type = TOK_ERROR;
        token.lexeme = "@OUT_OF_MEMORY";
    }
    return token;
}
static Token token_error(int line, int col, char *msg)
{
    return (Token){
        .line = line,
        .col = col,
        .type = TOK_ERROR,
        .lexeme = strdup(msg)};
}
void free_token(Token *token)
{
    free(token->lexeme);
    token->lexeme = NULL;
}

static void skip_ws(Lexer *lx)
{
    char c = peek(lx);
    while (c == ' ' || c == '\t' || c == '\n')
    {
        advance(lx);
        c = peek(lx);
    };
}
static Token lex_number(Lexer *lx)
{
    size_t start = lx->pos;
    while (isdigit((unsigned char)peek(lx)))
        advance(lx);
    if (peek(lx) == '.' && isdigit(peek_next(lx)))
    {
        advance(lx);
        while (isdigit((unsigned char)peek(lx)))
            advance(lx);
    }
    return make_token(lx, TOK_NUMBER, start, lx->pos, lx->line, lx->col);
}
Token next_token(Lexer *lx)
{
    char c = peek(lx);
    if (c == '\0')
        return make_token(lx, TOK_EOF, lx->pos, lx->pos, lx->line, lx->col);

    if (c == ' ')
    {
        skip_ws(lx);
        c = peek(lx);
    }

    if (isdigit(c))
        return lex_number(lx);

    advance(lx);
    switch (c)
    {
        case '(':
            return make_token(lx, TOK_LPAREN, lx->pos, lx->pos, lx->line, lx->col);
            break;
        case ')':
            return make_token(lx, TOK_RPAREN, lx->pos, lx->pos, lx->line, lx->col);
            break;
        case '+':
            return make_token(lx, TOK_PLUS, lx->pos, lx->pos, lx->line, lx->col);
            break;

        case '-':
            return make_token(lx, TOK_MINUS, lx->pos, lx->pos, lx->line, lx->col);
            break;

        case '*':
            return make_token(lx, TOK_STAR, lx->pos, lx->pos, lx->line, lx->col);
            break;

        case '/':
            return make_token(lx, TOK_SLASH, lx->pos, lx->pos, lx->line, lx->col);
            break;
        case '\0':
            return make_token(lx, TOK_EOF, lx->pos, lx->pos, lx->line, lx->col);
            break;
    }
    return token_error(lx->line, lx->col, "unexpected character");
}
const char *token_name(TokenType type)
{
    switch (type)
    {
        case TOK_NUMBER:
            return "NUMBER";
            break;
        case TOK_STAR:
            return "STAR";
            break;
        case TOK_LPAREN:
            return "LPAREN";
            break;
        case TOK_RPAREN:
            return "RPAREN";
            break;
        case TOK_PLUS:
            return "PLUS";
            break;
        case TOK_MINUS:
            return "MINUS";
            break;
        case TOK_SLASH:
            return "SLASH";
            break;
        case TOK_ERROR:
            return "ERROR";
            break;
        case TOK_EOF:
            return "EOF";
            break;
        default:
            return "???";
    }
}
