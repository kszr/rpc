from subprocess import *
import sys
import os
import time

def make():
	try:
		res = call(['make', 'client', 'client_test'])
	except CalledProcessError, e:
		sys.stderr.write(e.output + '\n')
		return False
	return res == 0

def executeBinary():
	cmd_server = "./client_test"
	cmd_client = "./client"

	p_server = Popen([cmd_server])
	sys.stdout.write(cmd_server+'\n')

	#Giving him time to set up....
	time.sleep(1)

	p_client = Popen([cmd_client, "localhost", "foobar.com"])
	sys.stdout.write(cmd_client+'\n')

	time.sleep(.1)
	p_client.poll()
	if p_client.returncode is None:
	 	time.sleep(2)
	 	#If it takes this long, there is probably deadlock.
	 	p_client.poll()
	 	if p_client.returncode is None:
	 		sys.stderr.write('Probable deadlock detected.' + '\n')

	if p_client.returncode != 0:
	 	sys.stderr.write(cmd_client + ' : Bad exit status %d \n' % p_client.returncode)
	os.system("killall client_test client")
	
if make():
	executeBinary()
else:
	sys.stderr.write('Your code did not compile. Please check your code for typos or syntax errors \n')
