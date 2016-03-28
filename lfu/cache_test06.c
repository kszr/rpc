/*
 * tests that gtcache evicts many small old entries to make space of a larger new one.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "gtcache.h"
#include "fake_www.h"
#include "test_utils.h"

int test_multiple_evictions(fake_www *www){
  struct MemoryStruct chunk;
  int i,v;
  size_t cache_sz;
  
  cache_sz = 0;
  for(i = 0; i < www->nsites; i++)
    if( cache_sz < www->sites[i].length + 1)
      cache_sz = www->sites[i].length + 1;

  cache_sz = 2 * cache_sz;

  if( cache_sz < FAKE_WWW_MIN_CACHE_LEN ){
    fprintf(stderr, "www lengths are too small for multiple eviction test\n");
    exit(EXIT_FAILURE);
  }

  for(i = 1, v = FAKE_WWW_MIN_CACHE_LEN; v < cache_sz; i++, v*=2);

  gtcache_init(FAKE_WWW_MIN_CACHE_LEN, i+1, cache_sz);

  for(i = 0; i < www->nsites; i++){
    fake_www_get(www, www->sites[i].url, &chunk);
    
    gtcache_set(www->sites[i].url, chunk.memory, chunk.size + 1);
    free(chunk.memory);

    if( gtcache_memused() > cache_sz){
      fprintf(stderr, "The cache size exceed capacity while storing and evicting multiple-sized entries.");
      return 0;    
    }
  }
  
  gtcache_destroy();
  
  return 1;
}

int main(int argc, char* argv[]){
  fake_www www;

  if(argc < 2){
    fprintf(stderr,"Usage: cache_test04 [WEB_CHEAT_FILE]\n");
    exit(0);
  }

  srand(12314);

  fake_www_init(&www, argv[1], 0);
 
  if(!test_multiple_evictions(&www)){
    exit(EXIT_FAILURE);
  }

  return UDACIOUS_EXIT;
}
