/* cnvmapread.h 
   ============
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _CNVMAPREAD_H
#define _CNVMAPREAD_H

int CnvMapRead(int fd,struct CnvMapData *map,struct GridData *grd);
int CnvMapFread(FILE *fp,struct CnvMapData *map,struct GridData *grd);

#endif









