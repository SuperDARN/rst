/* rawwrite.c
   ========== 
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"

int RawEncode(struct DataMap *ptr,struct RadarParm *prm,struct RawData *raw) {

  int tx;
  int c,d,x;
  int32 p0num,snum;
  int32 anum[3],xnum[3];
 
  int16 *slist=NULL;
  float *acfd=NULL;
  float *xcfd=NULL;

  DataMapAddScalar(ptr,"rawacf.revision.major",DATAINT,
		    &raw->revision.major);

  DataMapAddScalar(ptr,"rawacf.revision.minor",DATAINT,
		    &raw->revision.minor);

  DataMapAddScalar(ptr,"thr",DATAFLOAT,&raw->thr);

  snum=0;
  /*tx=(int) ((raw->thr*prm->noise.search)/2.0);*/
  tx=0;

  for (c=0;c<prm->nrang;c++) {
    if (raw->pwr0[c]<tx) continue;
    snum++;
  }

  p0num=prm->nrang;
  anum[0]=2;  
  anum[1]=prm->mplgs;
  anum[2]=snum;

  if (prm->xcf !=0) {
    xnum[0]=2;
    xnum[1]=prm->mplgs;
    xnum[2]=snum;
  } else {
    xnum[0]=0;
    xnum[1]=0;
    xnum[2]=0;
  }

  if (raw->acfd[0] == NULL) {
    DataMapStoreArray(ptr,"pwr0",DATAFLOAT,1,&p0num,raw->pwr0);
    fprintf(stderr,"Warning: acfd array missing\n");
    return 0;
  }
  
  if (snum !=0) slist=DataMapStoreArray(ptr,"slist",DATASHORT,1,&snum,NULL);
  DataMapStoreArray(ptr,"pwr0",DATAFLOAT,1,&p0num,raw->pwr0);
  if (snum !=0) {
    acfd=DataMapStoreArray(ptr,"acfd",DATAFLOAT,3,anum,NULL);
    if (prm->xcf !=0) xcfd=DataMapStoreArray(ptr,"xcfd",DATAFLOAT,3,xnum,NULL);
    x=0;
    for (c=0;c<prm->nrang;c++) {
      if (raw->pwr0[c]<tx) continue;
      slist[x]=c;
      for (d=0;d<prm->mplgs;d++) {
        acfd[2*(x*prm->mplgs+d)]=raw->acfd[0][c*prm->mplgs+d];
        acfd[2*(x*prm->mplgs+d)+1]=raw->acfd[1][c*prm->mplgs+d];
      }
      if (prm->xcf !=0) for (d=0;d<prm->mplgs;d++) {
        xcfd[2*(x*prm->mplgs+d)]=raw->xcfd[0][c*prm->mplgs+d];
        xcfd[2*(x*prm->mplgs+d)+1]=raw->xcfd[1][c*prm->mplgs+d];
      }
      x++;
    }
  }
  return 0;
}


int RawWrite(int fid,struct RadarParm *prm,
            struct RawData *raw) {

  int s;
  struct DataMap *ptr=NULL;

  ptr=DataMapMake();
  if (ptr==NULL) return -1;

  s=RadarParmEncode(ptr,prm);

  if (s==0) s=RawEncode(ptr,prm,raw);

  if (s==0) {
    if (fid !=-1) s=DataMapWrite(fid,ptr);
    else s=DataMapSize(ptr);
  }

  DataMapFree(ptr);
  return s;

}

int RawFwrite(FILE *fp,struct RadarParm *prm,
              struct RawData *raw) {
  return RawWrite(fileno(fp),prm,raw);
}
