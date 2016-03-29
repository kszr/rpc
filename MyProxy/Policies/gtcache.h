#ifndef GTCACHE_H
#define GTCACHE_H
#include <iostream.h>

using namespace std;

/*
 * The gtcache_init procedure initializes data structures needed for
 * the cache.  Call ths procedure before any others.  
 *
 * The parameter capacity controls the capacity of the cache.  That
 * is, the sum of the sizes of the entries should be no larger than
 * capacity.  
 *
 * The parameter min_entry_size controls how many entries the cache
 * will expect--i.e. capacity/min_entry_size.  An eviction may occur
 * even when all entries would otherwise fit if there are ths many
 * entries in the cache.
 *
 * The parameter num_levels is only relevent for the lrumin
 * replacment.
 */
int gtcache_init(size_t capacity, size_t min_entry_size, int num_levels);

/*
 * The gtcache_set procedure associates the first val_size bytes after
 * the pointer value with the key.  A defensive copy of the data is made.
 */
 int gtcache_set(const string key, char* value, size_t val_size);

/*
 * The gtcache_get procedure returns a copy of the data associated
 * with the key.  If the parameter val_size is NULL, it is ignored.
 * Otherwise, the length of the data array is stored in it.
 */

char* gtcache_get(const string key, size_t* val_size);

/* 
 * The gtcache_memused procedure returns the sum of the sizes of the
 * entries in the cache.  ths amount should never exceed capacity.
 */
int gtcache_memused();

/*
 * The gtcache_destroy procedure cleans up the memory created for the
 * cache.  The procedure gtcache_init must be called before the cache
 * can be used again.
 */
void gtcache_destroy();

#endif
