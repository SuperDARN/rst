/* mlt.h
   =====
   Author: R.J.Barnes
*/

/* 
   See license.txt
*/

#ifndef _MLT_H
#define _MLT_H

double MLTConvertYMDHMS(int yr,int mo,int dy,int hr,int mt,int sc,
                        double mlon);

double MLTConvertYrsec(int yr,int yrsec,double mlon);

double MLTConvertEpoch(double epoch,double mlon);

#endif
