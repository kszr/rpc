#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "gtcache.h"
#include "hshtbl.h"
#include "indexminpq.h"
#include "steque.h"

typedef struct{
  char* key;
  void* value;
  size_t val_size;
  int level;
  int id;
  struct timeval tv;
} cache_entry_t;


static int min_size;
static int nlevels;
static int used_mem;
static int cache_capacity;

static cache_entry_t* cache;
static hshtbl_t tbl;
static steque_t available_ids;
static indexminpq_t *eviction_queue;

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

static int calclevel(size_t val_size){
  int level;
  size_t entry_size;
  
  if(val_size > cache_capacity || val_size > (min_size << nlevels))
    return -1;

  level = 0;
  entry_size = 2 * min_size;
  while(val_size > entry_size){
    entry_size *=2;
    level++;
  }

  return level;
}

static void deleteentry(cache_entry_t* e){
  /*
    e is assumed to have already been evicted from 
    his priority queue.
  */
  used_mem -= e->val_size;

  hshtbl_delete(&tbl, e->key);

  free(e->key);
  free(e->value);

  steque_push(&available_ids, e->id);
}

static cache_entry_t* createentry(char* key, void* value, size_t val_size){
  cache_entry_t* e;
  size_t key_len;
  int level;

  if(steque_isempty(&available_ids)){
    fprintf(stderr, "ran out of ids for cache entries!\n");
    return NULL;
  }

  level = calclevel(val_size);
  key_len = strlen(key) + 1;

  used_mem += val_size;

  e = &cache[steque_pop(&available_ids)];
  e->key = (char*) malloc(key_len);
  e->value = (void*) malloc(val_size);
  e->val_size = val_size;
  e->level = level;
  memcpy(e->key, key, key_len);
  memcpy(e->value, value, val_size);

  hshtbl_put(&tbl, e->key, e);
  
  gettimeofday(&e->tv, NULL);
  indexminpq_insert(&eviction_queue[e->level], e->id, &e->tv);
  
  return e;
}
int gtcache_init(size_t capacity, size_t min_entry_size, int num_levels){
  int i, nmax;

  min_size = min_entry_size;
  cache_capacity = capacity;
  used_mem = 0;

  /*Calculating actual number of levels needed */
  nlevels = 0;
  while( min_size << nlevels < capacity && nlevels < num_levels)
    nlevels++;
  
  /*Calculating the maximum number of possible entries */
  nmax = cache_capacity / min_size;

  /*Initializing the hashtable */
  hshtbl_init(&tbl, 2 * nmax);

  /*Initializing the stack of ids */
  steque_init(&available_ids);
  for(i = 0; i < nmax; i++)
    steque_push(&available_ids, i);

  /*Initializing the cache entries */
  cache = (cache_entry_t*) malloc(nmax * sizeof(cache_entry_t));
  for(i = 0; i < nmax; i++)
    cache[i].id = i;

  /*Initializing the eviction queues */
  eviction_queue = (indexminpq_t*) malloc(nlevels * sizeof(indexminpq_t));
  for(i = 0; i < nlevels; i++)
    indexminpq_init(&eviction_queue[i], nmax, keycmp);
  
  return 0;
}

void* gtcache_get(char *key, size_t *val_size){
  cache_entry_t* e;
  void* ans;

  e = hshtbl_get(&tbl, key);

  if(e == NULL)
    return e;
  
  /* Mark e as used, if necessary */
  gettimeofday(&e->tv, NULL);
  indexminpq_increasekey(&eviction_queue[e->level], e->id, &e->tv);
  
  if( val_size != NULL)
    *val_size = e->val_size;

  ans = malloc(e->val_size);
  memcpy(ans, e->value, e->val_size);
  
  return ans;
}

int gtcache_set(char *key, void *value, size_t val_size){
  int i, level, space_needed;
  cache_entry_t* e;
  cache_entry_t* a;
  
  level = calclevel(val_size);

  if(level < 0){
    fprintf(stderr, "Error: val_size argument to gtcache_set is too big!\n");
    return 1;
  }
  
  e = hshtbl_get(&tbl, key);
  
  /* If it is already in the cache...*/
  if( e != NULL){
    if(e->val_size != val_size){
      /* If the size is different, we'll just delete it and start over */
      indexminpq_delete(&eviction_queue[e->level], e->id);
      deleteentry(e);
    }
    else{
      /*If it's the same, we'll just re-copy the data */
      memcpy(e->value, value, val_size);
      e->val_size = val_size;

      /* Mark e as used */
      gettimeofday(&e->tv, NULL);
      indexminpq_increasekey(&eviction_queue[e->level], e->id, &e->tv);
      return 0;
    }
  }

  /* Now that we know that there isn't already in cache, let's see if there is room */
  space_needed = used_mem + val_size - cache_capacity;

  /* See if eviction is necessary*/ 
  if(space_needed > 0){
    /* Looking for a bigger entry first */
    e = NULL;
    level = calclevel(space_needed);
    for(i = level + 1; i < nlevels; i++){
      if(!indexminpq_isempty(&eviction_queue[i])){
	a = &cache[indexminpq_minindex(&eviction_queue[i])];
	if( e == NULL || keycmp(&e->tv, &a->tv) > 0)
	  e = a;
      }
    }
    
    if( e != NULL){
      /*If we found one, delete and done */
      indexminpq_delmin(&eviction_queue[e->level]);
      deleteentry(e);
    }
    else{
      /*Otherwise, look down in the hierarchy*/
      for( i = level; i >= 0; i--){
	while(!indexminpq_isempty(&eviction_queue[i]) && space_needed > 0){
	  e = &cache[indexminpq_minindex(&eviction_queue[i])];
	  indexminpq_delmin(&eviction_queue[e->level]);
	  deleteentry(e);
	  
	  space_needed = used_mem + val_size - cache_capacity;
	}
      }
    }
  }	  

  /* Create a new entry for the new element*/
  if (createentry(key, value, val_size) == NULL){
    fprintf(stderr, "gtcache_set: unable to create cache entry!\n");
    return 1;
  }

  return 0;
}

int gtcache_memused(){
  return used_mem;
}

void gtcache_destroy(){
  int i = 0;

  for(i = 0; i < nlevels; i++){
    while( !indexminpq_isempty( &eviction_queue[i] ) )
      deleteentry(&cache[indexminpq_delmin(&eviction_queue[i])] );
    indexminpq_destroy(&eviction_queue[i]);
  }

  steque_destroy(&available_ids);
  hshtbl_destroy(&tbl);

  free(eviction_queue);
  free(cache);
}
