/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"rtmultiplex - TCP/IP stream mulitplexer.\n",
"rtmultiplex --help\n",
"rtmultiplex [-vb] [-t timeout] [-L logname]  [-T timename] [-pf portname] [-if pidname] [-lp lport] [-f] hostnames... rports...\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-t timeout\treset the connection if no packets are received in timeout seconds. By default, the timeout is 20 seconds.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-T timename\tlog the time that the last packet was received in the file timename. By default, the time is recorded in time.rt.\n",
"-pf portname\trecord the port number that the server is listening for connections on in the file portname. By default, the port is  recorded in port.id.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-lp lport\tforce the server to listen on the local port number port. By default, the server requests a port number from the Operating System.\n",
"-f\tallow stream filtering. Each data stream is assigned a numeric identifier.\n",
"hosts\tspace separated hostnames or IP numbers of the systems to connect to.\n",
"rport\tspace separated list of port numbers to connect to on the servers.\n",

NULL};
