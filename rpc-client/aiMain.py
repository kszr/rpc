import subprocess as sp
import time
import os
import random
import string
import sys

def main():
	def inject_code():
		with open('proxy_rpc_svc.c','r') as fd:
			lines = fd.readlines()
			for i in xrange(len(lines)):
				if lines[i].find('main (int argc, char **argv)') != -1:
					break

			if i == len(lines):
				raise Exception()

			content = ''.join(lines[:i-2] + ['\nextern char* gMessage;\n'] + lines[i-2:i+3] + ['\n\tgMessage = argv[1];\n'] + lines[i+3:])

		with open('proxy_rpc_svc.c','w') as fd:
			fd.write(content)		

	def make():
		try:
			p = sp.Popen(['make', '-f','TestMakefile', 'clean', 'rpcfiles'], stdout = sp.PIPE, stderr = sp.STDOUT )			
			(stdoutdata, stderrdata) = p.communicate()
		except:
			sys.stderr.write("Compilation failed.\n")
			sys.stderr.write(stdoutdata)
			raise

		if p.returncode != 0:
			sys.stderr.write("Compilation failed.\n")
			sys.stderr.write(stdoutdata)
			raise Exception()

		inject_code()

		try:
			p = sp.Popen(['make', '-f','TestMakefile', 'client', 'ud_client_test'], stdout = sp.PIPE, stderr = sp.STDOUT )			
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
		def __init__(self, client_args, server_args):
			self.err_msg = "Unknown error in aiMain."

			self.log_server = open('server.udalog','w')
			self.stdout_client = open('client-stdout.udalog','w')
			self.stderr_client = open('client-stderr.udalog','w')

			self.p_server = sp.Popen(server_args, stdout = self.log_server, stderr = sp.STDOUT)

			time.sleep(1)

			self.p_client = sp.Popen(client_args, stdout = self.stdout_client, stderr = self.stderr_client)

			i = 0
			while (self.p_client.poll() is None) and i < 10:
				time.sleep(1.0)
				i = i + 1		

			self.returncode = self.p_client.poll()
			if self.returncode is None:
				self.err_msg = 'Probable deadlock detected.'
			elif self.returncode != 0:
				self.err_msg = 'Bad exit status -%d' % self.returncode
			else:
				self.err_msg = None

			try:
				self.p_client.terminate()
			except:
				pass

			try:
				self.p_server.terminate()
			except:
				pass

			self.log_server.close()
			self.stdout_client.close()
			self.stderr_client.close()

		def get_stderr(self):
			with open('client-stderr.udalog','r') as fd:
				ans = fd.read()
			return ans

		def get_stdout(self):
			with open('client-stdout.udalog','r') as fd:
				ans = fd.read()
			return ans

		def get_errmsg(self):
			return self.err_msg

	def run():
		length = 100
		msg = ''.join(random.choice(string.uppercase) for i in range(length))

		t = TestCase(["./client", "localhost", "example.com"], ["./ud_client_test", msg])

		if t.get_errmsg() is not None:
			sys.stderr.write('Udacity: %s\n' % t.get_errmsg())
			sys.stderr.write('stderr:\n%s' % t.get_stderr())
			sys.stderr.write('stdout:\n%s' % t.get_stdout())
			return

		if t.get_stdout().rstrip() != msg:
			sys.stderr.write('./client localhost example.com' + ' did not print the message from the server.\n')

		return

	#main
	try:
		make()
	except:
		return

	run()
main()