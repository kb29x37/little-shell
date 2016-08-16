#include "adds.h"

list_data *create(){
  return calloc(1, sizeof(list_data));
}

int *dir; //if add reset

void destroy(list_data *l){
  //printf("last is : %s\n", l->last->s);
  foreach(node, l){
    if(node->next) {
      //printf("node->prev->s %s  %s\n", node->next->s, node->s);
      free(node->next->s);
      free(node->next);
      //printf("no supposed to\n");
    }
  }
  free(l->last->s);
  free(l->last);
  free(l);
  if(dir){
    free(dir);
  }
}

//add the new elements at the beginning of the list
//if the list is full, remove the last element
void put(list_data *l, char *s){
  t_node *node = calloc(1, sizeof(t_node));
  dir = 0;

  node->s = s;

  if(l->last == NULL){//list is empty
    l->head = l->last = l->curr = node;

  } else {//list has at least one element
    l->head->next = node;
    node->prev = l->head;
    l->head = l->curr = node;
    if(l->last->next == NULL){
      l->last->next = node;
    }
  }

  if(l->size == MAX){//delete the last node
    free(l->last->s);
    l->last = l->last->prev;
    free(l->last->next);
    l->last->next = NULL;
  } else {
    l->size++;
  }
}

//if curr goes to null send it back the value of the head or of the last
//if any change in direction, tail twice the position in the linked list
//don't care of null because of head and last (see with direction)

//some problems with up and down directly -> goes to NULL and try to read on null

char *get_next(list_data *l){
  //print_list(l);
  if(!dir){
    dir = calloc(1, sizeof(int));
  }
  //printf("dir %d\t", *dir);
  if(l->curr == NULL){
    //printf("curr null\n");
    if(l->size == 1){
      l->curr = l->head;
      return "";
    } else {
      l->curr = (*dir == NEXT) ? l->head : l->last->next;
      *dir = NEXT;
    }
  }

  char *to_return = l->curr->s;

  if(*dir == NEXT || *dir == 0){//same direction as before
    if(l->size == 1){
      return "";
    }
    l->curr = l->curr->next;
  } else if(l->size == 1){
    return "";
  } else {
    /*if(l->curr->next != NULL){
      //printf("that's normal : %s\n", l->curr->next->s);
    }*/
    l->curr = (l->curr->next->next == NULL) ? l->head : l->curr->next->next;
    to_return = l->curr->s;
  }

  *dir = NEXT;
  return to_return;
}

char *get_previous(list_data *l){
  //print_list(l);
  if(!dir){
    dir = calloc(1, sizeof(int));
  }
  //printf("dir %d\t", *dir);
  if(l->curr == NULL){
    //printf("curr null\n");
    if(l->size == 1){
      l->curr = l->head;
      return l->curr->s;
    } else {
      l->curr = (*dir == PREV) ? l->last : l->head->prev;
      *dir = PREV;
    }
  }

  char *to_return = l->curr->s;

  if(*dir == PREV || *dir == 0) {
    l->curr = l->curr->prev;
  } else if(l->size == 1) {
    l->curr = l->head;
    return l->head->s;
  } else {
    //printf("diff dir\n");
    l->curr = (l->curr->prev->prev == NULL) ? l->last : l->curr->prev->prev;
    to_return = l->curr->s;
  }

  *dir = PREV;
  return to_return;
}

void print_list(list_data *l){
  foreach(node, l){
    printf("%s\n", node->s);
  }
}
