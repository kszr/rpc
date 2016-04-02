#include <stdlib.h>
#include <stdio.h>
#include "steque.h"

void steque_init(steque_t *ths){
  ths->front = NULL;
  ths->back = NULL;
  ths->N = 0;
}

void steque_enqueue(steque_t* ths, steque_item item){
  steque_node_t* node;

  node = (steque_node_t*) malloc(sizeof(steque_node_t));
  node->item = item;
  node->next = NULL;
  
  if(ths->back == NULL)
    ths->front = node;
  else
    ths->back->next = node;

  ths->back = node;
  ths->N++;
}

void steque_push(steque_t* ths, steque_item item){
  steque_node_t* node;

  node = (steque_node_t*) malloc(sizeof(steque_node_t));
  node->item = item;
  node->next = ths->front;

  if(ths->back == NULL)
    ths->back = node;
  
  ths->front = node;
  ths->N++;
}

int steque_size(steque_t* ths){
  return ths->N;
}

int steque_isempty(steque_t *ths){
  return ths->N == 0;
}

steque_item steque_pop(steque_t* ths){
  steque_item ans;
  steque_node_t* node;
  
  if(ths->front == NULL){
    fprintf(stderr, "Error: underflow in steque_pop.\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  node = ths->front;
  ans = node->item;

  ths->front = ths->front->next;
  if (ths->front == NULL) ths->back = NULL;
  free(node);

  ths->N--;

  return ans;
}

void steque_cycle(steque_t* ths){
  if(ths->back == NULL)
    return;
  
  ths->back->next = ths->front;
  ths->back = ths->front;
  ths->front = ths->front->next;
  ths->back->next = NULL;
}

steque_item steque_front(steque_t* ths){
  if(ths->front == NULL){
    fprintf(stderr, "Error: underflow in steque_front.\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  return ths->front->item;
}

void steque_destroy(steque_t* ths){
  while(!steque_isempty(ths))
    steque_pop(ths);
}
