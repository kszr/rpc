CC     = gcc
CFLAGS = -Wall

RPCGEN   = rpcgen
RPCGEN_FLAGS = -C -K 0

CURL_LIBS = $(shell curl-config --libs)
CURL_CFLAGS = $(shell curl-config --cflags)

RPC_FILES = proxy_rpc_svc.c proxy_rpc.h proxy_rpc_clnt.c

#### RPC Client Part ####
$(RPC_FILES): proxy_rpc.x
	$(RPCGEN) $(RPCGEN_FLAGS) $<

#### RPC Server & Libcurl ####
proxy: proxy.c proxy_rpc_svc.c proxy_rpc.h
	$(CC) -o $@ $(CFLAGS) $(CURL_CFLAGS) proxy.c proxy_rpc_svc.c $(CURL_LIBS)

#### Cleanup ####
clean:
	rm -f $(RPC_FILES) *.o client client_test proxy

