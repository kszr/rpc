GEN_SRC := MyProxy.cpp myproxy_constants.cpp myproxy_types.cpp
GEN_OBJ := $(patsubst %.cpp,%.o, $(GEN_SRC))

# POLICY_SRC := ../policies/gds.cpp ../policies/lru.cpp ../policies/rnd.cpp
# POLICY_OBJ := $(patsubst %.cpp,%.o, $(POLICY_SRC))

DAT_STRUCT_SRC := ../policies/data-structures/hshtbl.cpp ../policies/data-structures/indexminpq.cpp ../policies/data-structures/indexrndq.cpp ../policies/data-structures/seqsrchst.cpp ../policies/data-structures/steque.cpp  
DAT_STRUCT_OBJ := $(patsubst %.cpp,%.o, $(DAT_STRUCT_SRC))

WORKLOAD_OBJ := ../workloads/workload_generator.o

THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include

INC := -I$(THRIFT_DIR) -I$(BOOST_DIR)

BOOST_FLAGS := -lboost_system -lboost_filesystem -lboost_date_time -lboost_program_options 

.PHONY: all clean

all: MyProxy_server MyProxy_client

%.o: %.cpp
	$(CXX) -Wall -std=c++11 -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H $(INC) -c $< -o $@

MyProxy_server: MyProxy_server.o $(GEN_OBJ) #$(POLICY_OBJ) $(DAT_STRUCT_OBJ)
	$(CXX) -std=c++11 $^ -o $@ -L/usr/local/lib -lthrift -lcurl $(BOOST_FLAGS)

MyProxy_client: MyProxy_client.o $(GEN_OBJ) $(WORKLOAD_OBJ)
	$(CXX) -std=c++11 $^ -o $@ -L/usr/local/lib -lthrift $(BOOST_FLAGS)

clean:
	$(RM) *.o MyProxy_server MyProxy_client
