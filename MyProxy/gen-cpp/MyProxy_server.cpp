// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <rpc/rpc.h>
#include <sys/dir.h>
#include <sys/time.h>

#include "MyProxy.h"
#include "../policies/data-structures/hshtbl.cpp"
#include "../policies/data-structures/steque.cpp"
#include "../policies/data-structures/indexminpq.cpp"
#include "../policies/data-structures/indexrndq.cpp"

#include "../policies/lru.cpp"
//#include "../policies/rnd.cpp"
// #include "../policies/gds.cpp"

static const std::size_t capacity = 1024*1024;
static const std::size_t minsize = 1;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::MyProxy;
using namespace std;

struct MemoryStruct {
  char* memory;
  size_t size;
};

int misses = 0; 

// using boost::shared_ptr;


/*
extern "C" {
string httpget_1_svc(const string url, struct svc_req* req);
}
*/

double timeit(struct timeval &start,struct timeval &end) {
    double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    return delta;
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
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
 

void get_by_curl(const string url, struct MemoryStruct* chunk) {
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

    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L); 

    /* get it! */ 
    res = curl_easy_perform(curl_handle);
  
    // cout<<"url in CURL is "<<url<<endl;
 
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
        struct timeval t1,t2;
        gettimeofday(&t1, NULL);
        get_by_curl(url, &chunk);
        gettimeofday(&t2, NULL);
        double timeTaken = timeit(t1,t2);	
        temp = (string)chunk.memory;
        //  	cout<<"Size of web content: "<<temp.size()<<endl;
        std::size_t body_start = temp.find("<body");
        std::size_t body_start_end = temp.find(">", body_start);
        std::size_t body_end = temp.find("</body>");
        temp = temp.substr(body_start_end+1, body_end - body_start_end-1);

        gtcache_set(url, (char *)temp.c_str(), temp.size(), timeTaken);
        // cout<<"Cache miss on url "<<url<<endl;
        misses++;
    } else {
        //	cout<<"Cache hit on url "<<url<<endl;
    }

    // cout << "Url : " << url << " size: " << temp.size() << endl;
    //   cout << temp << endl;
    //cout<<"Size of memory occupied "<<gtcache_memused()<<endl;
    return temp;
}

class MyProxyHandler : virtual public MyProxyIf {
    public:
        MyProxyHandler() {
            // Your initialization goes here
        }

        /**
        * A method definition looks like C code. It has a return type, arguments,
        * and optionally a list of exceptions that it may throw. Note that argument
        * lists and exception lists are specified using the exact same syntax as
        * field lists in struct or exception definitions.
        */
        void ping() {
            // Your implementation goes here
            //printf("Server Pinged\n");
            cout<<"Total Misses= "<<misses<<endl;
        }

        void httpget_1(std::string& _return, const std::string& url) {
            // Your implementation goes here
            //   int currsize;
            //   currsize= gtcache_memused();
            //    cout<<"Size filled: "<<currsize<<endl;
            //    printf("httpget_1\n");
            _return = httpget_1_svc(url, NULL);
            //    std::cout<<"Size of web content: "<<_return.size()<<endl;
            //    currsize= gtcache_memused();
            //    std::cout<<"Size filled: "<<currsize<<endl;
        }

};

int main(int argc, char **argv) {
    int port = 9090;
    ::boost::shared_ptr<MyProxyHandler> handler(new MyProxyHandler());
    ::boost::shared_ptr<TProcessor> processor(new MyProxyProcessor(handler));
    ::boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    ::boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    ::boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);

    gtcache_init (capacity, minsize, 1);
    server.serve();
    gtcache_destroy ();

    return 0;
}

