/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"merge_rt - Combines together multiple grid files to produce a single output file at specific intervals.\n",
"merge_rt --help\n",
"merge_rt [-vb] [-r]  [-L logname] [-if pidname] gridnames...\n",
"merge_rt -new [-vb] [-r]  [-L logname] [-if pidname] gridnames...\n",

"--help\tprint the help message and exit.\n",
"-i interval\tdivide the day into interval second intervals.\n",
"-o offset\tread the input files at offset seconds into each interval.\n",
"-r\tget the record time from the most recent record in the files, not from the record with the most data.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"gridnames\tfilenames of the grd format files to combine.\n",
"-new\tthe input files are in the grdmap format file.\n",

NULL};
