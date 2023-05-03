/* iqwrite.c
   ========= 
   Author R.J.Barnes
*/

/*
 Copyright (C) <year>  <name of author>
 
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "iq.h"

int IQEncode(struct DataMap *ptr,struct IQ *iq,
            unsigned int *badtr,int16 *samples) {

  int c,s=0;

  int32 tnum,snum,bnum;
  int32 *tsc=NULL;
  int32 *tus=NULL;
  int16 *tatten=NULL;
  float *tnoise=NULL;
  int32 *toff=NULL;
  int32 *tsze=NULL;
  int32 *tbadtr=NULL;
  
  int32 seqnum,chnnum,smpnum,skpnum,btnum;

  seqnum=iq->seqnum;
  chnnum=iq->chnnum;
  smpnum=iq->smpnum;
  skpnum=iq->skpnum;
  btnum=iq->tbadtr;

  tnum=iq->seqnum;
  bnum=iq->tbadtr*2;

  snum=iq->seqnum*iq->chnnum*iq->smpnum*2;

  DataMapAddScalar(ptr,"iqdata.revision.major",DATAINT,
		    &iq->revision.major);
  DataMapAddScalar(ptr,"iqdata.revision.minor",DATAINT,
		    &iq->revision.minor);

  DataMapStoreScalar(ptr,"seqnum",DATAINT,&seqnum);
  DataMapStoreScalar(ptr,"chnnum",DATAINT,&chnnum);
  DataMapStoreScalar(ptr,"smpnum",DATAINT,&smpnum);
  DataMapStoreScalar(ptr,"skpnum",DATAINT,&skpnum);
  if (iq->tbadtr !=0) DataMapStoreScalar(ptr,"btnum",DATAINT,&btnum);

  if (iq->seqnum !=0) {
    tsc=(int32 *) DataMapStoreArray(ptr,"tsc",DATAINT,1,&tnum,NULL);
    if (tsc==NULL) s=-1; 
    tus=(int32 *) DataMapStoreArray(ptr,"tus",DATAINT,1,&tnum,NULL);  
    if (tus==NULL) s=-1; 
    tatten=(int16 *) DataMapStoreArray(ptr,"tatten",DATASHORT,1,&tnum,NULL);
    if (tatten==NULL) s=-1;
    tnoise=(float *) DataMapStoreArray(ptr,"tnoise",DATAFLOAT,1,&tnum,NULL);
    if (tnoise==NULL) s=-1;
    toff=(int32 *) DataMapStoreArray(ptr,"toff",DATAINT,1,&tnum,NULL);
    if (toff==NULL) s=-1;
    tsze=(int32 *) DataMapStoreArray(ptr,"tsze",DATAINT,1,&tnum,NULL);
    if (tsze==NULL) s=-1;
    if (iq->badtr !=NULL) {
       tbadtr=(int32 *) DataMapStoreArray(ptr,"tbadtr",DATAINT,1,&tnum,NULL);
       if (tbadtr==NULL) s=-1;
    }

    if (badtr !=NULL) DataMapStoreArray(ptr,"badtr",DATAINT,1,&bnum,badtr);
    DataMapStoreArray(ptr,"data",DATASHORT,1,&snum,samples);
 
    if (s==0) {
      for (c=0;c<tnum;c++) {
        tsc[c]=(int) iq->tval[c].tv_sec;
        tus[c]=(int) iq->tval[c].tv_nsec/1000.0;
        tatten[c]=iq->atten[c];
        tnoise[c]=iq->noise[c];
        toff[c]=iq->offset[c];
        tsze[c]=iq->size[c];
        if (iq->badtr !=NULL) tbadtr[c]=iq->badtr[c];
      }
    }
  }
  return s;
}


int IQWrite(int fid,struct RadarParm *prm,
	    struct IQ *iq,unsigned int *badtr,int16 *samples) {

  int s;
  struct DataMap *ptr=NULL;

  ptr=DataMapMake();
  if (ptr==NULL) return -1;

  s=RadarParmEncode(ptr,prm);   
  if (s==0) s=IQEncode(ptr,iq,badtr,samples);
  if (s==0) {
    if (fid !=-1) s=DataMapWrite(fid,ptr);
    else s=DataMapSize(ptr);
  }

  DataMapFree(ptr);
  return s;

}




int IQFwrite(FILE *fp,struct RadarParm *prm,
	     struct IQ *iq,unsigned int *badtr,int16 *samples) {
  return IQWrite(fileno(fp),prm,iq,badtr,samples);
}


