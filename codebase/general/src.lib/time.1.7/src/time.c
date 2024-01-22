/* time.c
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

2021-08-27 Angeline G Burrell: Moved string-to-epoch-time routines here from
           make_grid.2.0 to allow better access across all tools.

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "rtime.h"


#define DAY_SEC 86400

int TimeYMDHMSToYrsec(int yr,int mo,int dy,int hr,int mn,int sec) {

  time_t clock;
  struct tm tm;
  char *tz;

  memset(&tm,0,sizeof(struct tm));
  tm.tm_year=yr-1900;
  tm.tm_mon=0;
  tm.tm_mday=1;
  tm.tm_hour=0;
  tm.tm_min=0;
  tm.tm_sec=0;

  tz = getenv("TZ");
  setenv("TZ", "", 1);
  tzset();
  clock=mktime(&tm);

  memset(&tm,0,sizeof(struct tm));
  tm.tm_year=yr-1900;
  tm.tm_mon=mo-1;
  tm.tm_mday=dy;
  tm.tm_hour=hr;
  tm.tm_min=mn;
  tm.tm_sec=sec;
  clock=mktime(&tm)-clock;
  

  if (tz) setenv("TZ", tz, 1);
  else unsetenv("TZ");
  tzset();

  return (int) clock;
}

void TimeYrsecToYMDHMS(int yrsec,int yr,int *mo,int *dy,int *hr,int *mn,
	                int *sc) {


  time_t clock;
  struct tm tmyr;
  struct tm *tm;
  char *tz;

  memset(&tmyr,0,sizeof(struct tm));
  tmyr.tm_year=yr-1900;
  tmyr.tm_mon=0;
  tmyr.tm_mday=1;
  tmyr.tm_hour=0;
  tmyr.tm_min=0;
  tmyr.tm_sec=0;

  tz = getenv("TZ");
  setenv("TZ", "", 1);
  tzset();
 
  clock=mktime(&tmyr);
  if (tz) setenv("TZ", tz, 1);
  else unsetenv("TZ");
  tzset();

  clock=clock+yrsec;
  tm=gmtime(&clock);
  
  *mo=tm->tm_mon+1;
  *dy=tm->tm_mday;
  *hr=tm->tm_hour; 
  *mn=tm->tm_min;
  *sc=tm->tm_sec;
}


double TimeYMDHMSToEpoch(int yr,int mo,int dy,int hr,int mn,double sec) {
 
  time_t clock;
  struct tm tm;
  char *tz;

  memset(&tm,0,sizeof(struct tm));
  tm.tm_year=yr-1900;
  tm.tm_mon=mo-1;
  tm.tm_mday=dy;
  tm.tm_hour=hr;
  tm.tm_min=mn;
  tm.tm_sec=floor(sec);

  tz = getenv("TZ");
  setenv("TZ", "", 1);
  tzset();
  clock=mktime(&tm);
  if (tz) setenv("TZ", tz, 1);
  else unsetenv("TZ");
  tzset();
               
  return clock+(sec-floor(sec));
}

void TimeEpochToYMDHMS(double tme,int *yr,int *mo,int *dy,int *hr,int *mn,
	       double *sc) {
  time_t clock;
  struct tm *tm;

  clock=floor(tme);
  tm=gmtime(&clock);
  
  *yr=tm->tm_year+1900;
  *mo=tm->tm_mon+1;
  *dy=tm->tm_mday;
  *hr=tm->tm_hour; 
  *mn=tm->tm_min;
  *sc=tm->tm_sec+(tme-floor(tme));
}

double TimeYMDHMSToJulian(int yr,int mo,int dy,int hr,int mt,double sc) {

  int A,B,i;
  double jdoy;
  double dfrac;
  yr=yr-1;
  i=yr/100;
  A=i;
  i=A/4;
  B=2-A+i;
  i=365.25*yr;
  i+=30.6001*14;
  jdoy=i+1720994.5+B;

  
  dfrac=1.0+(double)TimeYMDHMSToYrsec(yr+1,mo,dy,hr,mt,sc)/DAY_SEC;
   
  return jdoy+dfrac; 

}


int TimeJulianToYMDHMS(double jd,int *yr,int *mo,
                 int *dy,int *hr,int *mt,double *sc) {

  int Z,month;
  int hour,minute;

  double A,B,C,D,E,F,alpha,day,year,factor,second;

  factor=0.5/DAY_SEC/1000; 
  F=(jd+0.5)-floor(jd+0.5);
  if ((F+factor)>=1.0) {
    jd=jd+factor;
    F=0.0;
  }

  Z=floor(jd+0.5);

  if (Z<2299161) A=Z;
  else {
    alpha=floor((Z-1867216.25)/36524.25);
    A=Z+1+alpha-floor(alpha/4);
  }

  B=A+1524;
  C=floor((B-122.1)/365.25);
  D=floor(365.25*C);
  E=floor((B-D)/30.6001);
  day=B-D-floor(30.6001*E)+F;

  if (E<13.5) month=floor(E-0.5);
  else month=floor(E-12.5);
  if (month>2.5) year=C-4716;
  else year=C-4715;
  


  *yr=(int) year;
  *mo=month;
  *dy=(int) floor(day);

  /* okay now use the residual of the day to work out the time */

  A=(day-floor(day))*DAY_SEC;

  hour=(int) (A/3600.0);
  minute=(int) ((A-hour*3600)/60);
  second=A-hour*3600-minute*60;
    
  *hr=hour;
  *mt=minute;
  *sc=second;
  return 0;
}

