#include "builtin.h"
#include "adds.h"

#define BUFFER_SIZE 2048

void sh_loop(void);
char *read_line(void);
char **read_args(char *line);
int launch(char **args);
int execute(char **args);
void free_all();

int (*builtin_func[]) (char **) = {
  &sh_help,
  &sh_exit,
  &sh_cd,
  &sh_history
};

void free_all() {
  //free(login);
  login = NULL;
  free(host);
  host = NULL;
  free(path);
  path = NULL;
}

int main(int argc, char **argv){
  size_t size = sizeof(char) * ARG_BUF;

  /*  login = calloc(1, size);//memeset in the same time
  if(login == NULL) {
    return 1;
    }*/

  host = calloc(1, size);
  if(host == NULL) {
    free(login);
    return 1;
  }

  path = calloc(1, size);
  if(path == NULL) {
    free(login);
    free(host);
    return 1;
  }

  set_input_mode();

  //get path
  char buf[ARG_BUF];

  if((getcwd(buf, ARG_BUF * sizeof(char))) == NULL){
    perror("getcwd() error");
    free_all();
    return 1;
  }

  strcpy(path, buf);

  //get login
  if((login = getpwuid(getuid())->pw_name) == NULL){
    fprintf(stderr, "error, can't find login\n");
    free_all();
    return 1;
  }

  //get hostname
  if(gethostname(host, ARG_BUF) == -1){
    fprintf(stderr, "error, can't find hostname\n");
    free_all();
    return 1;
  }

  reformat_path();
  sh_loop();

  clean();
  free_all();

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
    line = read_line();//pass a buffer alocated in sh_loop here
    args = read_args(line);
    cont = execute(args);
    free(args);
  }while(cont);
}

char *read_line(){
  char *line = calloc(1, BUFFER_SIZE * sizeof(char));//maybe use an array here
  if(line == NULL) {
    fprintf(stderr, "cannot allocate memory\n");
    return NULL;
  }

  //  char *orig = line;
  if((get_cmd(line)) == NULL){//line = not keep probably
    free(line);
    return NULL;
  }

  add_command(line);

  return line;
}

//parse the given line into a string of separate words

char **read_args(char *line){//rename to parse
  printf("in args\n");

  if(line == NULL){
    return NULL;
  }

  char **args = calloc(ARG_BUF, sizeof(char*));//table where to store the parsed version
  if(args == NULL) {
    fprintf(stderr, "cannot allocate memory\n");
    return NULL;
  }
  char **arg_orig = args;//pointer to the beginning of thez array
  char *readed = line;

  char *orig = readed;//needed ?

  //iterate throught the line to parse it
  while((*readed = *line++)){
    if(*readed == ' '){
      *readed = '\0';
      *args++ = orig;//orig point to the beginning of the (last) word
      orig = readed + 1;//update orig to the next word beginin
    }

    readed++;
  }
  *args = orig;
  readed = NULL;
  readed = orig;
  printf("readed : %s\n", readed);
  return arg_orig;
}

int execute(char **args){
  printf("executing ..\n");
  extern char *built_in[];//wtf

  if(args == NULL || *args[0] == '\0'){
    return 1;
  }

  for(int i = 0; i < nb_built_in(); ++i){
    if(strcmp(args[0], built_in[i]) == 0){
      printf("found ! \n");
      return (*builtin_func[i])(args);
    }
  }
  return launch(args);
}

int launch(char **args){
  pid_t pid, wpid;
  int status, fail;

  fail = 0;
  if((pid = fork()) == 0) {
    //= 0 <=> child process
    if(execvp(args[0], args) == -1) {
      perror("shell spawn error");
      //free(args);
      args = NULL;
      fail = 1;
      return sh_exit(args);//exiting child process in case of error
    }
  } else if (pid == -1) {
    fprintf(stderr, "cannot create child process");
  } else {
    //pid = child pid in the parent
    if((wpid = waitpid(pid, &status, WUNTRACED)) == -1) {//Wuntraced return if child has stoped
      perror("shell");
    }
    //if(fail == 1) {
      set_input_mode();
      //}
    //put back the right input mode deleted in the child
    //might find some better way to do this
  }

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
  char buf[64];

  //initialize the buffer to an empty one
  memset(buf, 0, 64 * sizeof(char));

  int position = 0;
  char* new_path = path;
  char* orig = new_path;
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

      } else if (home == 1) {
        home = 0;
        *new_path++ = '/';
        for(int i = 0; i < 4; ++i){
          *new_path++ = HOME[i];
        }
        *new_path++ = '/';
        for(int i = 0; i < position; ++i){
          *new_path++ = buf[i];
        }

      } else  {
        if(position > 0) {
          *new_path++ = '/';
        }
        for(int i = 0; i < position; ++i){
          *new_path++ = buf[i];
        }
      }

      for(int i = 0; i < position+1; ++i){//use memset
        buf[i] = '\0';
      }
      position = 0;
    } else {
      buf[position] = *curr;
      ++position;

      }
    ++curr;
  }
  *new_path = '\0';

  if(strcmp("\0", orig) == 0){
    *new_path++ = '/';
    *new_path = '\0';
  }
  //curr = NULL;
  //free(path);
  //path = NULL;
  path = orig;
}
