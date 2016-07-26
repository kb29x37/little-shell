#ifndef adds_h
#define adds_h

#include <sys/types.h>//getuid
#include <pwd.h>//getuid
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 512

//double linked list related stuff

typedef struct t_node {
  char* s;
  struct t_node *next;//more recent typed direction
  struct t_node *prev;//less recent typed direction
} t_node;

typedef struct list_data {
  int size;
  t_node *head;
  t_node *curr;
  t_node *last;
} list_data;


list_data *create(void);
void destroy(list_data *);

void put(list_data *, char *);
char *get_next(list_data *);
char *get_previous(list_data *);


//application of the linked list related stuff

void add_command(char *);
char *get_next_cmd(void);
char *get_previous_cmd(void);
void clean(void);

//useful maccro

#define foreach(item, list) \
  for(t_node *item = list->head; item != NULL; item = item->next)

#endif
