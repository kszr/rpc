#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fake_www.h"
#include "gtcache.h"

typedef struct{
  char *str;
  int len;
} sortable_t;

int site_cmp(const void *a, const void *b){
  site_info *x = (site_info*) a;
  site_info *y = (site_info*) b;

  return strncmp(x->url, y->url, FAKE_WWW_MAX_URL_LEN);
}

void fake_www_init(fake_www* www, char* filename, int flags){
  FILE *fp;
  int i;


  if( NULL == (fp = fopen(filename, "r"))){
    fprintf(stderr,"Unable to open file %s.\n", filename);
    exit(EXIT_FAILURE);
  }

  /*Reading the number of urls as the first line in the file*/
  if( fscanf(fp, "%d\n", &www->nsites) != 1 ){
    fprintf(stderr, "Error in reading %s as a webcheat file\n", filename);
    exit(EXIT_FAILURE);
  }
  
  /*Allocating memory*/
  www->sites = (site_info*) malloc(www->nsites * sizeof(site_info));
  for(i = 0; i < www->nsites; i++){
    www->sites[i].url = (char*) malloc(FAKE_WWW_MAX_URL_LEN);
    if( fscanf(fp, "%s , %d\n", www->sites[i].url, &www->sites[i].length) != 2 ){
      fprintf(stderr, "Error in reading %s as a webcheat file\n", filename);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fp);
  
  if( (flags & FAKE_WWW_IGNORE_SIZE) > 0){
    for(i = 0; i < www->nsites; i++)
      www->sites[i].length = FAKE_WWW_MIN_CACHE_LEN-1;
  }

  qsort(www->sites, www->nsites, sizeof(site_info), site_cmp); 

  return;
}

void fake_www_get(fake_www *www, char *url, struct MemoryStruct* chunk){
  int j, k;
  site_info* site;
  site_info tmp;

  tmp.url = url;

  site = bsearch(&tmp, www->sites, www->nsites, sizeof(site_info), site_cmp); 
  
  if(site == NULL){
    fprintf(stderr, "url %s not found in fake_www_get\n", url);
    exit(EXIT_FAILURE);
  }

  chunk->size = site->length;

  chunk->memory = (char*) malloc(chunk->size + 1);
  k = rand();
  for(j = 0; j < chunk->size; j++)
    chunk->memory[j] = (char) ((k++) % 10 + 30);

  /*null-terminating so that it can be interpretted as a string*/
  chunk->memory[chunk->size] = '\0';
  return;
}
