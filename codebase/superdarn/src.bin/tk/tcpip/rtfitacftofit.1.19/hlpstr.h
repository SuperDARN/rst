/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"rtfitacftofit - Convert a fitacf format TCP/IP data stream to a fit format stream.\n",
"rtfitacftofit --help\n",
"rtfitacftofit [-t timeout] [-s stream] [-L logname] [-T timename] [-pf portname] [-if pidname] [-lp lport] host rport\n",
"rtfitacftofit -rpf [-t timeout] [-s stream] [-L logname] [-T timename] [-pf portname] [-if pidname] [-lp lport] host rportname\n",

"--help\tprint the help message and exit.\n",
"-t timeout\tset the time to wait before resetting the connection to timeout seconds\n",
"-s stream\tfor a multiplexed stream, read the stream number given by stream. A value of 255 will read all streams\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-T timename\tlog the time that the last packet was received in the file timename. By default, the time is recorded in time.rt.\n",
"-pf portname\trecord the port number that the server is listening for connections on in the file portname. By default, the port is  recorded in port.id.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-lp lport\tforce the server to listen on the local port number port. By default, the server requests a port number from the Operating System.\n",
"host\thostname or IP number of the system to connect to.\n",
"rport\tport number to connect to on the server.\n",
"-rpf\tThe remote port number is stored in a text file.\n",
"rportname\tfilename of the text file containing the remote port number.\n",

NULL};
