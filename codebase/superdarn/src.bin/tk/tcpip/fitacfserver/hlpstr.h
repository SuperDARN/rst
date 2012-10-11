/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"fitacfserver - Simple TCP/IP server program for fitacf and fit format files.\n",
"fitacfserver --help\n",
"fitacfserver [-a] [-d] [-r] [-new] [-L logname] [-pf portname] [-if pidname] [-lp port] [-b bound] [-s stid] [-i intt] name\n",

"--help\tprint the help message and exit.\n",
"-a\treplace the record times in the file with the actual time. \n",
"-d\tthe file contains one complete day of data and the server should send the appropriate record for the current time of day. \n",
"-r\tre-open the file when the end is reached.\n",
"-new\tinput file is in fitacf format rather than fit format.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-pf portname\trecord the port number that the server is listening for connections on in the file portname. By default, the port is  recorded in port.id.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-lp port\tforce the server to listen on the local port number port. By default, the server requests a port number from the Operating System.\n",
"-b bound\tsynchronize scans with an interval of bound seconds.\n",
"-s stid\tset the station identifier in the data records to stid.\n",
"-i intt\tignore the integration time in the data records and instead use intt seconds.\n",
"name\tfilename of the fitacf or fit format file to serve.\n",

NULL};
