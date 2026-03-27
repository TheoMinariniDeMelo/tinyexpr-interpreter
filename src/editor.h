#pragma once
#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>

#define ex(msg) do{perror(msg);\
    exit(1);\
}while(false);

#define NEW_WINDOW() newwin(1, 40, editor.viewport.cursor_row + 1, 5)

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
    size_t capacity; // buffer real size;
    Expression expr;
} Line;

typedef struct {
    int row_offset;
    int col_offset;
    int cursor_row;
    int cursor_col;
    int numrow;
    int numcol;
} Viewport;

typedef struct {
    Viewport viewport;
    Line** lines;
    Line* current_line;
    int numlines;
    WINDOW* subwin;
} Editor;

void
init_editor();

void
render();

void
process_key();

void
render_result();

char*
strdup_line_content(Line*);
