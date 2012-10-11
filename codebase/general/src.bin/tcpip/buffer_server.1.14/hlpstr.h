/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"buffer_server - Simple TCP/IP connection relay and buffer.\n",
"buffer_server --help\n",
"buffer_server [-lf logname] [-f portname] [-l lport] hostname rport\n",

"--help\tprint the help message and exit.\n",
"-lf logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-f portname\trecord the port number that the server is listening for connections on in the file portname. By default, the port is  recorded in port.id.\n",
"-l port\tforce the server to listen on the local port number port. By default, the server requests a port number from the Operating System.\n",
"host\thostname or IP number of the system to connect to.\n",
"rport\tport number to connect to on the server.\n",

NULL};
