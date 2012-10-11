/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"rtcfit - Creates a cFit format file from a fitacf TCP/IP data stream.\n",
"rtcfit --help\n",
"rtcfit [-vb] [-mp minpwr]  [-L logname] [-p pathname] [-if pidname] [-s stream] host port\n",
"rtcfit -rpf [-vb] [-mp minpwr] [-L logname] [-p pathname] [-if pidname] [-s stream] host portname\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-mp minpwr\tfilter out data with values of lag-zero power less than minpwr.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-p pathname\tstore the daily files in the directory pathname.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-s stream\tfor a multiplexed stream, read the stream number given by stream. A value of 255 will read all streams\n",
"host\thostname or IP number of the system to connect to.\n",
"rport\tport number to connect to on the server.\n",
"-rpf\tThe remote port number is stored in a text file.\n",
"rportname\tfilename of the text file containing the remote port number.\n",

NULL};
