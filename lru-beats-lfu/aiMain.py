from performai import make, TestCase
import sys

def main():
	p1 = 'lru_perform'
	p2 = 'lfu_perform'

	try:
		make(p1,p2)
	except:
		return

	t = TestCase(p1, p2, 'lru_beats_lfu.txt')

	if t.score2() >= t.score1():
		sys.stderr.write('LFU replacement did not outperform LRU on your data.\n')
	else:
		print 'Correct!'
main()
