#include <stdint.h>
#include <signal.h>
#include <termios.h> 
#include <stdlib.h>
#include <unistd.h>
#include "./list.h"
struct termios orgi_termios;
List* list;

typedef struct {
    char*   infix_expression;
    double  value;
    uint8_t error;
} Expression;

void enable_raw_mode(){
    if(!isatty(STDIN_FILENO)){
        exit(1);
    }
    if(tcgetattr(STDIN_FILENO, &orgi_termios) == -1) exit(1);
    struct termios raw = orgi_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0; // quantidade mínima de bytes para a syscall read retornar o valor
    raw.c_cc[VTIME] = 1; // 1 mili segundos de delay para retorno do read
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void handle_int(int signal){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orgi_termios);
    exit(0);
}

int main(int, char**){
    signal(SIGINT, handle_int);
    enable_raw_mode();

    list = init_list();

    for(;;){
        
    }
}
