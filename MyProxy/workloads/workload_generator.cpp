#include <iostream>
#include <fstream>
#include <sstream>
#include "workload_generator.hpp"

/**
 * Loads a list of URLs from a file.
 */
static std::vector<std::string> load_url_list(const std::string filepath) {
   //std::cout << filepath <<std::endl;
    std::ifstream file(filepath.c_str());
    std::string str;
    std::vector<std::string> urllist;
    while (std::getline(file, str)) {
        urllist.push_back(str);
        //std::cout << str << std::endl;
    }
    return urllist;
}

Workload::Workload() {
    initialize_chunk_store();
}

void Workload::initialize_chunk_store() {
    for(char a = 'A'; a <= 'Z'; a++) {
        std::stringstream ss;
        std::string s;
        ss << a;
        ss >> s;
        chunk_store.insert(std::pair<std::string, std::vector<std::string> >(s, load_url_list("../workloads/chunks/"+s+".txt")));
        //std::cout << chunk_store[s].size() << std::endl;
    }
}

/**
 * 

vector1.insert( vector1.end(), vector2.begin(), vector2.end() );

*/
void Workload::generate_lru_good_workload() {
    lru_good.insert(lru_good.end(), chunk_store["A"].begin(), chunk_store["A"].end());
    for(char a = 'D'; a <= 'Z'; a++) {
        lru_good.insert(lru_good.end(), chunk_store["B"].begin(), chunk_store["B"].end());
        lru_good.insert(lru_good.end(), chunk_store["C"].begin(), chunk_store["C"].end());
        std::stringstream ss;
        std::string s;
        ss << a;
        ss >> s;
        lru_good.insert(lru_good.end(), chunk_store[s].begin(), chunk_store[s].end());
    }
    // std::cout << lru_good.size() << std::endl;
    
    // std::ofstream output("lru_good.txt");
    // for(int i = 0; i < lru_good.size(); i++) {  
    //     output << lru_good[i] << std::endl; 
    // } 
}

void Workload::generate_lru_bad_workload() {
    for(char a = 'D'; a <= 'Z'; a++) {
        lru_bad.insert(lru_bad.end(), chunk_store["A"].begin(), chunk_store["A"].end());
        lru_bad.insert(lru_bad.end(), chunk_store["B"].begin(), chunk_store["B"].end());
        lru_bad.insert(lru_bad.end(), chunk_store["C"].begin(), chunk_store["C"].end());
        std::stringstream ss;
        std::string s;
        ss << a;
        ss >> s;
        lru_bad.insert(lru_bad.end(), chunk_store[s].begin(), chunk_store[s].end());
    }
    
    // std::ofstream output("lru_bad.txt");
    // for(int i = 0; i < lru_bad.size(); i++) {  
    //     output << lru_bad[i] << std::endl; 
    // } 
}

void Workload::generate_gds_workload() {
    
}