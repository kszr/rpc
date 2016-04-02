#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "indexrndq.h"

void indexrndq_init(indexrndq_t* ths, int NMAX){
  int i;
  
  ths->NMAX = NMAX;
  ths->N = 0;
  ths->pi = (int*) malloc(sizeof(int) * NMAX);
  ths->ip = (int*) malloc(sizeof(int) * NMAX);

  for(i = 0; i < NMAX; i++)
    ths->ip[i] = -1;
}

int indexrndq_isempty(indexrndq_t* ths){
  return ths->N == 0;
}

void indexrndq_enqueue(indexrndq_t* ths, int i){
  if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexrndq_enqueue out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  if (indexrndq_contains(ths, i)){
    fprintf(stderr, "Error: index %d is already in the indexrndq structure.", i);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  ths->ip[i] = ths->N;
  ths->pi[ths->N] = i;
  ths->N++;

  return;
}

int indexrndq_contains(indexrndq_t* ths, int i){
  return ths->ip[i] != -1;
}

void indexrndq_delete(indexrndq_t* ths, int i){
   if (i < 0 || i >= ths->NMAX){
    fprintf(stderr, "Error: Argument to indexrndq_contains out of bounds\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  if (!indexrndq_contains(ths, i)){
    fprintf(stderr, "Error: index %d is not in the indexrndq structure.", i);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  
  ths->N--;
  ths->pi[ths->ip[i]] = ths->pi[ths->N];
  ths->ip[ths->pi[ths->N]] = ths->ip[i];
  ths->ip[i] = -1;
}

int indexrndq_dequeue(indexrndq_t* ths){
  int ans, k;

  /* Testing for underflow */
  if(ths->N <= 0){
    fprintf(stderr, "Error: indexrndq underflow\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  k = (int) (1.0 * ths->N * rand() / (RAND_MAX + 1.0));
  ans = ths->pi[k];

  --ths->N;

  ths->pi[k] = ths->pi[ths->N];
  ths->ip[ths->pi[k]] = k;
  ths->ip[ans] = -1;

  return ans;
}

void indexrndq_destroy(indexrndq_t* ths){
  free(ths->pi);
  free(ths->ip);
}
