#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.c"
typedef struct Ast Ast; // forward

typedef enum {
  AST_NUM,
  AST_IDENT,
  AST_BINARY,
  AST_ASSIGN,
  AST_RETURN
} AstKind;

struct Ast {
  AstKind kind;
  int line, col;
  union {
    struct { double value; } num;
    struct { char* name; } ident;
    struct { int op; Ast* left; Ast* right; } binary; // op: TokenType
    struct { char* name; Ast* value; } assign;
    struct { Ast* value; } ret;
  };
};

typedef struct {
  Lexer lx;
  Token current;
  int had_error;
} Parser;

/* ---------- AST helpers ---------- */

static Ast* ast_new(AstKind kind, int line, int col) {
  Ast* n = (Ast*)calloc(1, sizeof(Ast));
  if (!n) return NULL;
  n->kind = kind;
  n->line = line;
  n->col = col;
  return n;
}

static Ast* ast_num(double v, int line, int col) {
  Ast* n = ast_new(AST_NUM, line, col);
  n->num.value = v;
  return n;
}

static Ast* ast_ident(const char* name, int line, int col) {
  Ast* n = ast_new(AST_IDENT, line, col);
  n->ident.name = strdup(name);
  return n;
}

static Ast* ast_binary(int op, Ast* l, Ast* r, int line, int col) {
  Ast* n = ast_new(AST_BINARY, line, col);
  n->binary.op = op;
  n->binary.left = l;
  n->binary.right = r;
  return n;
}

static Ast* ast_assign(const char* name, Ast* value, int line, int col) {
  Ast* n = ast_new(AST_ASSIGN, line, col);
  n->assign.name = strdup(name);
  n->assign.value = value;
  return n;
}

static Ast* ast_return(Ast* value, int line, int col) {
  Ast* n = ast_new(AST_RETURN, line, col);
  n->ret.value = value;
  return n;
}

static void ast_free(Ast* n) {
  if (!n) return;
  switch (n->kind) {
    case AST_IDENT: free(n->ident.name); break;
    case AST_BINARY:
      ast_free(n->binary.left);
      ast_free(n->binary.right);
      break;
    case AST_ASSIGN:
      free(n->assign.name);
      ast_free(n->assign.value);
      break;
    case AST_RETURN:
      ast_free(n->ret.value);
      break;
    case AST_NUM:
    default:
      break;
  }
  free(n);
}

/* ---------- Parser core ---------- */

static void parser_init(Parser* p, const char* src) {
  lex_init(&p->lx, src);
  p->current = next_token(&p->lx);
  p->had_error = 0;
}

static void parser_advance(Parser* p) {
  free_token(&p->current);
  p->current = next_token(&p->lx);
}

static int parser_match(Parser* p, TokenType t) {
  if (p->current.type != t) return 0;
  parser_advance(p);
  return 1;
}

static void parser_error(Parser* p, const char* msg) {
  fprintf(stderr, "Parse error at %d:%d: %s (got '%s')\n",
          p->current.line, p->current.col, msg, p->current.lexeme);
  p->had_error = 1;
}

static void parser_consume(Parser* p, TokenType t, const char* msg) {
  if (p->current.type == t) {
    parser_advance(p);
    return;
  }
  parser_error(p, msg);
}

/* ---------- Forward declarations for grammar ---------- */
static Ast* parse_expr(Parser* p);
static Ast* parse_term(Parser* p);
static Ast* parse_factor(Parser* p);
static Ast* parse_stmt(Parser* p);

/* ---------- Grammar implementation ---------- */

/*
factor → NUMBER | IDENT | "(" expr ")"
*/
static Ast* parse_factor(Parser* p) {
  int line = p->current.line, col = p->current.col;

  if (p->current.type == TOK_NUMBER) {
    double v = strtod(p->current.lexeme, NULL);
    parser_advance(p);
    return ast_num(v, line, col);
  }

  if (p->current.type == TOK_IDENT) {
    Ast* n = ast_ident(p->current.lexeme, line, col);
    parser_advance(p);
    return n;
  }

  if (parser_match(p, TOK_LPAREN)) {
    Ast* e = parse_expr(p);
    parser_consume(p, TOK_RPAREN, "expected ')'");
    return e;
  }

  parser_error(p, "expected NUMBER, IDENT, or '('");
  return NULL;
}

