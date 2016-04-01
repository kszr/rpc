#include "MyProxy.h"
#include <vector>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <iostream>
#include <fstream> 
#include "../workloads/workload_generator.cpp"
#include <sstream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace MyProxy;
using namespace  std;

//static void load_url_list(string filepath);
static void write_csv_file(vector<string> list, int length, string file_name);

static vector<string> urllist;

double timeit(struct timeval &start,struct timeval &end){
    double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    return delta;
}

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    
    MyProxyClient client(protocol);
    transport->open();
    string webcontent;
    //string url= "www.google.com";
    //load_url_list("../workloads/rnd_beats_lru.txt");
    Workload workload = Workload();
    
    /* Specify workload below */
    workload.generate_gds_good_workload();
    urllist = workload.get_gds_good_workload();
    
    vector<string> time_list;
    
    for(auto& str : urllist) {
        struct timeval t1,t2;
        gettimeofday(&t1, NULL);
        client.httpget_1(webcontent, str);
        gettimeofday(&t2, NULL);
        double timeTaken = timeit(t1,t2);	
        
        std::stringstream ss;
        std::string s;
        ss << timeTaken;
        ss >> s;
        
        time_list.push_back(s);
        
        //cout << "Web content: "<<webcontent << endl;
        //cout << str << endl;
    }
    
    //write_csv_file(time_list, time_list.size(), "access_time_gds_lru-bad.csv");
    
    //cout<<"The web page content for URL: "<<url<<" is:" <<webcontent<<endl;
    client.ping();
    transport->close();

    return 0;
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

