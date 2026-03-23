#include <stddef.h>
#define __STDC_WANT_LIB_EXT1__ 1
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h> 
#include <unistd.h>
#include "./parser.h"
#include "./lexer.h"
#include <ncurses.h>
#include "./list.h"
#define ex(msg) do{perror(msg);\
    exit(1);\
}while(false);

typedef struct {
    enum {
        VOID,
        VALID,
        INVALID,
    } tag;
    union {
        double value;
        char* error;
    } data;
} Expression;

typedef struct {
    char* content;
    char* gap_start;
    char* gap_end;
    size_t length;
    Expression expr;
} Line;

typedef struct {
    int x;
    int y;
    Line** lines;
    WINDOW* subwindow;
    int nlines;
} State;

Lexer lx;
int is_operation(char ch){
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}
Line* mkline(char* str, Expression expr){
    Line* line = malloc(sizeof(Line));
    line->expr = expr;
    char *buff = malloc(sizeof(strlen(str) + 10));
    memcpy(buff, str, strlen(str));
    line->content = buff;
    line->gap_start = line->content + strlen(str) + 1;
    line->gap_end = line->content + strlen(str) + 10;
    line->length = strlen(str);
    return line;
}
void append_char(Line* line, char c){
    if(line->gap_start > line->gap_end){
        *line->gap_start = c;
        line->gap_start += 1;
        return;
    }
    *line->gap_start = c;
    line->length += 10;
    line->content = realloc(line->content, line->length);
    if(line->content == NULL) ex("Error: impossible realloc string");
    if(line->gap_end < line->content + line->length){
        memmove(line->gap_end + 1 + 10, line->gap_end + 1, line->length - (int)(line->gap_end - line->content));
    }
    line->gap_start += 1;
    line->gap_end += 10;
}

int main(){
    int c;
    noecho();
    initscr();
    cbreak();
    refresh();
    WINDOW* win;
    Line** lines = malloc(sizeof(Line*));
    State state= {
        .x = 0,
        .y = 0,
        .lines = lines,
        .nlines = 1,
        .subwindow = win
    };

    Line* line = mkline("", (Expression){});
    lines[0] = line;

    while(1){
        c = getch();
        if(c == KEY_ENTER) {
            delwin(win);
            move(state.y + 1, 0);
            state.y += 1; state.x = 0;
            state.lines = realloc(state.lines, ++state.nlines);
            state.lines[state.nlines - 1] = mkline("", (Expression) {.tag = VOID });
            wmove(win, state.y + 1, 0);
            werase(win);
            refresh();
        }
        if(is_operation(c) || isdigit(c) || c == '(' || c == ')' || c == ' '){
            append_char(line, c);
        }
    }
}
