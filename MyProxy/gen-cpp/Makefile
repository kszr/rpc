GEN_SRC := MyProxy.cpp myproxy_constants.cpp myproxy_types.cpp
GEN_OBJ := $(patsubst %.cpp,%.o, $(GEN_SRC))

THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include

INC := -I$(THRIFT_DIR) -I$(BOOST_DIR)

.PHONY: all clean

all: MyProxy_server MyProxy_client

%.o: %.cpp
	$(CXX) -Wall -std=c++11 -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H $(INC) -c $< -o $@

MyProxy_server: MyProxy_server.o $(GEN_OBJ)
	$(CXX) -std=c++11 $^ -o $@ -L/usr/local/lib -lthrift -lcurl

MyProxy_client: MyProxy_client.o $(GEN_OBJ)
	$(CXX) -std=c++11 $^ -o $@ -L/usr/local/lib -lthrift 

clean:
	$(RM) *.o MyProxy_server MyProxy_client
