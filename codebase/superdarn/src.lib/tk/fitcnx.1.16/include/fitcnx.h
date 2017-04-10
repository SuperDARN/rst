/* fitcnx.h
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/





#ifndef _FITCNX_H
#define _FITCNX_H

int FitCnxRead(int num,int *sock,struct RadarParm *prm,
                    struct FitData *fit,int *flag,struct timeval *tout); 

#endif









