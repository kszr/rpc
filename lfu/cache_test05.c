/*
 * tests that replacement occurs as expected.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assert.h"
#include "gtcache.h"
#include "fake_www.h"
#include "test_utils.h"

int test_lru_replaces(fake_www *www){
  int i, j, N, C, cache_size;
  struct MemoryStruct chunk;
  char *data;
  size_t sz;

  N = 8;
  C = N/2;
  cache_size = FAKE_WWW_MIN_CACHE_LEN * C;
  if( N > www->nsites){
    fprintf(stderr, "test_lru_replaces: www doesn't have enough urls\n");
    exit(1);
  }
  
  gtcache_init(cache_size, FAKE_WWW_MIN_CACHE_LEN, 1);
  /* Testing that it doesn't over fill*/
  for(j = 0; j < N; j++){
    fake_www_get(www, www->sites[j].url, &chunk);
    gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);
    free(chunk.memory);
    
    if( gtcache_memused() > cache_size ){
      fprintf(stderr, "Capacity exceeded\n");
      return 0;
    }
  }
  gtcache_destroy();


  /* Testing that most recent elements of a cycle are in the cache */
  for(i = 0; i < N; i++){

    gtcache_init(cache_size, FAKE_WWW_MIN_CACHE_LEN, 1);

    //Warming up the cache
    for(j = 0; j < N; j++){
      fake_www_get(www, www->sites[j].url, &chunk);
      gtcache_set(www->sites[j].url, chunk.memory, chunk.size+1);
      free(chunk.memory);
    }

    //See if ith request is still there
    data = gtcache_get(www->sites[i].url, &sz);
    
    if( (data == NULL && i >= C) || (data != NULL && i < C) ){
      if( data != NULL) free(data);
      return 0;
    }
    if( data != NULL) free(data);
      
    gtcache_destroy();
  }

  /* Testing that cache hits change place in eviction queue */

  for(i = 0; i < N; i++){
    gtcache_init(cache_size, FAKE_WWW_MIN_CACHE_LEN, 1);
    
    /* Warming up the cache */
    for(j = 0; j < N; j++){
      fake_www_get(www, www->sites[j].url, &chunk);
      gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);
      free(chunk.memory);
    }
  
    /* Hitting the 3rd quartile */
    for(j = C; j < C + C/2; j++){
      fake_www_get(www, www->sites[j].url, &chunk);
      gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);
      free(chunk.memory);
    }

    /* Missing and setting the 1st quartile */
    for(j = 0; j < C/2; j++){
      fake_www_get(www, www->sites[j].url, &chunk);
      gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);	
      free(chunk.memory);
    }

    //See if ith request is still there
    data = gtcache_get(www->sites[i].url, &sz);
          
    if( (data == NULL && i % C < C/2) || (data != NULL && i % C >= C)){
      if( data != NULL) free(data);
      return 0;
    }
    if( data != NULL) free(data);
      
    gtcache_destroy();
  }

  return 1;
}

int test_lfu_replaces(fake_www *www){
  int i, j, N, C, cache_size;
  struct MemoryStruct chunk;
  char *data;
  size_t sz;

  N = 8;
  C = N/2;
  cache_size = FAKE_WWW_MIN_CACHE_LEN * C;
  if( N > www->nsites){
    fprintf(stderr, "test_lfu_replaces: www doesn't have enough urls\n");
    exit(1);
  }
  
  for(i = 0; i < N; i++){

    gtcache_init(cache_size, FAKE_WWW_MIN_CACHE_LEN, 1);

    //Warming up the cache
    for(j = 0; j < N; j++){
      fake_www_get(www, www->sites[j].url, &chunk);
      gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);
      free(chunk.memory);

      if( gtcache_memused() > cache_size ){
	fprintf(stderr, "Capacity exceeded\n");
	return 0;
      }

      if( j % 2 == 1){
	data = gtcache_get(www->sites[j].url, &sz);
	if( data != NULL) free(data);
      }
    }

    //See if ith request is still there
    data = gtcache_get(www->sites[i].url, &sz);

    if( (data == NULL && i % 2 == 1) || (data != NULL && i % 2 == 0)){
      if( data != NULL) free(data);
      return 0;
    }
      
    gtcache_destroy();
  }

  return 1;
}

