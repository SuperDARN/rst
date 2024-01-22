/* rtime.h
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

2021-08-27 Angeline G. Burrell: Moved string-to-epoch-time routines here from
           make_grid.2.0 to allow better access across all tools.
*/



#ifndef _RTIME_H
#define _RTIME_H

void TimeYrsecToYMDHMS(int time,int yr, 
                        int *mo,int *dy,
                        int *hr,int *mn,int *sc);
int TimeYMDHMSToYrsec(int yr, int mo, int day, int hr, int mn,int sec);
double TimeYMDHMSToEpoch(int yr,int mo,int dy,int hr,int mn,double sec);
void TimeEpochToYMDHMS(double tme,int *yr,int *mo,int *dy,int *hr,int *mn,
                       double *sc);
double TimeYMDHMSToJulian(int yr,int mo,int dy,int hr,int mt,double sc);
int TimeJulianToYMDHMS(double jd,int *yr,int *mo,
                       int *dy,int *hr,int *mt,double *sc);

void TimeReadClock(int *yr,int *month,int *day,int *hour,int *min,int *sec,
                   int *usec);

double TimeStrToSOD(char *text);
double TimeStrToEpoch(char *text);
int dayno(int year, int month, int day, int *diy);


#endif
