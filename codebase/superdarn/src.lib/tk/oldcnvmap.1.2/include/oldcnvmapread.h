/* oldcnvmapread.h 
   ===============
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _OLDCNVMAPREAD_H
#define _OLDCNVMAPREAD_H

int OldCnvMapFread(FILE *fp,struct CnvMapData *map,struct GridData *grd);
int OldCnvMapFseek(FILE *fp,int yr,int mo,int dy,int hr,int mt,int sc,
               struct RfileIndex *inx,double *aval);


#endif









