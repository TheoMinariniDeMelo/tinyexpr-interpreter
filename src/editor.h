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

void
on_resize(int sig);

int
is_operation(char ch);

Line*
get_line(uint8_t idx);

Line*
mkline(char* str, Expression expr);

void
update_offset();

void
insert_char(Line* line, char c);

void
concat_lines(Line* line1, Line* line2);

char*
strdup_line_content(Line* line);

char*
line_to_str(Line* line);
