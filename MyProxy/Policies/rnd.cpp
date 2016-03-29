#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gtcache.h"
#include "hshtbl.h"
#include "indexrndq.h"
#include "steque.h"

typedef struct{
  char* key;
  void* value;
  size_t val_size;
  int id;
} cache_entry_t;

static int min_size;
static int used_mem;
static int cache_capacity;

static cache_entry_t* cache;
static hshtbl_t tbl;
static steque_t available_ids;
static indexrndq_t eviction_queue;

static void deleteentry(cache_entry_t* e){
  /*
    e is assumed to have already been evicted from the priority queue.
  */
  used_mem -= e->val_size > min_size ? e->val_size : min_size;

  hshtbl_delete(&tbl, e->key);

  free(e->key);
  free(e->value);

  steque_push(&available_ids, e->id);
}

static cache_entry_t* createentry(char* key, void* value, size_t val_size){
  cache_entry_t* e;
  size_t key_len;

  if(steque_isempty(&available_ids)){
    fprintf(stderr, "ran out of ids for cache entries!\n");
    return NULL;
  }

  key_len = strlen(key) + 1;

  used_mem += val_size;

  e = &cache[steque_pop(&available_ids)];
  e->key = (char*) malloc(key_len);
  e->value = (void*) malloc(val_size);
  e->val_size = val_size;
  memcpy(e->key, key, key_len);
  memcpy(e->value, value, val_size);

  hshtbl_put(&tbl, e->key, e);
  indexrndq_enqueue(&eviction_queue, e->id);
  
  return e;
}

int gtcache_init(size_t capacity, size_t min_entry_size, int num_levels){
  int j, nmax;

  min_size = min_entry_size;
  cache_capacity = capacity;
  used_mem = 0;

  nmax = cache_capacity / min_size;

  hshtbl_init(&tbl, 2 * nmax);
  
  cache = (cache_entry_t*) malloc(nmax * sizeof(cache_entry_t));

  steque_init(&available_ids);
  indexrndq_init(&eviction_queue, nmax);
    
  for( j = 0; j < nmax; j++){
    cache[j].id = j;
    steque_push(&available_ids, j);
  }
  
  return 0;
}

void* gtcache_get(char *key, size_t* val_size){
  cache_entry_t* e;
  void* ans;

  e = hshtbl_get(&tbl, key);

  if(e == NULL)
    return e;
  
  /* Mark e as used, if necessary */
  
  if(val_size != NULL)
    *val_size = e->val_size;

  ans = malloc(e->val_size);
  memcpy(ans, e->value, e->val_size);
  
  return ans;
}

int gtcache_set(char *key, void *value, size_t val_size){
  int needed_size;
  cache_entry_t* e;
  
  needed_size = min_size > val_size ? min_size : val_size;

  if (needed_size > cache_capacity){
    /* It's hopeless. */
    fprintf(stderr, "Value exceeds cache limit\n");
    return 1;
  }
  
  e = hshtbl_get(&tbl, key);
  
  /* If it is already in the cache...*/
  if( e != NULL){
    if(e->val_size == val_size){
      memcpy(e->value, value, val_size);
      return 0;
    }
    else{
      indexrndq_delete(&eviction_queue, e->id);
      deleteentry(e);
    }
  }
  
  /* Now that we know it isn't already in cache, let's see if there is room */
  while(used_mem + needed_size > cache_capacity)
    deleteentry(&cache[indexrndq_dequeue(&eviction_queue)] );

  /* Create a new entry for the new element*/
  if( createentry(key, value, val_size) == NULL){
    fprintf(stderr, "gtcache_set: unable to create cache_entry\n");
    return -1;
  }

  return 0;
}

int gtcache_memused(){
  return used_mem;
}

void gtcache_destroy(){
  while( !indexrndq_isempty( &eviction_queue ) )
    deleteentry(&cache[indexrndq_dequeue(&eviction_queue)] );
  
  steque_destroy(&available_ids);
  indexrndq_destroy(&eviction_queue);
  
  free(cache);

  hshtbl_destroy(&tbl);
}
