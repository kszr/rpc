import subprocess as sp
import sys
import os
import time

UDACIOUS_EXIT = 59

def main(make_args):

	def make(make_args):
		try:
			p = sp.Popen(['make', 'all'] + make_args, stdout = sp.PIPE, stderr = sp.STDOUT )			
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
		def __init__(self, args, description):
			self.description = description

			self.log = open('log.udalog','w')

			self.p = sp.Popen(args, stdout = self.log, stderr = sp.STDOUT)

			self.p.wait()

			self.cleanup()

			self._passed = (self.p.returncode == UDACIOUS_EXIT)

		def cleanup(self):
			self.log.close()

			with open('log.udalog','r') as fd:
				self.output = fd.read()

		def passed(self):
			return self._passed

		def comment(self):
			if self.passed():
				return None

			ans = ['An error was detected in a test that %s' % self.description]
			ans.append('Exit code: -%d.' % self.p.returncode)
			ans.append(self.output())
			return '\n'.join(ans)

	def run():
		tests = [TestCase(['./cache_test01', 'webdata.txt'], 'repeatedly calls init and destroy.'),
				TestCase(['./cache_test02', 'webdata.txt'], 'tests that destroying the cache erases it.'),
				TestCase(['./cache_test03', 'webdata.txt'], 'tests that memused returns a correct value.'),
				TestCase(['./cache_test04', 'webdata.txt'], 'tests that gtcache_get returns the correct content.'),
				TestCase(['./cache_test05', 'webdata.txt'], 'tests that replacement occurs as expected.'),
				TestCase(['./cache_test06', 'webdata.txt'], 'tests that gtcache evicts many small old entries to make space of a larger new one.')]
	
		err_msg = []
		for t in tests:
			if not t.passed():
				err_msg.append('*** Error Detected ***\n%s' % t.comment())
		if len(err_msg) == 0:
			sys.stdout.write('Correct!\n')
		else:
			sys.stderr.write('\n'.join(err_msg))			

	try:
		make(make_args)
	except:
		return

	run()



