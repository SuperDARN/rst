/* bound.h
   ======= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _BOUND_H
#define _BOUND_H

int FilterBoundType(struct RadarScan *ptr,int type);
int FilterBound(int prm,struct RadarScan *ptr,double *min,double *max);

#endif
