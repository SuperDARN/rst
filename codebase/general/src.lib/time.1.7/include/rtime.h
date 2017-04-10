/* rtime.h
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
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


#endif
