#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include "gtcache.h"
#include "data-structures/hshtbl.h"
#include "data-structures/indexminpq.h"
#include "data-structures/steque.h"

using namespace std;

typedef struct{
  char* key;
  char* value;
  size_t val_size;
  int id;
  struct timeval tv;
} cache_entry_t;

static int min_size;
static int used_mem;
static int cache_capacity;

static cache_entry_t* cache;
static hshtbl_t tbl;
static steque_t available_ids;
static indexminpq_t eviction_queue;

int keycmp(indexminpq_key a, indexminpq_key b){
  struct timeval* tva;
  struct timeval* tvb;

  tva = (struct timeval*) a;
  tvb = (struct timeval*) b;
  
  if (tva->tv_sec > tvb->tv_sec)
    return 1;
  else if (tva->tv_sec < tvb->tv_sec)
    return -1;
  else if (tva->tv_usec > tvb->tv_usec)
    return 1;
  else if (tva->tv_usec < tvb->tv_usec)
    return -1;
  else
    return 0;
}

static void deleteentry(cache_entry_t* e){
  /*
    e is assumed to have already been evicted from the priority queue.
  */
  used_mem -= e->val_size > min_size ? e->val_size : min_size;

  hshtbl_delete(&tbl, e->key);

  free(e->key);
  free(e->value);

  steque_push(&available_ids, (void *) e->id);
}

static cache_entry_t* createentry(char* key, char* value, size_t val_size){
  cache_entry_t* e;
  size_t key_len;

  key_len = strlen(key) + 1;

  used_mem += val_size;

  if(steque_isempty(&available_ids)){
    fprintf(stderr, "ran out of ids for cache entries!\n");
    return NULL;
  }

  e = &cache[(long) steque_pop(&available_ids)];
  e->key = (char*) malloc(key_len);
  e->value = (char*) malloc(val_size);
  e->val_size = val_size;
  memcpy(e->key, key, key_len);
  memcpy(e->value, value, val_size);

  hshtbl_put(&tbl, e->key, e);
  
  gettimeofday(&e->tv, NULL);
  indexminpq_insert(&eviction_queue, e->id, &e->tv);
  
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
  indexminpq_init(&eviction_queue, nmax, keycmp);
    
  for( j = 0; j < nmax; j++){
    cache[j].id = j;
    steque_push(&available_ids, (void *)j);
  }
  
  return 0;
}

char* gtcache_get(const string key, size_t* val_size){
  cache_entry_t* e;
  char* ans;
  char *ch = (char *) key.c_str();

  e = (cache_entry_t *) hshtbl_get(&tbl, ch);

  if(e == NULL)
    return NULL;
  
  /* Mark e as used, if necessary */
  gettimeofday(&e->tv, NULL);
  indexminpq_increasekey(&eviction_queue, e->id, &e->tv);
  
  if(val_size != NULL)
    *val_size = e->val_size;

  ans = (char *) malloc(e->val_size);
  memcpy(ans, e->value, e->val_size);
  
  return ans;
}

int gtcache_set(const string key, char *value, size_t val_size){
  cache_entry_t* e;
  char *ch = (char *) key.c_str();
  
//  cout<<"Cache capacity: "<<cache_capacity<<endl;
//  cout<<"Page size: "<<val_size<<endl;

  if (val_size > cache_capacity){
    /* It's hopeless. */
    fprintf(stderr, "Value exceeds cache limit\n");
    return 1;
  }
  
  e = (cache_entry_t *) hshtbl_get(&tbl, ch);
  
  /* If it is already in the cache...*/
  if( e != NULL){
    if(e->val_size == val_size){
      memcpy(e->value, value, val_size);
      /* Mark e as used, if necessary */
      gettimeofday(&e->tv, NULL);
      indexminpq_increasekey(&eviction_queue, e->id, &e->tv);
      return 0;
    }
    else{
      indexminpq_delete(&eviction_queue, e->id);
      deleteentry(e);
    }
  }
  
  /* Now that we know it isn't already in cache, let's see if there is room */
  while(used_mem + val_size > cache_capacity)
    deleteentry(&cache[indexminpq_delmin(&eviction_queue)]);

  /* Create a new entry for the new element*/
  if( createentry(ch, (char *) value, val_size) == NULL){
    fprintf(stderr, "gtcache_set: unable to create cache_entry\n");
    return -1;
  }

  return 0;
}

int gtcache_memused(){
  return used_mem;
}

void gtcache_destroy(){
  while( !indexminpq_isempty( &eviction_queue ) )
    deleteentry(&cache[indexminpq_delmin(&eviction_queue)] );
  
  steque_destroy(&available_ids);
  indexminpq_destroy(&eviction_queue);
  
  free(cache);

  hshtbl_destroy(&tbl);
}
