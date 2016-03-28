from performai import *
import sys

if not make('lru_perform') or not make('lfu_perform'):
	sys.stderr.write('Your code did not compile. Please check your code for typos or syntax errors \n')
else:
	(cache_sizes, hits1, hits2) = experiment('lru_perform', 'lfu_perform','lru_beats_lfu.txt')
	sys.stdout.write('#Cache Size\tLRU Hit Rate\tLFU Hit Rate\n' )
	for i in range(len(cache_sizes)):
		sys.stdout.write('%d\t%f\t%f\n' % (cache_sizes[i], hits1[i], hits2[i]))
