#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int sh_help(char **);
int sh_exit(char **);
int sh_cd(char **);
int nb_built_in(void);
void reformat_path(void);//change header name?
char getch(void);
void ungetch(void);


#define HOME "home"
#define ARG_BUF 64

//colors
#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"

//define extra keys
#define UP "^[[A"
#define DOWN "^[[B"

//global variables
char *path;
char *login;
char *host;
char *buffer;
