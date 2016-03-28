/*
 * tests that gtcache_get returns the correct content.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "gtcache.h"
#include "fake_www.h"
#include "test_utils.h"

int test_content_matches(fake_www *www){
  struct MemoryStruct chunk;
  char* data;
  int i;
  size_t sz;

  gtcache_init(FAKE_WWW_MIN_CACHE_LEN, 1, FAKE_WWW_MIN_CACHE_LEN * 8);

  for(i = 0; i < www->nsites; i++){
    fake_www_get(www, www->sites[i].url, &chunk);
    
    gtcache_set(www->sites[i].url, chunk.memory, chunk.size + 1);
    
    data = gtcache_get(www->sites[i].url, &sz);
    
    if(data == NULL || sz != chunk.size + 1 || memcmp(chunk.memory, data, sz) != 0){
      free(data);
      return 0;
    }

    free(chunk.memory);
    free(data);
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

  fake_www_init(&www, argv[1], FAKE_WWW_IGNORE_SIZE);
 
  if(!test_content_matches(&www)){
    fprintf(stderr, "gtcache_get did not return the correct content.");
    exit(EXIT_FAILURE);
  }

  return UDACIOUS_EXIT;
}
