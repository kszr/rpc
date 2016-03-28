CC     = gcc
CFLAGS = -Wall

RPCGEN   = rpcgen
RPCGEN_FLAGS = -C -K 0

RPC_FILES = proxy_rpc_svc.c proxy_rpc.h proxy_rpc_clnt.c

#### RPC Client Part ####
$(RPC_FILES): proxy_rpc.x
	$(RPCGEN) $(RPCGEN_FLAGS) $<

client: client.c proxy_rpc_clnt.c proxy_rpc.h
	$(CC) -o $@ $(CFLAGS) client.c proxy_rpc_clnt.c $(LIBS)

client_test: client_test.c proxy_rpc_svc.c proxy_rpc.h
	$(CC) -o $@ $(CFLAGS) client_test.c proxy_rpc_svc.c $(LIBS)

#### Cleanup ####
clean:
	rm -f $(RPC_FILES) *.o client client_test

