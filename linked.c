#include "adds.h"

list_data *create(){
  return calloc(1, sizeof(list_data));//see what calloc do !!
}

void destroy(list_data *l){//check for any error in freeing pointers ?
  foreach(node, l){
    if(node->prev) {
      free(node->prev->s);
      free(node->prev);
    }
  }
  free(l->last->s);
  free(l->last);
  free(l);
}

//add the new elements at the beginning of the list
//if the list is full, remove the last element
void put(list_data *l, char *s){
  t_node *node = calloc(1, sizeof(t_node));

  node->s = s;

  if(l->last == NULL){//list is empty
    l->head = l->last = l->curr = node;

  } else {//list has at least one element
    l->head->next = node; //find why this don't work
    node->prev = l->head;
    l->head = l->curr = node;
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

int prev;

//sometimes curr goes to NULL -> get rid of this

char *get_next(list_data *l){
  if(l->curr == NULL){
    return "";//end of the list
  }

  if(prev == 2){
    l->curr = l->curr->next;
  }

  char *to_return = l->curr->s;

  if(prev != 2){
    l->curr = l->curr->next;
  }

  prev = 1;
  return to_return;
}

char *get_previous(list_data *l){
  if(l->curr == NULL){
    return "";
  }

  if(prev == 1){
    l->curr = l->curr->prev;
  }

  char *to_return = l->curr->s;

  if(prev != 1){
    l->curr = l->curr->prev;
  }

  prev = 2;
  return to_return;
}
