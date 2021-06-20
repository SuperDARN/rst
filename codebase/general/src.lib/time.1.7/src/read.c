/* read.c
   ======
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>



void TimeReadClock(int *yr,int *month,int *day,int *hour,int *min,int *sec,
		   int *usec) {


  struct timeval tv;

  time_t tm;
  struct tm *gmt_time;
  int s;


  s=gettimeofday(&tv,NULL);
  if (s !=0) {
    fprintf(stderr,"gettimeofday failed.\n");
    exit(-1);
  }

  tm=tv.tv_sec;
  gmt_time = gmtime(&tm);
  *yr = gmt_time->tm_year + 1900;
  *month = gmt_time->tm_mon + 1;
  *day = gmt_time->tm_mday; 
  *hour = gmt_time->tm_hour;
  *min = gmt_time->tm_min;
  *sec = gmt_time->tm_sec;
  *usec = tv.tv_usec;
}  
