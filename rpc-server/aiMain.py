import subprocess as sp
from curlai import HOST_NAME, PORT_NUMBER
import sys
import os
import time
import random
import string

def main():

    def make():
        try:
            p = sp.Popen(['make', '-f','TestMakefile', 'ud_proxy_test'], stdout = sp.PIPE, stderr = sp.STDOUT )          
            (stdoutdata, stderrdata) = p.communicate()
        except:
            sys.stderr.write("Compilation failed.\n")
            sys.stderr.write(stdoutdata)
            raise

        if p.returncode != 0:
            sys.stderr.write("Compilation failed.\n")
            sys.stderr.write(stdoutdata)
            raise Exception()

        try:
            p = sp.Popen(['make', 'proxy'], stdout = sp.PIPE, stderr = sp.STDOUT )          
            (stdoutdata, stderrdata) = p.communicate()
        except:
            sys.stderr.write("Compilation failed.\n")
            sys.stderr.write(stdoutdata)
            raise

        if p.returncode != 0:
            sys.stderr.write("Compilation failed.\n")
            sys.stderr.write(stdoutdata)
            raise Exception()

    class Testcase():
        def __init__(self, secret):
            self.log_server = open('server.udalog','w')
            self.log_client = open('client.udalog','w')

            self.p_server = sp.Popen(['python', 'curlai.py', secret], stdout = self.log_server, stderr = sp.STDOUT)
            time.sleep(1)

            url = "%s:%d" % (HOST_NAME, PORT_NUMBER)
            cmd = "./ud_proxy_test"

            self.p_client = sp.Popen([cmd, url], stdout = self.log_client, stderr = sp.STDOUT)
    
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
            self.log_client.close()

        def get_output(self):
            with open('client.udalog','r') as fd:
                ans = fd.read()
            return ans

        def get_errmsg(self):
            return self.err_msg

    def run():
        length = 100
        secret = ''.join(random.choice(string.uppercase) for i in range(length))

        t = Testcase(secret)

        if t.get_errmsg() is not None:
            sys.stderr.write("Udacity: %s\n" % t.get_errmsg())
            sys.stderr.write(t.get_output())
            return
        if t.get_output().rstrip() != secret:
            sys.stderr.write("Udacity: Your code did not properly retrieve the server's content.\n")
            sys.stderr.write(t.get_output())
            return

        print "SUCCESS"


    try:
        make()
    except:
        return

    run()

main()
