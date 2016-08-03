#include "builtin.h"
#include "adds.h"

#define BUFFER_SIZE 2048

void sh_loop(void);
char *read_line(void);
char **read_args(char *line);
int launch(char **args);
int execute(char **args);
//void reformat_path();//put in builtin.h but rename the file

int (*builtin_func[]) (char **) = {
  &sh_help,
  &sh_exit,
  &sh_cd
};


int main(int argc, char **argv){
  //add configuration files
  login = malloc(sizeof(char) * ARG_BUF);
  host = malloc(sizeof(char) * ARG_BUF);

  set_input_mode();

  //get path
  if((path = getcwd(NULL, 0)) == NULL){//getcwd allocate dynamically the path
    perror("getcwd() error");
    return 1;
  }
  //printf("path : %s\n", path);

  //get login
  if((login = getpwuid(getuid())->pw_name) == NULL){
    fprintf(stderr, "error, can't find login\n");
    return 1;
  }
  //printf("initial login : %s\n", login);

  //get hostname
  if(gethostname(host, ARG_BUF) == -1){
    fprintf(stderr, "error, can't find hostname\n");
    return 1;
  }

  reformat_path();
  sh_loop();
  printf("out of the loop\n");

  clean();//free the linked list segfault with 1 in the linked list
  free(host);
  free(path);
  free(login);
  return EXIT_SUCCESS;
}

void sh_loop(void){
  printf("start loop...\n");
  char *line;
  char **args;
  int cont;//if exit, close the shell
  do {
    fprintf(stdout, "%s%s%s@%s%s%s:%s%s\n->", MAG, login, NRM, BLU, host, RED, path, NRM);
    fflush(stdout);
    line = read_line();
    //printf("line : %s\n", line);
    args = read_args(line);

    /*for(int i = 0; i < 4; ++i){
      printf("args : %s \n", args[i]);
      }*/

    cont = execute(args);
    //printf("cont = %d\n", cont);
  }while(cont);
}

char *read_line(){//don't forget to handle erasures
  char *line = malloc(sizeof(char) * BUFFER_SIZE);//line = cursor ?
  if(!line){
    fprintf(stderr, "cannot allocate memory");
  }
  char *orig = line;
  if(get_cmd(line) == NULL){
    return NULL;
  }

  /*while((c = getchar()) != '\n'){
    *line++ = c;

    //for this use getch and ungetch
    if(c == '\033'){//escape value
      getchar();//skip [

      if((c = getchar()) == 'A'){
        printf("%s\n", get_previous_cmd());

      } else if(c == 'B'){
        printf("%s\n",  get_next_cmd());
      }
      if((c = getchar()) != '\n'){//get rid of the last \n if present
        //TODO
      }
      return NULL;
    }

    //printf("added : %c\n", c);
  }
  *line = '\0';
  //printf("orig :%s\n", orig);*/
  add_command(orig);

  return orig;
}

//parse the given line into a string of separate words

char **read_args(char *line){//rename to parse
  printf("in args\n");

  if(line == NULL){
    return NULL;
  }

  char **args = calloc(ARG_BUF, sizeof(char)*32);//table where to store the parsed version
  char **arg_orig = args;//pointer to the beginning of thez array
  char *readed = malloc(sizeof(char) * BUFFER_SIZE);//temporary store all words readed

  if(!readed || !args){
    fprintf(stderr, "cannot allocate memeory");
  }

  char *orig = readed;//needed ?

  //iterate throught the line to parse it
  while((*readed = *line++)){
    if(*readed == ' '){//add some delimiters ?
      *readed = '\0';
      *args++ = orig;//orig point to the beginning of the (last) word
      orig = readed + 1;//update orig to the next word beginin
      //printf("line : %s\n", line);
    }

    readed++;
    //printf("line : %s\n", line);
  }
  *args = orig;//see here
  readed = NULL;
  readed = orig;
  printf("readed : %s\n", readed);
  return arg_orig;
}

int execute(char **args){
  printf("executing ..\n");
  extern char *built_in[];

  if(args == NULL || *args[0] == '\0'){
    return 1;
  }
  //printf("nb : %d\n", nb_built_in());

  for(int i = 0; i < nb_built_in(); ++i){
    //printf("%d  %s\n", i, built_in[i]);
    if(strcmp(args[0], built_in[i]) == 0){
      //printf("here at i = %d\n", i);
      return (*builtin_func[i])(args);
    }
  }
  return launch(args);
}

int launch(char **args){
  printf("launch\n");
  pid_t pid, wpid;
  int status;

  if((pid = fork()) == 0){
    //= 0 <=> child process
    if(execvp(args[0], args) == -1){
      perror("shell spawn error");
      return sh_exit(args);//exiting child process in case of error
    }
  } else if (pid == -1) {
    //error here
    fprintf(stderr, "cannot create child process");
  } else {
    //pid = child pid
    if((wpid = waitpid(pid, &status, WUNTRACED)) == -1){//Wuntraced return if child has stoped
      perror("shell");//status ?
    }
  }
  free(args);
  return 1;
}

/*
 * ! : take care about /home/dir
 * iterate throught the path, the buffer is clear at / (strcpy)
 * if needed we add the buffer to the new pat
 * we only some part of it
 *
 * reformat this function
 */

void reformat_path(){
  char *curr = path;
  char *old = path;
  char buf[64];

  //initialize the buffer to an empty one
  for(int i = 0; i < 64; ++i){
    buf[i] = '\0';
  }

  int position = 0;
  char *new_path = calloc(1, 2048);//change the 2048, temporary
  char *orig = new_path;
  int home = 0;
  int last = 1;

  while(*curr != '\0' || last){
    if(*curr == '\0'){
      last = 0;
    }
    if(*curr == '/' || last == 0){
      //printf("buf : %s\n", buf);
      if(strcmp(HOME, buf) == 0){//here in home
        home = 1;
        //printf("here home\n");
        if(last == 0){
          *new_path++ = '/';
          for(int i = 0; i < position; ++i){
            *new_path++ = buf[i];
          }
        }

      } else if (home == 1 && strcmp(login, buf) == 0){//here in /home/user
        home = 0;
        *new_path++ = '~';//replace /home/usr by ~
        if(last == 0){
          *new_path++ = '/';
        }
        printf("here %s\n", login);

      } else if (home == 1) {
        //printf("here = 1 : %s, %s and %d\n", login, buf, strcmp(login, buf));
        home = 0;
        *new_path++ = '/';
        for(int i = 0; i < 4; ++i){
          *new_path++ = HOME[i];
        }
        *new_path++ = '/';
        for(int i = 0; i < position; ++i){
          *new_path++ = buf[i];
        }
        //printf("here end 1\n");

      } else  {
        //printf("here updtate\n");
        if(position > 0) {
          *new_path++ = '/';
        }
        for(int i = 0; i < position; ++i){
          *new_path++ = buf[i];
        }
      }

      for(int i = 0; i < position+1; ++i){//buffer reset
        buf[i] = '\0';
      }
      position = 0;
      //printf("buf after clear %s\n", buf);
    } else {
      buf[position] = *curr;
      ++position;
      //printf("here end %c\n", *curr);
      //printf("buf : %s\n", buf);
      }
    ++curr;
  }
  *new_path = '\0';
  if(strcmp("\0", orig) == 0){
    *new_path++ = '/';
    *new_path = '\0';
  }
  free(old);//free the old path
  path = orig;
  //printf("path after formating : %s\n", path);
}
