/* loginfo.c
   =========
   Author: R.J.Barnes

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
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

  date[strlen(date)-1]=0;
  
  fprintf(stderr,"%s : (%d) : %s\n",date,getpid(),str);

  sprintf(logpath,"%s.%.4d%.2d%.2d",fname,1900+
          time_of_day->tm_year,time_of_day->tm_mon+1,
          time_of_day->tm_mday);

  fp=fopen(logpath,"a");
  if (fp==NULL) {
    fprintf(stderr,"WARNING:Log failed.\n");
    return;
  }

  fprintf(fp,"%s : (%d) : %s\n",date,getpid(),str);
  fclose(fp);
}

