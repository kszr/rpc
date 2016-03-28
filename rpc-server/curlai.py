import BaseHTTPServer
import sys

HOST_NAME = '127.0.0.1'
PORT_NUMBER = 8080
SECRET = "To be set later"

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
	def do_HEAD(s):
		s.send_response(200)
		s.send_header("Content-type", "text/html")
		s.end_headers()

	def do_GET(s):
		"""Respond to a GET request."""
		s.send_response(200)
		s.send_header("Content-type", "text/html")
		s.end_headers()
		#Anagram for Udacity Rules
		s.wfile.write(SECRET)

if __name__ == '__main__':
	SECRET = sys.argv[1]

	server_class = BaseHTTPServer.HTTPServer
	httpd = server_class((HOST_NAME, PORT_NUMBER), MyHandler)

	print "serving on port %d . . ." % PORT_NUMBER
	try:
		httpd.handle_request()
	except KeyboardInterrupt:
		pass
	httpd.server_close()
