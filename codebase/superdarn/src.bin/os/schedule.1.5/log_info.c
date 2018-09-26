/* log_info.c
   ==========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern char *logname;

void log_info(int flg,char *str) {
  FILE *fp;
  char *date;
  char logpath[256];
  time_t ltime;
  struct tm *time_of_day;
 
  time(&ltime);  
  time_of_day=gmtime(&ltime);

  date=asctime(time_of_day);  
  date[strlen(date)-1]=':';
  if (flg==0) fprintf(stderr,date);
  fprintf(stderr,str);
  fprintf(stderr,"\n");
  
  sprintf(logpath,"%s.%.4d%.2d%.2d",logname,1900+
          time_of_day->tm_year,time_of_day->tm_mon+1,
          time_of_day->tm_mday);
  fp=fopen(logpath,"a");
  if (fp !=NULL) {
    if (flg==0) fprintf(fp,date);
    fprintf(fp,str);
    fprintf(fp,"\n");
    fclose(fp);
  }
}










