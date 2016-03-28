/* 
 * tests that memused returns a correct value.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "gtcache.h"
#include "fake_www.h"
#include "test_utils.h"

int test_reports_memused(fake_www *www){
  int j, before, after;
  struct MemoryStruct chunk;

  gtcache_init(FAKE_WWW_MIN_CACHE_LEN << 10, FAKE_WWW_MIN_CACHE_LEN, 1);
    
  /*
    Assumed that fake www will fit into the cache
  */

  for(j = 0; j < 10; j++){
    fake_www_get(www, www->sites[j].url, &chunk);
    
    before = gtcache_memused();
    gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);
    free(chunk.memory);
    after = gtcache_memused();

    if(after - before != chunk.size + 1)
      return 0;
  } 
    
  gtcache_destroy();

  return 1;
}


int main(int argc, char* argv[]){
  fake_www www;

  if(argc < 2){
    fprintf(stderr,"Usage: cache_test03 [WEB_CHEAT_FILE]\n");
    exit(0);
  }

  srand(12314);

  fake_www_init(&www, argv[1], FAKE_WWW_IGNORE_SIZE);

  if(!test_reports_memused(&www)){
    fprintf(stderr, "gtcache_memused returned an incorrect amount.\n");
    exit(EXIT_FAILURE);
  }

  return UDACIOUS_EXIT;
}
