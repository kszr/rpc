#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hshtbl.h"

static int hshtbl_hv(hshtbl_t* ths, char *key){
  int i, ans;

  ans = 0;
  for(i = 0; key[i] != '\0'; i++)
    ans = (31 * ans + key[i]) % ths->M;
  
  return ans;
}

int hshtbl_init(hshtbl_t* ths, int M){
  int i;

  ths->N = 0;
  ths->M = M;
  ths->keys = (char**) malloc(sizeof(char*) * M);
  ths->items = (hshtbl_item*) malloc(sizeof(void*) * M);
  
  for(i = 0; i < M; i++){
    ths->keys[i] = NULL;
    ths->items[i] = NULL;
  }
  return 0;
} 

void hshtbl_put(hshtbl_t* ths, char *key, hshtbl_item item){
  int j;

  if(item == NULL){
    hshtbl_delete(ths, key);
    return;
  }

  if(ths->N == ths->M){
    fprintf(stderr, "Error: Overflow in hshtbl_put.\n");
    exit(EXIT_FAILURE);
   }
    
  for( j = hshtbl_hv(ths, key); ths->keys[j] != NULL; j = (j + 1) % ths->M){
    if( strcmp(key, ths->keys[j]) == 0 ){
      ths->items[j] = item;
      return;
    }
  }

  ths->keys[j] = key;
  ths->items[j] = item;
  ths->N++;
}

hshtbl_item hshtbl_get(hshtbl_t* ths, char *key){
  int j,k;
  
  for( j = hshtbl_hv(ths, key), k = 0; ths->keys[j] != NULL && k < ths->M; j = (j + 1) % ths->M, k++)
    if( strcmp(key, ths->keys[j]) == 0)
      return ths->items[j];
  return NULL;
}

void hshtbl_delete(hshtbl_t* ths, char* key){
  int j;
  char* key_to_rehash;
  void *item_to_rehash;

  if( hshtbl_get(ths, key) == NULL) 
    return;

  for( j = hshtbl_hv(ths,key); strcmp(key, ths->keys[j]) != 0; j = (j + 1) % ths->M);
  
  ths->keys[j] = NULL;
  ths->items[j] = NULL;

  j = (j+1) % ths->M;
  while(ths->keys[j] != NULL){
    key_to_rehash = ths->keys[j];
    item_to_rehash = ths->items[j];
    ths->keys[j] = NULL;
    ths->items[j] = NULL;
    ths->N--;
    hshtbl_put(ths, key_to_rehash, item_to_rehash);
    j = (j+1) % ths->M;
  }

  ths->N--;
}

void hshtbl_destroy(hshtbl_t* ths){
  free(ths->keys);
  free(ths->items);
}
