CC     = gcc
CFLAGS = -Wall

GENERICS = hshtbl.c indexminpq.c indexrndq.c steque.c

CACHE_SRC = rnd.c
CACHE_FLAGS = -DRND_REPLACEMENT

######################################################################

all: cache_test01 cache_test02 cache_test03 cache_test04 cache_test05 cache_test06

cache_test01: cache_test01.c $(CACHE_SRC) $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $(CACHE_FLAGS) $^ $(LIBS) 

cache_test02: cache_test02.c $(CACHE_SRC) $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $(CACHE_FLAGS) $^ $(LIBS) 

cache_test03: cache_test03.c $(CACHE_SRC) $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $(CACHE_FLAGS) $^ $(LIBS)

cache_test04: cache_test04.c $(CACHE_SRC) $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $(CACHE_FLAGS) $^ $(LIBS)

cache_test05: cache_test05.c $(CACHE_SRC) $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $(CACHE_FLAGS) $^ $(LIBS)

cache_test06: cache_test06.c $(CACHE_SRC) $(GENERICS) fake_www.c
	$(CC) -o $@ $(CFLAGS) $(CACHE_FLAGS) $^ $(LIBS)

######################################################################
clean:
	rm -f cache_test01 cache_test02 cache_test03 cache_test04 cache_test05 cache_test06 *.o

