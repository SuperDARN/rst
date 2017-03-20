/* loginfo.c
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


void loginfo(char *fname,char *str) {
  FILE *fp;
  char *date;
  char logpath[1024];
  time_t ltime;
  struct tm *time_of_day;
 
  time(&ltime);  
  time_of_day=localtime(&ltime);

  date=asctime(time_of_day);  

  date[strlen(date)-1]=':';
  
  fprintf(stderr,date);
  fprintf(stderr,str);
  fprintf(stderr,"\n");

   sprintf(logpath,"%s.%.4d%.2d%.2d",fname,1900+
          time_of_day->tm_year,time_of_day->tm_mon+1,
          time_of_day->tm_mday);

  fp=fopen(logpath,"a");
  if (fp==NULL) {
    fprintf(stderr,"WARNING:Log failed.\n");
    return;
  }
  fprintf(fp,date);
  fprintf(fp,str);
  fprintf(fp,"\n");
  fclose(fp);
}










