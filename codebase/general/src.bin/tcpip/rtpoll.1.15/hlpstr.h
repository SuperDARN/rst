/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"rtpoll - Periodically polls a file to see if it has been modified, and if it has, executes a command.\n",
"rtpoll --help\n",
"rtpoll [-L logname][-if pidname] [-d pollsec] [-f name] command\n",

"--help\tprint the help message and exit.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-d pollsec\tlength of time to wait between polling to see if the data file has been modified, to pollsec seconds.\n",
"-f name\tpoll to see if the file name has been modified.\n",
"command\tcommand to execute if the data file has been modified.\n",

NULL};