/*
term → factor (("*" | "/") factor)*
*/
static Ast* parse_term(Parser* p) {
  Ast* left = parse_factor(p);
  while (!p->had_error && (p->current.type == TOK_STAR || p->current.type == TOK_SLASH)) {
    int op = p->current.type;
    int line = p->current.line, col = p->current.col;
    parser_advance(p);
    Ast* right = parse_factor(p);
    left = ast_binary(op, left, right, line, col);
  }
  return left;
}

/*
expr → term (("+" | "-") term)*
*/
static Ast* parse_expr(Parser* p) {
  Ast* left = parse_term(p);
  while (!p->had_error && (p->current.type == TOK_PLUS || p->current.type == TOK_MINUS)) {
    int op = p->current.type;
    int line = p->current.line, col = p->current.col;
    parser_advance(p);
    Ast* right = parse_term(p);
    left = ast_binary(op, left, right, line, col);
  }
  return left;
}

/*
stmt → "return" expr ";" 
     | ident "=" expr ";"
*/
static Ast* parse_stmt(Parser* p) {
  int line = p->current.line, col = p->current.col;

  // return statement (we used TOK_KEYWORD with lexeme = "return")
  if (p->current.type == TOK_KEYWORD && strcmp(p->current.lexeme, "return") == 0) {
    parser_advance(p);
    Ast* value = parse_expr(p);
    parser_consume(p, TOK_SEMI, "expected ';' after return");
    return ast_return(value, line, col);
  }

  // assignment: IDENT = expr ;
  if (p->current.type == TOK_IDENT) {
    char* name = strdup(p->current.lexeme);
    parser_advance(p);
    parser_consume(p, TOK_ASSIGN, "expected '=' after identifier");
    Ast* value = parse_expr(p);
    parser_consume(p, TOK_SEMI, "expected ';' after assignment");
    Ast* node = ast_assign(name, value, line, col);
    free(name);
    return node;
  }

  parser_error(p, "expected statement (return or assignment)");
  return NULL;
}

/*
program → stmt* EOF
For simplicity: parse until EOF, returning an array-like list.
*/
typedef struct {
  Ast** items;
  size_t len;
  size_t cap;
} AstList;

static void list_push(AstList* l, Ast* n) {
  if (!n) return;
  if (l->len == l->cap) {
    size_t newcap = l->cap ? l->cap * 2 : 8;
    l->items = (Ast**)realloc(l->items, newcap * sizeof(Ast*));
    l->cap = newcap;
  }
  l->items[l->len++] = n;
}

static AstList parse_program(Parser* p) {
  AstList out = {0};
  while (!p->had_error && p->current.type != TOK_EOF) {
    Ast* s = parse_stmt(p);
    list_push(&out, s);
  }
  return out;
}

/* ---------- Debug print ---------- */

static void ast_print(Ast* n, int indent) {
  if (!n) { printf("%*s(null)\n", indent, ""); return; }

  switch (n->kind) {
    case AST_NUM:
      printf("%*sNUM(%g)\n", indent, "", n->num.value);
      break;
    case AST_IDENT:
      printf("%*sIDENT(%s)\n", indent, "", n->ident.name);
      break;
    case AST_BINARY:
      printf("%*sBINARY(op=%d)\n", indent, "", n->binary.op);
      ast_print(n->binary.left, indent + 2);
      ast_print(n->binary.right, indent + 2);
      break;
    case AST_ASSIGN:
      printf("%*sASSIGN(%s)\n", indent, "", n->assign.name);
      ast_print(n->assign.value, indent + 2);
      break;
    case AST_RETURN:
      printf("%*sRETURN\n", indent, "");
      ast_print(n->ret.value, indent + 2);
      break;
  }
}

/* ---------- Example usage ---------- */

int main(void) {
  const char* src =
    "x = 10 + 2 * 3;\n"
    "return x;\n";

  Parser p;
  parser_init(&p, src);

  AstList prog = parse_program(&p);

  if (!p.had_error) {
    for (size_t i = 0; i < prog.len; i++) {
      ast_print(prog.items[i], 0);
    }
  }

  // cleanup
  for (size_t i = 0; i < prog.len; i++) ast_free(prog.items[i]);
  free(prog.items);
  free_token(&p.current);

  return p.had_error ? 1 : 0;
}
