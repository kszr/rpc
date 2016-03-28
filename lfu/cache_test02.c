/*
 * tests that destroying the cache erases it.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "gtcache.h"
#include "fake_www.h"
#include "test_utils.h"

int test_destroy_erases(fake_www *www){
  int i;
  size_t sz;
  struct MemoryStruct chunk;
  char* data;

  fake_www_get(www, www->sites[i].url, &chunk);

  /*Works once */
  gtcache_init(FAKE_WWW_MIN_CACHE_LEN * 8, FAKE_WWW_MIN_CACHE_LEN, 1);

  gtcache_set(www->sites[0].url, chunk.memory, chunk.size + 1);
  
  gtcache_destroy();

  free(chunk.memory);

  /*Should be empty now*/
  gtcache_init(FAKE_WWW_MIN_CACHE_LEN, 1, FAKE_WWW_MIN_CACHE_LEN);

  data = gtcache_get(www->sites[0].url, &sz);
  if( data == NULL)
    return 1;
  else{
    free(data);
    return 0;
  }
}

int main(int argc, char* argv[]){
  fake_www www;

  if(argc < 2){
    fprintf(stderr,"Usage: cache_test02 [WEB_CHEAT_FILE]\n");
    exit(0);
  }

  srand(12314);

  fake_www_init(&www, argv[1], FAKE_WWW_IGNORE_SIZE);

  if(!test_destroy_erases(&www)){
    fprintf(stderr, "Destroying the cache does not seem to empty its contents.\n");
    exit(EXIT_FAILURE);
  }
    
  return UDACIOUS_EXIT;
}
