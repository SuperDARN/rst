/* writegrid.c
   ===========
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
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "griddata.h"




int GridWrite(int fid,struct GridData *ptr) {
  struct DataMap *data;

  int s;
  int32 stnum=0;
  int32 npnt=0;

  int16 syr,smo,sdy,shr,smt;
  double ssc;
  int16 eyr,emo,edy,ehr,emt;
  double esc;

  int yr,mo,dy,hr,mt;
  double sc;


  int16 *stid=NULL;
  int16 *chn=NULL;
  int16 *nvec=NULL;
  float *freq=NULL;
  int16 *major_rev=NULL;
  int16 *minor_rev=NULL;
  int16 *progid=NULL;
  float *noise_mean=NULL;
  float *noise_sd=NULL;
  int16 *gsct=NULL;
  float *v_min=NULL;
  float *v_max=NULL;
  float *p_min=NULL;
  float *p_max=NULL;
  float *w_min=NULL;
  float *w_max=NULL;
  float *ve_min=NULL;
  float *ve_max=NULL;
 
  float *gmlon=NULL;
  float *gmlat=NULL;
  float *kvect=NULL;
  float *srng=NULL;
  int16 *vstid=NULL;
  int16 *vchn=NULL;
  int32 *index=NULL;
  float *vlos=NULL;
  float *vlos_sd=NULL;
  float *pwr=NULL;
  float *pwr_sd=NULL;
  float *wdt=NULL;
  float *wdt_sd=NULL;

  int size=0;
  unsigned char *buf=NULL;
  unsigned char *bptr=NULL;

  int xtd=0;
  int n,p;
  for (n=0;n<ptr->stnum;n++) if (ptr->sdata[n].st_id !=-1) stnum++;
  if (stnum==0) return 0;

  size=stnum*(11*sizeof(float)+7*sizeof(int16));

  xtd=ptr->xtd;

  for (n=0;n<ptr->vcnum;n++) if (ptr->data[n].st_id !=-1) npnt++;

  size+=npnt*(4*sizeof(float)+sizeof(int32)+
              2*sizeof(int16)+(2+4*xtd)*sizeof(float));
  if (size==0) return 0;
  buf=malloc(size);
  if (buf==NULL) return -1;

  bptr=buf;

  stid=(int16 *) bptr;
  bptr+=stnum*sizeof(int16);
  chn=(int16 *) bptr;
  bptr+=stnum*sizeof(int16);
  nvec=(int16 *) bptr;
  bptr+=stnum*sizeof(int16);
  freq=(float *) bptr;
  bptr+=stnum*sizeof(float);
  major_rev=(int16 *) bptr;
  bptr+=stnum*sizeof(int16);
  minor_rev=(int16 *) bptr;
  bptr+=stnum*sizeof(int16);
  progid=(int16 *) bptr;
  bptr+=stnum*sizeof(int16);
  noise_mean=(float *) bptr;
  bptr+=stnum*sizeof(float);
  noise_sd=(float *) bptr;
  bptr+=stnum*sizeof(float);
  gsct=(int16 *) bptr;
  bptr+=stnum*sizeof(int16);

  v_min=(float *) bptr;
  bptr+=stnum*sizeof(float);
  v_max=(float *) bptr;
  bptr+=stnum*sizeof(float);

  p_min=(float *) bptr;
  bptr+=stnum*sizeof(float);
  p_max=(float *) bptr;
  bptr+=stnum*sizeof(float);

  w_min=(float *) bptr;
  bptr+=stnum*sizeof(float);
  w_max=(float *) bptr;
  bptr+=stnum*sizeof(float);

  ve_min=(float *) bptr;
  bptr+=stnum*sizeof(float);
  ve_max=(float *) bptr;
  bptr+=stnum*sizeof(float);

  n=0;
  for (p=0;p<ptr->stnum;p++) if (ptr->sdata[p].st_id !=-1) {
     stid[n]=ptr->sdata[p].st_id;
     chn[n]=ptr->sdata[p].chn;
     nvec[n]=ptr->sdata[p].npnt;
     freq[n]=ptr->sdata[p].freq0;
     major_rev[n]=ptr->sdata[p].major_revision;
     minor_rev[n]=ptr->sdata[p].minor_revision;
     progid[n]=ptr->sdata[p].prog_id;
     noise_mean[n]=ptr->sdata[p].noise.mean;
     noise_sd[n]=ptr->sdata[p].noise.sd;
     gsct[n]=ptr->sdata[p].gsct;
     v_min[n]=ptr->sdata[p].vel.min;
     v_max[n]=ptr->sdata[p].vel.max;
     p_min[n]=ptr->sdata[p].pwr.min;
     p_max[n]=ptr->sdata[p].pwr.max;
     w_min[n]=ptr->sdata[p].wdt.min;
     w_max[n]=ptr->sdata[p].wdt.max;
     ve_min[n]=ptr->sdata[p].verr.min;
     ve_max[n]=ptr->sdata[p].verr.max;
     n++;
  }
  if (npnt !=0) {
    gmlon=(float *) bptr;
    bptr+=npnt*sizeof(float);
    gmlat=(float *) bptr;
    bptr+=npnt*sizeof(float);
    kvect=(float *) bptr;
    bptr+=npnt*sizeof(float);
    srng=(float *) bptr;
    bptr+=npnt*sizeof(float);
    vstid=(int16 *) bptr;
    bptr+=npnt*sizeof(int16);
    vchn=(int16 *)  bptr;
    bptr+=npnt*sizeof(int16);
    index=(int32 *) bptr;
    bptr+=npnt*sizeof(int32);
    vlos=(float *)  bptr;
    bptr+=npnt*sizeof(float);
    vlos_sd=(float *) bptr;
    bptr+=npnt*sizeof(float);
	    
    if (xtd) {
       pwr=(float *) bptr;
       bptr+=npnt*sizeof(float);
       pwr_sd=(float *) bptr;
       bptr+=npnt*sizeof(float);
       wdt=(float *) bptr;
       bptr+=npnt*sizeof(float);
       wdt_sd=(float *) bptr;
       bptr+=npnt*sizeof(float);
    }
    n=0;
    for (p=0;p<ptr->vcnum;p++) if (ptr->data[p].st_id !=-1) {
      gmlat[n]=ptr->data[p].mlat;
      gmlon[n]=ptr->data[p].mlon;
      kvect[n]=ptr->data[p].azm;
      srng[n]=ptr->data[p].srng;
      vstid[n]=ptr->data[p].st_id;
      vchn[n]=ptr->data[p].chn;
      index[n]=ptr->data[p].index;
      vlos[n]=ptr->data[p].vel.median;
      vlos_sd[n]=ptr->data[p].vel.sd;
      if (xtd) {
        pwr[n]=ptr->data[p].pwr.median;
        pwr_sd[n]=ptr->data[p].pwr.sd;
        wdt[n]=ptr->data[p].wdt.median;
        wdt_sd[n]=ptr->data[p].wdt.sd;
      }
      n++;
    }
  }
 
  TimeEpochToYMDHMS(ptr->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
  eyr=yr;
  emo=mo;
  edy=dy;
  ehr=hr;
  emt=mt;
  esc=sc;
  TimeEpochToYMDHMS(ptr->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
  syr=yr;
  smo=mo;
  sdy=dy;
  shr=hr;
  smt=mt;
  ssc=sc;

  data=DataMapMake();

  DataMapAddScalar(data,"start.year",DATASHORT,&syr);
  DataMapAddScalar(data,"start.month",DATASHORT,&smo);
  DataMapAddScalar(data,"start.day",DATASHORT,&sdy);
  DataMapAddScalar(data,"start.hour",DATASHORT,&shr);
  DataMapAddScalar(data,"start.minute",DATASHORT,&smt);
  DataMapAddScalar(data,"start.second",DATADOUBLE,&ssc);

  DataMapAddScalar(data,"end.year",DATASHORT,&eyr);
  DataMapAddScalar(data,"end.month",DATASHORT,&emo);
  DataMapAddScalar(data,"end.day",DATASHORT,&edy);
  DataMapAddScalar(data,"end.hour",DATASHORT,&ehr);
  DataMapAddScalar(data,"end.minute",DATASHORT,&emt);
  DataMapAddScalar(data,"end.second",DATADOUBLE,&esc);

  DataMapAddArray(data,"stid",DATASHORT,1,&stnum,stid);
  DataMapAddArray(data,"channel",DATASHORT,1,&stnum,chn);
  DataMapAddArray(data,"nvec",DATASHORT,1,&stnum,nvec);
  DataMapAddArray(data,"freq",DATAFLOAT,1,&stnum,freq);
  DataMapAddArray(data,"major.revision",DATASHORT,1,&stnum,major_rev);
  DataMapAddArray(data,"minor.revision",DATASHORT,1,&stnum,minor_rev);
  DataMapAddArray(data,"program.id",DATASHORT,1,&stnum,progid    );
  DataMapAddArray(data,"noise.mean",DATAFLOAT,1,&stnum,noise_mean);
  DataMapAddArray(data,"noise.sd",DATAFLOAT,1,&stnum,noise_sd);
  DataMapAddArray(data,"gsct",DATASHORT,1,&stnum ,gsct);
  DataMapAddArray(data,"v.min",DATAFLOAT,1,&stnum,v_min);
  DataMapAddArray(data,"v.max",DATAFLOAT,1,&stnum,v_max);
  DataMapAddArray(data,"p.min",DATAFLOAT,1,&stnum,p_min);
  DataMapAddArray(data,"p.max",DATAFLOAT,1,&stnum,p_max);
  DataMapAddArray(data,"w.min",DATAFLOAT,1,&stnum,w_min);
  DataMapAddArray(data,"w.max",DATAFLOAT,1,&stnum,w_max);
  DataMapAddArray(data,"ve.min",DATAFLOAT,1,&stnum,ve_min);
  DataMapAddArray(data,"ve.max",DATAFLOAT,1,&stnum,ve_max);

  if (npnt !=0) {
    DataMapAddArray(data,"vector.mlat",DATAFLOAT,1,&npnt,gmlat);
    DataMapAddArray(data,"vector.mlon",DATAFLOAT,1,&npnt,gmlon);
    DataMapAddArray(data,"vector.kvect",DATAFLOAT,1,&npnt,kvect);
    DataMapAddArray(data,"vector.srng",DATAFLOAT,1,&npnt,srng);
    DataMapAddArray(data,"vector.stid",DATASHORT,1,&npnt,vstid);
    DataMapAddArray(data,"vector.channel",DATASHORT,1,&npnt,vchn);
    DataMapAddArray(data,"vector.index",DATAINT,1,&npnt,index);
    DataMapAddArray(data,"vector.vel.median",DATAFLOAT,1,&npnt,vlos);
    DataMapAddArray(data,"vector.vel.sd",DATAFLOAT,1,&npnt,vlos_sd);
    if (xtd) {
      DataMapAddArray(data,"vector.pwr.median",DATAFLOAT,1,&npnt,pwr);
      DataMapAddArray(data,"vector.pwr.sd",DATAFLOAT,1,&npnt,pwr_sd);
      DataMapAddArray(data,"vector.wdt.median",DATAFLOAT,1,&npnt,wdt);
      DataMapAddArray(data,"vector.wdt.sd",DATAFLOAT,1,&npnt,wdt_sd);

    }
    
  }
  if (fid !=-1) s=DataMapWrite(fid,data);
  else s=DataMapSize(data);
  DataMapFree(data);
  free(buf);
  return s;

}






int GridFwrite(FILE *fp,struct GridData *ptr) {
  return GridWrite(fileno(fp),ptr);
}





