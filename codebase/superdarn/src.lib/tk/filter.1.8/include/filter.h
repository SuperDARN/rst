/* filter.h
   ======= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _FILTER_H
#define _FILTER_H

#define MAX_BEAM 256
#define FILTER_WIDTH 3
#define FILTER_HEIGHT 3
#define FILTER_DEPTH 3

int FilterRadarScan(int mode,int depth,int inx,struct RadarScan **src,
                    struct RadarScan *dst,int prm);


#endif
