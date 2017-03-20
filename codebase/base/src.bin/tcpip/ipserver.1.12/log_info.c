/* log_info.c
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>




char *logfname=NULL;

void log_info(char *str) {
  FILE *fp;
  char *date;
  char logpath[256];
  time_t ltime;
  struct tm *time_of_day;
 
  time(&ltime);  
  time_of_day=localtime(&ltime);

  date=asctime(time_of_day);  

  date[strlen(date)-1]=':';
  
  fprintf(stderr,date);
  fprintf(stderr,str);
  fprintf(stderr,"\n");


  sprintf(logpath,"%s.%.3d",logfname,time_of_day->tm_yday);
  fp=fopen(logpath,"a");
  fprintf(fp,date);
  fprintf(fp,str);
  fprintf(fp,"\n");
  fclose(fp);
}










