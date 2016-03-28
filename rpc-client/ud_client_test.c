#include "proxy_rpc.h"

char *gMessage;
static char* ans;


char ** httpget_1_svc(char** url, struct svc_req* req){
  //Memory concerns

	ans = strdup(gMessage);

  return &ans;
}
