#include <iostream>
#include <fstream>
#include <sstream>
#include "workload_generator.hpp"

static const int reps = 5; // No. of times a the access pattern for a caching policy should be run.

/**
 * Loads a list of URLs from a file.
 */
static std::vector<std::string> load_url_list(const std::string filepath) {
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
    for(int i=0; i<reps; i++) {
        for(char a = 'A'; a <= 'Z'; a++) {
            std::stringstream ss;
            std::string s;
            ss << a;
            ss >> s;
            chunk_store.insert(std::pair<std::string, std::vector<std::string> >(s, load_url_list("../workloads/chunks/"+s+".txt")));
            //std::cout << chunk_store[s].size() << std::endl;
        }
    }
}

void Workload::generate_lru_good_workload() {
    lru_good.insert(lru_good.end(), chunk_store["A"].begin(), chunk_store["A"].end());
    lru_good.insert(lru_good.end(), chunk_store["B"].begin(), chunk_store["B"].end());
    lru_good.insert(lru_good.end(), chunk_store["C"].begin(), chunk_store["C"].end());
    
    for(int i=0; i<reps; i++) {
        for(char a = 'A'; a <= 'E'; a++) {
            lru_good.insert(lru_good.end(), chunk_store["D"].begin(), chunk_store["D"].end());
            lru_good.insert(lru_good.end(), chunk_store["E"].begin(), chunk_store["E"].end());
            std::stringstream ss;
            std::string s;
            ss << a;
            ss >> s;
            lru_good.insert(lru_good.end(), chunk_store[s].begin(), chunk_store[s].end());
        }
    }
    
    // std::cout << lru_good.size() << std::endl;
    
    // std::ofstream output("lru_good.txt");
    // for(int i = 0; i < lru_good.size(); i++) {  
    //     output << lru_good[i] << std::endl; 
    // } 
}

void Workload::generate_lru_bad_workload() {
    for(int i=0; i<reps; i++) {
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
    }
    
    // std::ofstream output("lru_bad.txt");
    // for(int i = 0; i < lru_bad.size(); i++) {  
    //     output << lru_bad[i] << std::endl; 
    // } 
}

/**
 * Repeating chunks D and E between sequences of three chunks that were randomly generated.
 */
void Workload::generate_gds_good_workload() {
<<<<<<< HEAD
    std::string sequence = "AFUDSERQZTXVDEZEHDEXDZTAVFBRDEOP";
=======
    std::string sequence = "AEDACBCDAEDCBCDEDCABBCADEDCBACBEDA";
>>>>>>> 00f9cda24860bdea4a89a8a1225bf29c079ff569
    for(int i=0; i<reps; i++) {
        for(auto& ch : sequence) {
            std::stringstream ss;
            std::string s;
            ss << ch;
            ss >> s;
            gds_good.insert(gds_good.end(), chunk_store[s].begin(), chunk_store[s].end());
        }
    }
    
    // std::ofstream output("gds_good.txt");
    // for(int i = 0; i < gds_good.size(); i++) {  
    //     output << gds_good[i] << std::endl; 
    // } 
}