int test_rnd_replaces(fake_www *www){
  /*
    
    We repeat an experiment in where there is a 1/4 chance that the first
    element added will still be in the cache, assuming that eviction is done
    uniformly at random.

    Test fails if chernoff bound say probability is small.

    Pr[|X - M/4| > \epsilon M/4] < 2 exp(-\epsilon * M/4 / 3) < 10^-6
    Thus, 
    log(2 * 10^6) < \epsilon M / 12
    12 * log(2 * 10^6) / \epsilon < M

    \epsilon had better be around 1/6 to distingish 1/3 from 1/4.
    Thus, M ~ 1500 should suffice
  */
  int i, j, count, M, N, C, cache_size;
  size_t sz;
  struct MemoryStruct chunk;
  char* data;

  M = 1500;
  N = 4;
  C = 2;
  cache_size = FAKE_WWW_MIN_CACHE_LEN * C;
  count = 0;
  for(i = 0; i < M; i++){
    gtcache_init(cache_size, FAKE_WWW_MIN_CACHE_LEN, 1);

    //Warming up the cache
    for(j = 0; j < N; j++){
      fake_www_get(www, www->sites[j].url, &chunk);
      gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);
      free(chunk.memory);
    }

    //See if 0th request is still there
    data = gtcache_get(www->sites[0].url, &sz);

    if (data != NULL){
      free(data);
      count++;
    }
      
    gtcache_destroy();
  }
  
  if( fabs(count - M / 4.0) > M / 24.0 )
    return 0;

  return 1;
}


