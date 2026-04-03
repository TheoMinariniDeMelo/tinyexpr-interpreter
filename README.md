# calc

`calc` is a small terminal calculator written in C. It uses `ncurses` for the interface and a handwritten lexer, parser, and AST evaluator for arithmetic expressions.

## Features

- Interactive terminal UI built with `ncurses`
- Live evaluation while you type
- Support for integers and decimal numbers
- Support for `+`, `-`, `*`, `/`, and parentheses
- Multi-line input, with one expression per line

## Requirements

- `gcc`
- `make`
- `ncurses`

On Debian/Ubuntu, the `ncurses` development package is usually:

```sh
sudo apt install libncurses-dev
```

## Build

```sh
make
```

This produces the executable `./calc`.

To remove build artifacts:

```sh
make clean
```

## Run

```sh
./calc
```

The program starts in an interactive screen. Type an expression on the current line and the result is rendered below it. Incomplete or invalid expressions show a parser or lexer error instead.

Example:

```text
>>> 1+2
    3.000000

>>> (3*4)/2
    6.000000
```

## Controls

- `0-9`, `.`, `+`, `-`, `*`, `/`, `(`, `)`, and space insert characters
- `Enter` creates a new line
- `Backspace` deletes within the current line
- `Backspace` at the start of a line merges it with the previous line
- `Ctrl+C` exits the program

## Supported Grammar

The current parser accepts arithmetic expressions in this form:

```ebnf
expression = term , { ("+" | "-") , term } ;
term       = factor , { ("*" | "/") , factor } ;
factor     = number | "(" , expression , ")" ;
number     = digit , { digit } ;
```

The lexer also supports decimal literals such as `3.14`.

## Project Layout

- `src/main.c`: editor loop and expression processing
- `src/editor.c`: terminal UI and line editing
- `src/lexer.c`: tokenization
- `src/parser.c`: recursive descent parser
- `src/ast.c`: expression evaluation
- `grammar.ebnf`: high-level grammar reference

## Notes

- This is currently an interactive calculator, not a one-shot CLI like `calc "1+2"`.
- A test scaffold exists under `src/tests/`, but there is no test target wired into the top-level `Makefile` yet.
