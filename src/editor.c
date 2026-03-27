#include "editor.h"
#include <assert.h>
#include <ctype.h>
#include <ncurses.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Editor editor;

void
on_resize(int sig) {
    if (sig == SIGWINCH) {
        getmaxyx(stdscr, editor.viewport.numrow, editor.viewport.numcol);
    }
}

int
is_operation(char ch){
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

void
init_editor(){
    int c;
    signal(SIGWINCH, on_resize);
    initscr();
    noecho();
    cbreak();
    start_color();
    refresh();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK); 

    WINDOW *win = NEW_WINDOW();
    Line** lines = malloc(sizeof(Line*));
    editor = (Editor) {
        .viewport = (Viewport) { .cursor_row = 0 },
        .lines = lines,
        .numlines = 1,
        .current_line = mkline("", (Expression){ .tag = VOID }),
        .subwin = win
    };
    lines[0] = editor.current_line;
}

void
render(){
    update_offset();
    for(int i = 0; i < editor.numlines; i++){
        move(i, 0);
        Line* line = get_line(i);
        printw(">>> ");
        printw("%s", line_to_str(line));
    }
}

void
process_key(){
    int c = getch();
    if(c == KEY_ENTER) {
        delwin(editor.subwin);
        Line* new_line = mkline("", (Expression) { .tag = VOID });
        editor.lines = realloc(editor.lines, sizeof(editor.lines) + sizeof(Line*));
        memmove(editor.lines + editor.viewport.cursor_row + 2, editor.lines + editor.viewport.cursor_row + 1, sizeof(Line*) * (editor.numlines - editor.viewport.cursor_row));

        if(editor.lines == NULL) ex("Error");

        editor.lines[editor.viewport.cursor_row] = new_line;
        editor.numlines += 1;
        editor.current_line = new_line;
        editor.viewport.cursor_row += 1;
        editor.viewport.cursor_col = 0;

        editor.subwin = NEW_WINDOW();
    }
    if(is_operation(c) || isdigit(c) || c == '(' || c == ')' || c == ' '){
        insert_char(editor.current_line, c);
        return;
    }
    if(c == KEY_BACKSPACE){
        if(editor.viewport.cursor_col == 0 && editor.viewport.cursor_row >= 1){
            delwin(editor.subwin);
            Line* previous_line = get_line(editor.viewport.cursor_col - 1);
            int previous_line_length = previous_line->length;
            concat_lines(previous_line, editor.current_line);

            free(editor.current_line->content);
            free(editor.current_line);

            int row = editor.viewport.cursor_row;
            memmove(editor.lines + row, editor.lines + row + 1, sizeof(Line*)*(editor.numlines - row));
            editor.lines = realloc(editor.lines, sizeof(Line*)*(editor.numlines - 1));
            editor.numlines -= 1;
            editor.viewport.cursor_row -= 1;
            editor.viewport.cursor_col = previous_line_length;
            editor.subwin = NEW_WINDOW();
            return;
        }
        if(editor.viewport.cursor_col > 0){
            editor.current_line->gap_start--;
            return;
        }
    }
}

void
concat_lines(Line *line1, Line *line2){
    char* content = line1->content;
    char* content_line2 = strdup_line_content(line2);
    size_t size = line1->length + line2->length;
    content = realloc(content, size + line2->length);
    memcpy(content + size, content_line2, strlen(content_line2));
    line1->length += line2->length;
}

/*
 * Remove gap buffer of a line's content
 * */
char*
strdup_line_content(Line* line){
    char* str = malloc(line->length + 1);
    str[line->length] = '\0';
    memcpy(str, line->content, (line->gap_start - line->content));
    memcpy(str + (line->gap_start - line->content), line->gap_end + 1, line->length - (line->gap_end - line->content));
    return str;
}

void
render_result(){
    WINDOW *win = editor.subwin;
    if (editor.subwin == NULL) {
        endwin();
        fprintf(stderr, "erro ao criar subwindow\n");
        exit(1);
    }
    werase(win);
    switch(editor.current_line->expr.tag){
        case (VALID): {
            wattron(win, COLOR_PAIR(2));
            wprintw(win, "%f", editor.current_line->expr.data.value);
            wattroff(win, COLOR_PAIR(2));
            break;
        }
        case (INVALID): {
            wattron(win, COLOR_PAIR(1));
            wprintw(win, "%s", editor.current_line->expr.data.error);
            wattroff(win, COLOR_PAIR(1));
            break;
        }
        case (VOID): {
            werase(win);
            break;
        }
    }
    wrefresh(win);
    move(editor.viewport.cursor_row, editor.viewport.cursor_col + 4);
    refresh();
}

Line* get_line(uint8_t idx){
    if(idx >= editor.numlines) return NULL;
    if(editor.viewport.cursor_row == idx) return editor.current_line;
    return editor.lines[idx];
}

Line*
mkline(char* str, Expression expr){
    Line* line = malloc(sizeof(Line));
    line->expr = expr;
    char *buff = malloc(sizeof(strlen(str) + 10));
    memcpy(buff, str, strlen(str));
    line->content = buff;
    line->gap_start = line->content + strlen(str);
    line->gap_end = line->content + strlen(str) + 10 - 1;
    line->length = strlen(str);
    line->capacity = strlen(str) + 10; // buffer real size;
    return line;
}

void
update_offset(){
    Viewport view = editor.viewport;
    if(view.cursor_col >= view.col_offset + view.numcol + 1) {
        view.col_offset = view.cursor_col - view.numcol + 1;
    }
    if(view.cursor_col < view.col_offset){
        view.col_offset = view.cursor_col;
    }
}

void insert_char(Line *line, char c) {
    if (line->gap_start <= line->gap_end) {
        *line->gap_start = c;
        line->gap_start += 1;
        line->length += 1;
        editor.viewport.cursor_col += 1;
        return;
    }

    size_t grow = 10;
    size_t old_capacity = line->capacity;

    size_t gs = (size_t)(line->gap_start - line->content);
    size_t ge = (size_t)(line->gap_end - line->content);

    char *new_content = realloc(line->content, old_capacity + grow);
    if (new_content == NULL) {
        ex("Error: impossible realloc string");
    }

    line->content = new_content;
    line->capacity = old_capacity + grow;

    line->gap_start = line->content + gs;
    line->gap_end   = line->content + ge;

    size_t right_size = old_capacity - (ge + 1);

    memmove(line->gap_end + 1 + grow,
            line->gap_end + 1,
            right_size);

    line->gap_end += grow;

    *line->gap_start = c;
    line->gap_start += 1;
    line->length += 1;
    editor.viewport.cursor_col += 1;
}

char*
line_to_str(Line* line){
    char* str = malloc(line->length + 1);
    memcpy(str, line->content, line->length);
    str[line->length] = '\0';
    return str;
}
