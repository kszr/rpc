CC     = gcc
CFLAGS = -Wall -I./
LIBS = -lm

GENERICS = hshtbl.c indexrndq.c steque.c indexminpq.c

all: rnd_perform lfu_perform lru_perform lrumin_perform

#### Cache Policy Experiments ####
rnd_perform: cache_perform.c rnd.c $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

lfu_perform: cache_perform.c lfu.c $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

lru_perform: cache_perform.c lru.c $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

lrumin_perform: cache_perform.c lrumin.c $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

#### Cleanup ####
clean:
	rm -f *.o rnd_perform lfu_perform lru_perform lrumin_perform

