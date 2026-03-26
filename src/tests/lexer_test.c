#include "../lexer.h"

int main(int arc, char ** argsv)
{
    Lexer lexer;
    lexer_init(&lexer, "8 * 5 * 2 * ( 5 * 3 )*2*2/3");
    Token *tokens = malloc(17*sizeof(Token));
    Token token;
    int i = 0;
    while (token.type != TOK_EOF && token.type != TOK_ERROR)
    {
        token = next_token(&lexer);
        printf("%s\n", token_name(token.type));
        tokens[i++] = token;
    }
    void* (*m)(int) = malloc; 
    char * arr = m(10);
}
