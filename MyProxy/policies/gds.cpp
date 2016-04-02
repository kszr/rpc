/**
 * An implementation of the Greedy Dual-Size algorithm. From the assignment
 * description: "Removes the item with the lowest value of H. Each item starts
 * off with a value of H = (cost of bringing the item to cache / size of the item).
 * When an item is replaced, decrement all of the other items H values by the replaced
 * item's H value. When items are accessed again, restore H to the original
 * H value (cost / size). The cost function is parameterized."
 *
 * The present implementation is a variation of that obtained from here:
 * https://www.usenix.org/legacy/publications/library/proceedings/usits97/full_papers/cao/cao_html/node8.html
 * This is easier to implement, because it does not require iterating over the entire
 * cache and updating the weights (H values) of cache entries every time an eviction
 * occurs. In this implementation:
 * Initialize L to 0.
 *  - If p is already in the cache, set H(p) to L + c(p)/s(p). 
 *  - If p is not in the cache,
 *      - While there is not enough room in the cache for p,
 *          - Let L be the minimum H(q) over all pages q in the cache.
 *          - Evict q such that H(q) = L.
 *  - Bring p into the cache and set H(p) to be L+c(p)/s(p) 
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>
#include "gtcache.h"
#include "data-structures/hshtbl.h"
#include "data-structures/indexminpq.h"
#include "data-structures/steque.h"
#include <limits>

using namespace std;

typedef struct{
  char* key;
  void* value;
  size_t val_size;
  int id;
  double weight;
} cache_entry_t;

static int min_size;
static int used_mem;
static int cache_capacity;
static double least_weight; // The global "L" value - i.e., the current least weight among all cached items.

static cache_entry_t* cache;
static hshtbl_t tbl;
static steque_t available_ids;
static indexminpq_t eviction_queue;

int keycmp(indexminpq_key a, indexminpq_key b){
  cache_entry_t* ea;
  cache_entry_t* eb;
  
  ea = (cache_entry_t*) a;
  eb = (cache_entry_t*) b;

  if (ea->weight > eb->weight)
    return 1;
  else if (ea->weight < eb->weight)
    return -1;
  else
    return 0;
}

static void deleteentry(cache_entry_t* e){
  /*
    e is assumed to have already been evicted from the priority queue.
  */
  used_mem -= e->val_size > min_size ? e->val_size : min_size;
  least_weight = e->weight; // Updating least_weight with the most recently evicted entry's weight.
  hshtbl_delete(&tbl, e->key);

  free(e->key);
  free(e->value);

  steque_push(&available_ids, (void *) e->id);
}

static cache_entry_t* createentry(char* key, void* value, size_t val_size, double timeTaken){
  cache_entry_t* e;
  size_t key_len;

  if(steque_isempty(&available_ids)){
    fprintf(stderr, "ran out of ids for cache entries!\n");
    return NULL;
  }

  key_len = strlen(key) + 1;

  used_mem += val_size;

  e = (cache_entry_t *) &cache[(long) steque_pop(&available_ids)];
  e->key = (char*) malloc(key_len);
  e->value = (void*) malloc(val_size);
  e->val_size = val_size;
  e->weight = least_weight + timeTaken/(val_size*val_size);
  memcpy(e->key, key, key_len);
  memcpy(e->value, value, val_size);

  hshtbl_put(&tbl, e->key, e);
  
  indexminpq_insert(&eviction_queue, e->id, e);
  
  return e;
}

int gtcache_init(size_t capacity, size_t min_entry_size, int num_levels) {
  int j, nmax;
  least_weight = 0.0;

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
    steque_push(&available_ids, (void *) j);
  }
  
  return 0;
}

int gtcache_set(const std::string key, char* value, size_t val_size, double timeTaken) {
  cache_entry_t* e;
  char *ch = (char *) key.c_str();
  
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
      cout<<"UH OH, MAYDAY!!!"<<endl;
      e->weight = val_size > 0 ? 1.0/val_size : std::numeric_limits<double>::infinity();
      indexminpq_increasekey(&eviction_queue, e->id, (indexminpq_key) e);
      return 0;
    }
    else{
      indexminpq_delete(&eviction_queue, (int) e->id);
      deleteentry(e);
    }
  }
  
  /* Now that we know it isn't already in cache, let's see if there is room */
  while(used_mem + val_size > cache_capacity)
    deleteentry(&cache[indexminpq_delmin(&eviction_queue)]);

  if( createentry(ch, value, val_size, timeTaken) == NULL){
    fprintf(stderr, "gtcache_set: unable to create cache_entry\n");
    return -1;
  }

  return 0; 
}

char* gtcache_get(const std::string key, size_t* val_size) {
  if(cache_capacity <= 0)
    return NULL;
    
  cache_entry_t* e;
  char *ans;
  char *ch = (char *) key.c_str();

  e = (cache_entry_t *) hshtbl_get(&tbl, ch);

  if(e == NULL)
    return NULL;
  
  //cout<<"val_size= "<<e->val_size<<endl;
  e->weight = e->val_size > 0 ? (e->weight + least_weight) : std::numeric_limits<double>::infinity(); // Revert weight to old default.
  indexminpq_increasekey(&eviction_queue, e->id, (indexminpq_key) e);
  
  if( val_size != NULL)
    *val_size = e->val_size;

  ans = (char *) malloc(e->val_size);
  memcpy(ans, e->value, e->val_size);
  
  return ans;    
}

int gtcache_memused() {
    return used_mem;
}

void gtcache_destroy() {
    while( !indexminpq_isempty( &eviction_queue ) )
        deleteentry(&cache[indexminpq_delmin(&eviction_queue)] );
  
    steque_destroy(&available_ids);
    indexminpq_destroy(&eviction_queue);

    free(cache);

    hshtbl_destroy(&tbl);
}
