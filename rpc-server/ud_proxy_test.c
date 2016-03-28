#include <stdio.h>
#include "proxy.c"


int main(int argc, char**argv){
	char* data;
	char* url;

	if(argc < 2){
		fprintf(stderr,"usage: proxy_test url");
		exit(EXIT_FAILURE);
	}

	url = argv[1];

	data = *httpget_1_svc(&url, NULL);

	puts(data);

	return 0;
}
