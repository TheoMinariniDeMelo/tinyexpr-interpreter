#include "../lexer.h"

int main(int, char **)
{
    Lexer lexer;
    lexer_init(&lexer, "8 * 5 * 2 * ( 5 * 3 )*2*2/3");
    Token token;
    while (token.type != TOK_EOF && token.type != TOK_ERROR)
    {
        token = next_token(&lexer);
        printf("%s\n", token_name(token.type));
    }
}