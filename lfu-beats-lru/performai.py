import subprocess as sp
import sys
import os
import time


def make(policy1, policy2):
	try:
		p = sp.Popen(['make', policy1, policy2], stdout = sp.PIPE, stderr = sp.STDOUT )			
		(stdoutdata, stderrdata) = p.communicate()
	except:
		sys.stderr.write("Compilation failed.\n")
		sys.stderr.write(stdoutdata)
		raise

	if p.returncode != 0:
		sys.stderr.write("Compilation failed.\n")
		sys.stderr.write(stdoutdata)
		raise Exception()

class TestCase():
	def __init__(self, policy1, policy2, datafile):
		try:
			with open(datafile) as fp:
				o1 = sp.check_output(['./' + policy1, 'webdata_small.txt', '2000'], stdin = fp)
			with open(datafile) as fp:
				o2 = sp.check_output(['./' + policy2, 'webdata_small.txt', '2000'], stdin = fp) 
			
		except sp.CalledProcessError, e:
			sys.stderr.write('Experiment failed.\n')
			raise

		cache_sizes = []
		hits1 = []
		for line in o1.splitlines():
			if line[0] != '#':
				(c, h) = line.split('\t')
				cache_sizes.append(int(c))
				hits1.append(float(h))

		hits2 = []
		for line in o2.splitlines():
			if line[0] != '#':
				(c, h) = line.split('\t')
				hits2.append(float(h))

		assert(len(cache_sizes) == len(hits1) and len(hits1) == len(hits2))

		self._score1 = 0
		self._score2 = 0
		for i in range(len(hits1)):
			if hits1[i] > hits2[i]:
				self._score1 = self._score1 + 1
			elif hits1[i] < hits2[i]:
				self._score2 = self._score2 + 1

	def score1(self):
		return self._score1

	def score2(self):
		return self._score2
