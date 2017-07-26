#include "builtin.h"
#include "adds.h"

char *built_in[] = {"help", "exit", "cd", "history"};

int nb_built_in(void){
  return sizeof(built_in)/sizeof(char*);
}

int sh_cd(char **args){
   if(args[1] == NULL){
    chdir(getenv("HOME"));
    if((getcwd(path, ARG_BUF * sizeof(char))) == NULL){//cannot obtain path to HOME
      perror("getcwd() error");
    }
    reformat_path();
  } else {
    if(args[1][0] == '~'){
      if(args[1][1] == '\0'){
        args[1] = getenv("HOME");
      } else {
        chdir(getenv("HOME"));
        strcpy(args[1], args[1]+2);
      }
    }

    if(chdir(args[1]) != 0){
      perror("chdir error");
    } else {
      if((getcwd(path, ARG_BUF * sizeof(char))) == NULL){
        perror("getcwd error");
      }
      reformat_path();
    }
  }

  return 1;
}

int sh_help(char **args){
  if(args[1] == NULL){
    printf("Welcome to shell ! \n\n");
    printf("Use help name to know more about an internal shell command(following list)\n");
    printf("Use man -k to know more about commands not in this list\n");
    printf("Here is the list of builtin commands : \n");

    for(int i = 0; i < nb_built_in(); ++i){
      printf("%s\n", built_in[i]);
    }

    } else {
    if(strcmp(args[1], built_in[0])){
      printf("Print some help about the shell. %s\n", built_in[0]);
    } else if (strcmp(args[1], built_in[2])){
      printf("Change the shell working directory by moving to the directory given as argument.\n");
    } else if (strcmp(args[1], built_in[1])){
      printf("Quit the shell.\n");
    } else {
      printf("Not a builtin command");
    }
  }
  return 1;
}

int sh_exit(char **args){
  (void) args;
  return 0;
}

int sh_history(char **args){
  (void) args;
  print_hist();
  return 1;
}
