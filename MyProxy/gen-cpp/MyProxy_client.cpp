#include "MyProxy.h"
#include <vector>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace MyProxy;
using namespace  std;

static void load_file_list(string filepath);

static vector<string> filelist;

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    
    MyProxyClient client(protocol);
    transport->open();
    client.ping();
    string webcontent;
    string url= "www.google.com";
    client.httpget_1(webcontent, url);
    //cout<<"The web page content for URL: "<<url<<" is:" <<webcontent<<endl;
    transport->close();

    return 0;
}

static void load_file_list(string filepath) {
    
}