/**
 * @brief Convert an input time from HHMM format to number of seconds.
 *
 * @param[in] text - String with the format HHMM
 *
 * @param[out] Seconds of day corresponding to input string
 **/
double TimeStrToSOD(char *text)
{
  int i, hr, mn;

  /* Determine the location of a possible colon in the text string */
  for(i = 0; (text[i] != ':') && (text[i] != 0); i++);

  /* Only hours are specified, convert to seconds and return */
  if(text[i] == 0) return atoi(text) * 3600L;

  /* Extract both the minutes and seconds from the string */
  text[i] = 0;
  hr = atoi(text);
  mn = atoi(text + i + 1);
  return hr * 3600L + mn * 60L;
}

/**
 * @brief Convert a formatted string to an epoch time
 *
 * @param[in] text - String with the format YYYYMMDD
 *
 * @param[out] tme - Number of seconds since 00:00 UT on January 1, 1970.
 **/
double TimeStrToEpoch(char *text)
{
  double tme;
  int val;
  int yr,mo,dy;

  /* Calculate day, month, and year from YYYYMMDD format date */
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);

  /* If only 2-digit year provided then assume it was pre-2000 */
  if (yr<1970) yr+=1900;

  /* Calculate epoch time of input year, month, and day */
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  /* Return epoch time in number of seconds since 00:00UT on January 1, 1970 */
  return tme;
}


/*-----------------------------------------------------------------------------
;
; NAME:
;       dayno
;
; PURPOSE:
;       Function to compute the day of the year and the number of days in the
;       year.
;
; CALLING SEQUENCE:
;       doy = dayno(year, month, day, diy);
;
;     Input Arguments:
;       year          - year [1965-2014]
;       month         - month of year [01-12]
;       day           - day of month [01-31]
;
;     Output Arguments (integer pointers):
;       diy           - number of days in the given year
;
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------
*/

int dayno(int year, int month, int day, int *diy)
{
  int k,tot;
  int ndays[] = {31,28,31,30,31,30,31,31,30,31,30,31};

  *diy = 365;
  if(((year%4==0)&&(year%100!=0))||(year%400==0)) {
    ndays[1]++;
    *diy = 366;
  }

  tot = 0;
  for (k=0; k<month-1; k++) tot += ndays[k];
  tot += day;

  return tot;
}
