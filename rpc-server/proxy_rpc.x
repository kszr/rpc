/*
 * Specify an interface with one procedure named httpget_1 that
 * takes takes a string (the url) as an input and returns a string.
 *
 * Your RPCL code here.
*/


program PROXYRPCPROG {
	version PROXYRPCVERS {
		string HTTPGET(string) = 1;
	} = 1;
} = 99;