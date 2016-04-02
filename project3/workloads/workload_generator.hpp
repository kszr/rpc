#ifndef WORKLOAD_GENERATOR_HPP
#define WORKLOAD_GENERATOR_HPP

#include <vector>
#include <string>
#include <map>

class Workload {
    private:
        std::vector<std::string> lru_good;
        std::vector<std::string> lru_bad;
        std::vector<std::string> gds_good;
        std::map<std::string,std::vector<std::string> > chunk_store;
        
    public:
        Workload();
        void initialize_chunk_store();
        void generate_lru_good_workload();
        void generate_lru_bad_workload();
        void generate_gds_good_workload();
        
        std::vector<std::string> get_lru_good_workload() {
            return lru_good;
        }
        
        std::vector<std::string> get_lru_bad_workload() {
            return lru_bad;
        }
        
        std::vector<std::string> get_gds_good_workload() {
            return gds_good;
        }
};

#endif