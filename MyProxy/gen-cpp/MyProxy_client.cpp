#include "MyProxy.h"
#include <vector>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <iostream>
#include <fstream> 

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace MyProxy;
using namespace  std;

static void load_url_list(string filepath);
static void write_csv_file(vector<string> list, int length, string file_name);

static vector<string> urllist;

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    
    MyProxyClient client(protocol);
    transport->open();
    string webcontent;
    //string url= "www.google.com";
    load_url_list("../workloads/rnd_beats_lru.txt");
    
    for(auto& str : urllist) {
        client.httpget_1(webcontent, str);
        //cout << "Web content: "<<webcontent << endl;
        //cout << str << endl;
    }
    //cout<<"The web page content for URL: "<<url<<" is:" <<webcontent<<endl;
    client.ping();
    transport->close();

    return 0;
}

/**
 * Loads a list of URLs from a file.
 */
static void load_url_list(const string filepath) {
    ifstream file(filepath.c_str());
    string str; 
    while (std::getline(file, str))
    {
        urllist.push_back(str);
        //cout << str << endl;
    }
}

/**
 * Function that prints array A to a *.csv file. Each element of the vector list
 * should be a comma-separated string representing a single row.
 */
static void write_csv_file(vector<string> list, int length, string file_name) {  
    ofstream output(file_name);
    for(int i = 0; i < length; i++) {  
        output << list[i] << endl; 
    } 
}