int test_lrumin_replaces(fake_www *www){
  int i, j, k,  N, C, nlevels, cache_size;
  struct MemoryStruct chunk;
  char *data;
  size_t sz;

  /*Checking that each individual level works*/
  nlevels = 3;

  for(k = 0; k < nlevels; k++){
    N = 8;
    C = N/2;

    cache_size = C * (FAKE_WWW_MIN_CACHE_LEN << k);

    if( N > www->nsites){
      fprintf(stderr, "test_lru_replaces: www doesn't have enough urls\n");
      exit(1);
    }
    
    for(i = 0; i < www->nsites; i++)
      www->sites[i].length = (FAKE_WWW_MIN_CACHE_LEN << k) - 5;

    for(i = 0; i < N; i++){
      gtcache_init( cache_size, FAKE_WWW_MIN_CACHE_LEN, nlevels);

      //Warming up the cache
      for(j = 0; j < N; j++){
      	fake_www_get(www, www->sites[j].url, &chunk);
      	gtcache_set(www->sites[j].url, chunk.memory, chunk.size + 1);
	free(chunk.memory);

      	if( gtcache_memused() > cache_size ){
      	  fprintf(stderr, "Capacity exceeded\n");
      	  return 0;
      	}
      }

      //See if ith request is still there
      data = gtcache_get(www->sites[i].url, &sz);

      if(data == NULL && i >= C){
      	fprintf(stderr, "Level %d failed.\n", k);
      	return 0;
      }
      if(data != NULL && i < C){
      	fprintf(stderr, "Level %d failed.\n", k);
	free(data);
      	return 0;
      }
      
      if( data != NULL) free(data);
      gtcache_destroy();
    }
  }

  /*Checking that it replaces LRU bigger item*/
  nlevels = 4;

  /*First up if it can*/
  www->sites[0].length = 2 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[1].length = 16 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[2].length = 8 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[3].length = 4 * FAKE_WWW_MIN_CACHE_LEN - 2;  
  www->sites[4].length = www->sites[0].length;
  www->sites[5].length = www->sites[1].length;
  www->sites[6].length = www->sites[2].length;
  www->sites[7].length = www->sites[3].length;

  cache_size = 0;
  for(i = 0; i < 8; i++)
    cache_size += www->sites[i].length + 1;

  gtcache_init(cache_size, FAKE_WWW_MIN_CACHE_LEN, nlevels);

  for(i = 0; i < 8; i++){
    fake_www_get(www, www->sites[i].url, &chunk);
    gtcache_set(www->sites[i].url, chunk.memory, chunk.size + 1);
    free(chunk.memory);
  }

  www->sites[8].length = www->sites[2].length;
  fake_www_get(www, www->sites[8].url, &chunk);
  gtcache_set(www->sites[8].url, chunk.memory, chunk.size + 1);
  free(chunk.memory);

  /*1 should be evicted */
  for(i = 0; i < 8; i++){
    data = gtcache_get(www->sites[i].url, &sz);
    if( (i != 1 && data == NULL) || (i==1 && data != NULL) ){
      if (data != NULL) free(data);
      fprintf(stderr, "Least recently used item from > levels should be replaced.\n");
      return 0;
    }
    if (data != NULL) free(data);
  }

  gtcache_destroy();

  /*Second Test*/
  www->sites[0].length = 2 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[1].length = 8 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[2].length = 4 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[3].length = 16 * FAKE_WWW_MIN_CACHE_LEN - 2;  
  www->sites[4].length = www->sites[0].length;
  www->sites[5].length = www->sites[1].length;
  www->sites[6].length = www->sites[2].length;
  www->sites[7].length = www->sites[3].length;

  cache_size = 0;
  for(i = 0; i < 8; i++)
    cache_size += www->sites[i].length + 1;

  gtcache_init(cache_size, FAKE_WWW_MIN_CACHE_LEN, nlevels);

  for(i = 0; i < 8; i++){
    fake_www_get(www, www->sites[i].url, &chunk);
    gtcache_set(www->sites[i].url, chunk.memory, chunk.size + 1);
    free(chunk.memory);
  }

  www->sites[8].length = www->sites[1].length;
  fake_www_get(www, www->sites[8].url, &chunk);
  gtcache_set(www->sites[8].url, chunk.memory, chunk.size+1);
  free(chunk.memory);

  /* 3 should be evicted*/
  for(i = 0; i < 8; i++){
    data = gtcache_get(www->sites[i].url, &sz);
    if(  (3 != i && data == NULL)  || (3==i && data != NULL) ){
    if (data != NULL) free(data);
      fprintf(stderr, "Least recently used item from > levels should be replaced.\n");
      return 0;
    }
    if (data != NULL) free(data);
  }

  gtcache_destroy();

/*Third Test*/
  www->sites[0].length = 4 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[1].length = 8 * FAKE_WWW_MIN_CACHE_LEN - 2;
  www->sites[2].length = www->sites[0].length;
  www->sites[3].length = www->sites[1].length;
  www->sites[4].length = www->sites[0].length;
  www->sites[5].length = www->sites[1].length;
  www->sites[6].length = www->sites[0].length;
  www->sites[7].length = www->sites[1].length;

  cache_size = 0;
  for(i = 0; i < 8; i++)
    cache_size += www->sites[i].length + 1;

  gtcache_init( cache_size, FAKE_WWW_MIN_CACHE_LEN, nlevels);

  for(i = 0; i < 8; i++){
    fake_www_get(www, www->sites[i].url, &chunk);
    gtcache_set(www->sites[i].url, chunk.memory, chunk.size + 1);
    free(chunk.memory);
  }

  www->sites[8].length = www->sites[0].length/2 + www->sites[1].length;
  fake_www_get(www, www->sites[8].url, &chunk);
  gtcache_set(www->sites[8].url, chunk.memory, chunk.size + 1);
  free(chunk.memory);

  /* 0 and 1 should be evicted*/
  for(i = 0; i < 8; i++){
    data = gtcache_get(www->sites[i].url, &sz);
    if( (i > 1 && data == NULL) || (i <= 1 && data != NULL) ){
      if (data != NULL) free(data);
      fprintf(stderr, "When there is no item larger than the space needed, remove from the largest non-empty level and recurse with new value of space needed.\n");
      return 0;
    }
    if (data != NULL) free(data);
  }

  gtcache_destroy();

  return 1;
}

int main(int argc, char* argv[]){
  fake_www www;
  int res;

  if(argc < 2){
    fprintf(stderr,"Usage: cache_test [WEB_CHEAT_FILE]\n");
    exit(0);
  }

  srand(12314);

  fake_www_init(&www, argv[1], FAKE_WWW_IGNORE_SIZE);

  res = 0;
  
#if defined LRU_REPLACEMENT
  res = test_lru_replaces(&www);
#elif defined LFU_REPLACEMENT
  res = test_lfu_replaces(&www);
#elif defined RND_REPLACEMENT
  res = test_rnd_replaces(&www);
#elif defined RNDMIN_REPLACEMENT
  res = test_rndmin_replaces(&www);
#elif defined LRUMIN_REPLACEMENT
  res = test_lrumin_replaces(&www);
#endif
   
  if(!res){
    fprintf(stderr,"An unexpected replacement seems to have taken place." );
    exit(EXIT_FAILURE);
  } 

  return UDACIOUS_EXIT;
}
