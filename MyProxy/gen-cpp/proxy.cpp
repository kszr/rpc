#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <rpc/rpc.h>
#include <sys/dir.h>

//#include "../policies/lru.cpp"
//#include "../policies/rnd.cpp"
#include "../policies/gds.cpp"

using namespace std;

struct MemoryStruct {
  char* memory;
  size_t size;
};
 
int misses = 0;

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = (char*) realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  
//  cout<<"Size of webpage"<<mem->size<<endl;
 
  return realsize;
}
 

void get_by_curl(const string url, struct MemoryStruct* chunk){
  CURL *curl_handle;
  CURLcode res;
 
  chunk->memory = (char*) realloc(chunk->memory, 1);  /* will be grown as needed by the realloc above */ 
  chunk->size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* specify URL to get */ 
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
 
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
 
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
  /* get it! */ 
  res = curl_easy_perform(curl_handle);
  
//  cout<<"url in CURL is "<<url<<endl;
 
  /* check for errors */ 
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
 
  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();

  return;
}

string httpget_1_svc(const string url, struct svc_req* req){
  static struct MemoryStruct chunk = {NULL, 0};
  
  std::string temp;
  
  if ((chunk.memory=gtcache_get(url, &chunk.size)) == NULL) {
  	get_by_curl(url, &chunk);
  	
  	temp = (string)chunk.memory;
//  	cout<<"Size of web content: "<<temp.size()<<endl;
  	std::size_t body_start = temp.find("<body");
    std::size_t body_start_end = temp.find(">", body_start);
    std::size_t body_end = temp.find("</body>");
  	temp = temp.substr(body_start_end+1, body_end - body_start_end-1);
  	
  	gtcache_set(url, (char *)temp.c_str(), temp.size());
  	cout<<"Cache miss on url "<<url<<endl;
  	misses++;
  } else {
  	cout<<"Cache hit on url "<<url<<endl;
  }
  
  //cout<<"Size of memory occupied "<<gtcache_memused()<<endl;
  return temp;
}

