#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h> 
#include <unistd.h>
#include <ncurses.h>
#include "./parser.h"
#include "./lexer.h"
#include "./editor.h"

Lexer lx;
jmp_buf exception_env;
ParseError error;
extern Editor editor;

void 
process_line(){
    Line* line = editor.current_line;
    if(line->length == 0){
        line->expr = (Expression){
            .tag = VOID,
        };
        return;
    }
    lexer_init(&lx, strdup_line_content(line));

    if(setjmp(exception_env) == 0){
        AST* ast = parse_expr();
        if(ast == NULL) return;
        double value = eval(ast);
        line->expr = (Expression){
            .tag= VALID,
            .data = {
                .value = value
            }
        };
    }else {
        char buff[50];
        int value = sprintf(buff, "Error: %s, line: %d col: %d", error.msg, error.line, error.col);
        buff[value] = '\0';
        line->expr = (Expression){
            .tag= INVALID,
            .data = {
                .error = strdup(buff)
            }
        };
    }
}


int
main(){
    init_editor();
    render();
    while(1){
        process_key();
        render();
        process_line();
        render_result();
    }
}
