/* cnvmapwrite.h 
   ============
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _CNVMAPWRITE_H
#define _CNVMAPWRITE_H

int CnvMapWrite(int fd,struct CnvMapData *map,struct GridData *grd);
int CnvMapFwrite(FILE *fp,struct CnvMapData *map,struct GridData *grd);

#endif









