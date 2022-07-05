/* readgrid.c
   ==========
   Author: R.J.Barnes
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
#include <string.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "griddata.h"




int GridRead(int fid,struct GridData *gp) {
  int c,x,n;
  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  int size=0;
  int yr,mo,dy,hr,mt;
  double sc;


  void *tmp;


  char *sname[]={"start.year","start.month","start.day","start.hour",
                 "start.minute","start.second",
                 "end.year","end.month","end.day","end.hour",
                 "end.minute","end.second",0};

  int stype[]={DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATADOUBLE,
                 DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATADOUBLE};

  struct DataMapScalar *sdata[12];

  char *aname[]={"stid","channel","nvec",
                 "freq","major.revision","minor.revision",
                 "program.id","noise.mean","noise.sd","gsct",
                 "v.min","v.max","p.min","p.max","w.min","w.max","ve.min",
                 "ve.max",
                 "vector.mlat","vector.mlon","vector.kvect",
                 "vector.stid","vector.channel","vector.index",
                 "vector.vel.median","vector.vel.sd",
                 "vector.pwr.median","vector.pwr.sd",
                 "vector.wdt.median","vector.wdt.sd",0};

  int atype[]={DATASHORT,DATASHORT,DATASHORT,
               DATAFLOAT,DATASHORT,DATASHORT,
               DATASHORT,DATAFLOAT,DATAFLOAT,DATASHORT,
               DATAFLOAT,DATAFLOAT,DATAFLOAT,DATAFLOAT,DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT,DATAFLOAT,
               DATASHORT,DATASHORT,DATAINT,
               DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT};

  struct DataMapArray *adata[40];

  ptr=DataMapReadBlock(fid,&size);
  if (ptr==NULL) return -1;

  for (c=0;sname[c] !=0;c++) sdata[c]=NULL;
  for (c=0;aname[c] !=0;c++) adata[c]=NULL;

  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    for (x=0;sname[x] !=0;x++) 
      if ((strcmp(s->name,sname[x])==0) && (s->type==stype[x])) {
        sdata[x]=s;
        break;
      }
  }



  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    for (x=0;aname[x] !=0;x++) {
      if ((strcmp(a->name,aname[x])==0) && (a->type==atype[x])) {
        adata[x]=a;
        break;
      } 
    }
  }

  for (x=0;sname[x] !=0;x++) if (sdata[x]==NULL) break;

  if ((sname[x] !=0) && (sdata[x]==NULL)) {
    DataMapFree(ptr);
    return -1;
  }

 

  for (x=0;x<18;x++) if (adata[x]==NULL) break;
  if (x !=18) {
    DataMapFree(ptr);
    return -1;
  }

  yr=*(sdata[0]->data.sptr);
  mo=*(sdata[1]->data.sptr);
  dy=*(sdata[2]->data.sptr);
  hr=*(sdata[3]->data.sptr);
  mt=*(sdata[4]->data.sptr);
  sc=*(sdata[5]->data.dptr);
  gp->st_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  yr=*(sdata[6]->data.sptr);
  mo=*(sdata[7]->data.sptr);
  dy=*(sdata[8]->data.sptr);
  hr=*(sdata[9]->data.sptr);
  mt=*(sdata[10]->data.sptr);
  sc=*(sdata[11]->data.dptr);
  gp->ed_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  gp->stnum=adata[0]->rng[0];
  if (gp->stnum==0) {
    DataMapFree(ptr);
    return -1;
  }  

  if (gp->sdata !=NULL) {
    tmp=realloc(gp->sdata,sizeof(struct GridSVec)*gp->stnum);
    if (tmp==NULL) {
      DataMapFree(ptr);
      return -1;
    }
    gp->sdata=tmp;
  } else gp->sdata=malloc(sizeof(struct GridSVec)*gp->stnum);

  if (gp->sdata==NULL) {
     DataMapFree(ptr);
     return -1;
  }
 
  for (n=0;n<gp->stnum;n++) {    
    gp->sdata[n].st_id=adata[0]->data.sptr[n];
    gp->sdata[n].chn=adata[1]->data.sptr[n];
    gp->sdata[n].npnt=adata[2]->data.sptr[n];
    gp->sdata[n].freq0=adata[3]->data.fptr[n];
    gp->sdata[n].major_revision=adata[4]->data.sptr[n];
    gp->sdata[n].minor_revision=adata[5]->data.sptr[n];
    gp->sdata[n].prog_id=adata[6]->data.sptr[n];
    gp->sdata[n].noise.mean=adata[7]->data.fptr[n];
    gp->sdata[n].noise.sd=adata[8]->data.fptr[n];
    gp->sdata[n].gsct=adata[9]->data.sptr[n];
    gp->sdata[n].vel.min=adata[10]->data.fptr[n];
    gp->sdata[n].vel.max=adata[11]->data.fptr[n];
    gp->sdata[n].pwr.min=adata[12]->data.fptr[n];
    gp->sdata[n].pwr.max=adata[13]->data.fptr[n];
    gp->sdata[n].wdt.min=adata[14]->data.fptr[n];
    gp->sdata[n].wdt.max=adata[15]->data.fptr[n];
    gp->sdata[n].verr.min=adata[16]->data.fptr[n];
    gp->sdata[n].verr.max=adata[17]->data.fptr[n];
  }

  if (adata[18] !=NULL) gp->vcnum=adata[18]->rng[0];
  else {

    gp->vcnum=0;
    if (gp->data !=NULL) free(gp->data);
    gp->data=NULL;
    DataMapFree(ptr);
    return size;
  }
 
  if (gp->data !=NULL) {
    tmp=realloc(gp->data,sizeof(struct GridGVec)*gp->vcnum);
    if (tmp==NULL) {
      DataMapFree(ptr);
      return -1;
    }
    gp->data=tmp;
  } else gp->data=malloc(sizeof(struct GridGVec)*gp->vcnum);


  if (gp->data==NULL) {
     DataMapFree(ptr);
     return -1;
  }

  gp->xtd=0;

  for (n=26;n<30;n++) if (adata[n] !=NULL) {
    gp->xtd=1;
    break;
  }
  for (n=0;n<gp->vcnum;n++) {
    gp->data[n].mlat=adata[18]->data.fptr[n];
    gp->data[n].mlon=adata[19]->data.fptr[n];
    gp->data[n].azm=adata[20]->data.fptr[n];
    
    gp->data[n].st_id=adata[21]->data.sptr[n];
    gp->data[n].chn=adata[22]->data.sptr[n];
    gp->data[n].index=adata[23]->data.iptr[n];
    gp->data[n].vel.median=adata[24]->data.fptr[n];
    gp->data[n].vel.sd=adata[25]->data.fptr[n];
    gp->data[n].pwr.median=0;
    gp->data[n].pwr.sd=0;
    gp->data[n].wdt.median=0;
    gp->data[n].wdt.sd=0;

    if (adata[26] !=NULL) gp->data[n].pwr.median=adata[26]->data.fptr[n];
    if (adata[27] !=NULL) gp->data[n].pwr.sd=adata[27]->data.fptr[n];
    if (adata[28] !=NULL) gp->data[n].wdt.median=adata[28]->data.fptr[n];
    if (adata[29] !=NULL) gp->data[n].wdt.sd=adata[29]->data.fptr[n];
    
  }
   
  DataMapFree(ptr);
  return size;
}

int GridFread(FILE *fp,struct GridData *ptr) {
  return GridRead(fileno(fp),ptr);
}


