#include"adds.h"

static list_data *l;

void add_command(char *a){
  if(!l){
    l = create();
    printf("creation succeded\n");
  }
  put(l, a);
  printf("succeded to put in the structure\n");
}

char *get_next_cmd(){
  return !l ? "" : get_next(l);
}

char *get_previous_cmd(){
  return !l ? "" : get_previous(l);
}

void clean(){
  if(l){
    destroy(l);
  }
}
