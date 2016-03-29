#include <stdlib.h>
#include <stdio.h>
#include "indexminpq.h"

static int greater_than(indexminpq_t* ths, int i, int j){
  return ths->keycmp(ths->keys[ths->pq[i]],ths->keys[ths->pq[j]]) > 0;
}

static void exch(indexminpq_t* ths, int i, int j){
  int swap;
  swap = ths->pq[i]; 
  ths->pq[i] = ths->pq[j]; 
  ths->pq[j] = swap;
  
  ths->qp[ths->pq[i]] = i; 
  ths->qp[ths->pq[j]] = j;
}

static void swim(indexminpq_t* ths, int k)  {
  while (k > 1 && greater_than(ths, k/2, k)) {
    exch(ths, k, k/2);
    k = k/2;
  }
}

static void sink(indexminpq_t* ths, int k) {
  int j;
  while (2*k <= ths->N) {
    j = 2*k;
    if (j < ths->N && greater_than(ths, j, j+1)) j++;
    if (!greater_than(ths, k, j)) break;
    exch(ths, k, j);
    k = j;
  }
}


void indexminpq_init(indexminpq_t* ths, int NMAX, int (*keycmp)(indexminpq_key a, indexminpq_key b)){
  int i;

  ths->NMAX = NMAX;
  ths->N = 0;
  ths->pq = (int*) malloc(sizeof(int) * (NMAX + 1));
  ths->qp = (int*) malloc(sizeof(int) * (NMAX + 1));
  ths->keys = (indexminpq_key*) malloc(sizeof(indexminpq_key) * (NMAX + 1));
  ths->keycmp = keycmp;

  for(i = 0; i <= NMAX; i++)
    ths->qp[i] = -1;
}

int indexminpq_isempty(indexminpq_t* ths){ 
  return ths->N == 0; 
}

int indexminpq_contains(indexminpq_t* ths, int i) {
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexminpq_contains out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  return ths->qp[i] != -1;
}

int indexminpq_size(indexminpq_t* ths) {
  return ths->N;
}

void indexminpq_insert(indexminpq_t* ths, int i, indexminpq_key key) {
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexminpq_contains out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  if (indexminpq_contains(ths, i)){
    fprintf(stderr, "Error: index %d is already in the indexminpq structure.", i);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
   
   ths->N++;
   ths->qp[i] = ths->N;
   ths->pq[ths->N] = i;
   ths->keys[i] = key;
   swim(ths, ths->N);
}

int indexminpq_minindex(indexminpq_t* ths) { 
  if (ths->N == 0){
    fprintf(stderr, "Error: underflow in indexminpq_minindex.");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  return ths->pq[1];        
}

indexminpq_key indexminpq_minkey(indexminpq_t* ths) { 
  if (ths->N == 0){
    fprintf(stderr, "Error: underflow in indexminpq_minkey.");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  return ths->keys[ths->pq[1]];        
}

int indexminpq_delmin(indexminpq_t* ths) { 
  int min;
  
  if(ths->N == 0){
    fprintf(stderr, "Error: underflow in indexminpq_delmin.");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  min = ths->pq[1];        
  exch(ths, 1, ths->N); 
  ths->N--;
  sink(ths, 1);
  ths->qp[min] = -1;  /* marking as deleted */
  return min; 
}

indexminpq_key indexminpq_keyof(indexminpq_t* ths, int i) {
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexminpq_keyof out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  if (!indexminpq_contains(ths, i)){
    fprintf(stderr, 
	    "Invalid argument to indexminpq_keyof: index %d is not in the priority queue", i);
    fflush(stderr);exit(EXIT_FAILURE);
  }
  return ths->keys[i];
}

void indexminpq_changekey(indexminpq_t* ths, int i, indexminpq_key key) {
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexminpq_changekey out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  if (!indexminpq_contains(ths, i)){
    fprintf(stderr,
	    "Invalid argument to indexminpq_changekey: index %d is not in the priority queue", i);
    fflush(stderr);
    exit(EXIT_FAILURE);  
  }
  ths->keys[i] = key;
  swim(ths, ths->qp[i]);
  sink(ths, ths->qp[i]);
}

void indexminpq_decreasekey(indexminpq_t *ths, int i, indexminpq_key key) {
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexminpq_decreasekey out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  if (!indexminpq_contains(ths, i)){
    fprintf(stderr,
	    "Invalid argument to indexminpq_decreasekey: index %d is not in the priority queue", i);
    fflush(stderr);
    exit(EXIT_FAILURE);  
  }
 
  if (ths->keycmp(ths->keys[i], key) < 0){
    fprintf(stderr,"Error: indexminpq_decreasekey called with increasing key");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
    
  ths->keys[i] = key;
  swim(ths, ths->qp[i]);
}

void indexminpq_increasekey(indexminpq_t *ths, int i, indexminpq_key key) {
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexminpq_increasekey out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  if (!indexminpq_contains(ths, i)){
    fprintf(stderr,
	    "Invalid argument to indexminpq_increasekey: index %d is not in the priority queue\n", i);
    fflush(stderr);
    exit(EXIT_FAILURE);  
  }
  
  if (ths->keycmp(ths->keys[i], key) > 0){
    fprintf(stderr,"Error: indexminpq_increasekey called with decreasing key\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  ths->keys[i] = key;
  sink(ths, ths->qp[i]);
}

void indexminpq_delete(indexminpq_t *ths, int i) {
  int index;
  
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexminpq_increasekey out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  if (!indexminpq_contains(ths, i)){
    fprintf(stderr,"Error: Invalid argument to indexminpq_delete: index %d is not in the priority queue", i);
    fflush(stderr);
    exit(EXIT_FAILURE);  
  }

  index = ths->qp[i];
  exch(ths, index, ths->N);
  ths->N--;
  swim(ths, index);
  sink(ths, index);
  ths->qp[i] = -1;
}

void indexminpq_destroy(indexminpq_t* ths){
  free(ths->pq);
  free(ths->qp);
  free(ths->keys);
}
