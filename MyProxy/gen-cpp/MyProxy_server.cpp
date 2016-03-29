// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.
#include <stdlib.h>
#include "MyProxy.h"
#include <curl/curl.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "../../../../../proxy-server/rpc-server/proxy.cpp"
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::MyProxy;
using namespace std;
/*
extern "C" {
string httpget_1_svc(const string url, struct svc_req* req);
}
*/

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
    printf("ping\n");
  }

  void httpget_1(std::string& _return, const std::string& url) {
    // Your implementation goes here
    
    printf("httpget_1\n");
    _return = httpget_1_svc(url, NULL);
//    _return = &chunk.memory;
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<MyProxyHandler> handler(new MyProxyHandler());
  shared_ptr<TProcessor> processor(new MyProxyProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

