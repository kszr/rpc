from performai import *
import sys

if not make('rnd_perform') or not make('lru_perform'):
	sys.stderr.write('Your code did not compile. Please check your code for typos or syntax errors \n')
else:
	(cache_sizes, hits1, hits2) = experiment('rnd_perform', 'lru_perform', 'rnd_beats_lru.txt')
	sys.stdout.write('#Cache Size\tRandom Hit Rate\tLRU Hit Rate\n' )
	for i in range(len(cache_sizes)):
		sys.stdout.write('%d\t%f\t%f\n' % (cache_sizes[i], hits1[i], hits2[i]))
