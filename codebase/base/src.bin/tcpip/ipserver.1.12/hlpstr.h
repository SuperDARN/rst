/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"ipserver - Simple TCP/IP server program.\n",
"ipserver --help\n",
"ipserver [-L logname] [-pf portname] [-if pidname] [-lp port]\n",

"--help\tprint the help message and exit.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-pf portname\trecord the port number that the server is listening for connections on in the file portname. By default, the port is  recorded in port.id.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-lp port\tforce the server to listen on the local port number port. By default, the server requests a port number from the Operating System.\n",

NULL};
