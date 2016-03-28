/*
 * repeatedly calls init and destroy.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "gtcache.h"
#include "fake_www.h"
#include "test_utils.h"


int test_destroy_frees(){
 int i;

  for(i = 0; i < 100; i++){
    gtcache_init(FAKE_WWW_MIN_CACHE_LEN * 8, FAKE_WWW_MIN_CACHE_LEN, 1);
    gtcache_destroy();
  }

  return 1;
}

int main(int argc, char* argv[]){
  if(argc < 2){
    fprintf(stderr,"Usage: cache_test01 [WEB_CHEAT_FILE]\n");
    exit(0);
  }

  if(!test_destroy_frees())
    exit(EXIT_FAILURE);

  return UDACIOUS_EXIT;
}
