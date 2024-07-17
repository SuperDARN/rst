/* oldreadgrid.c
   =============
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
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"



#define BLOCK1 "st_id nvec freq0 major_rev minor_rev prog_id noise_mean noise_sd gsct v_min v_max p_min p_max w_min w_max ve_min ve_max"

#define BLOCK2 "st_id chn nvec freq0 major_rev minor_rev prog_id noise_mean noise_sd gsct v_min v_max p_min p_max w_min w_max ve_min ve_max"


#define BLOCK3 "gmlong gmlat kvect st_id grid_index vlos vlos_sd"


#define BLOCK4 "gmlong gmlat kvect st_id grid_index vlos vlos_sd pwr pwr_sd wdt wdt_sd"

 
#define BLOCK5 "gmlong gmlat kvect st_id chn grid_index vlos vlos_sd"


#define BLOCK6 "gmlong gmlat kvect st_id chn grid_index vlos vlos_sd pwr pwr_sd wdt wdt_sd"




int OldGridDecodeOne(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct GridData *gp;
  gp=(struct GridData *)ptr;
 
  if (strcmp(name,BLOCK1) !=0) return 0;
 
  if (pnt==0) {
    if (npnt>0) {
      if (gp->sdata !=NULL) gp->sdata=realloc(gp->sdata,
        sizeof(struct GridSVec)*npnt);
      else gp->sdata=malloc( sizeof(struct GridSVec)*npnt);
    } else {
      free(gp->sdata);
      gp->sdata=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->stnum=npnt;
  }

  if (npnt>0) {
    gp->sdata[pnt].st_id=data[0].data.ival;
    gp->sdata[pnt].chn=0;
    gp->sdata[pnt].npnt=data[1].data.ival;
    gp->sdata[pnt].freq0=data[2].data.fval;
    gp->sdata[pnt].major_revision=data[3].data.ival;
    gp->sdata[pnt].minor_revision=data[4].data.ival;
    gp->sdata[pnt].prog_id=data[5].data.ival;
    gp->sdata[pnt].noise.mean=data[6].data.fval;
    gp->sdata[pnt].noise.sd=data[7].data.fval;
    gp->sdata[pnt].gsct=data[8].data.ival;
    gp->sdata[pnt].vel.min=data[9].data.fval;
    gp->sdata[pnt].vel.max=data[10].data.fval;
    gp->sdata[pnt].pwr.min=data[11].data.fval;
    gp->sdata[pnt].pwr.max=data[12].data.fval;
    gp->sdata[pnt].wdt.min=data[13].data.fval;
    gp->sdata[pnt].wdt.max=data[14].data.fval;
    gp->sdata[pnt].verr.min=data[15].data.fval;
    gp->sdata[pnt].verr.max=data[16].data.fval;
   
  }
  return 1;
}


int OldGridDecodeTwo(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct GridData *gp;
  gp=(struct GridData *)ptr;


  if (strcmp(name,BLOCK2) !=0) return 0;
 
  if (pnt==0) {
    if (npnt>0) {
      if (gp->sdata !=NULL) gp->sdata=realloc(gp->sdata,
        sizeof(struct GridSVec)*npnt);
      else gp->sdata=malloc( sizeof(struct GridSVec)*npnt);
    } else {
      free(gp->sdata);
      gp->sdata=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->stnum=npnt;
  }

  if (npnt>0) {
    gp->sdata[pnt].st_id=data[0].data.ival;
    gp->sdata[pnt].chn=data[1].data.ival;
    gp->sdata[pnt].npnt=data[2].data.ival;
    gp->sdata[pnt].freq0=data[3].data.fval;
    gp->sdata[pnt].major_revision=data[4].data.ival;
    gp->sdata[pnt].minor_revision=data[5].data.ival;
    gp->sdata[pnt].prog_id=data[6].data.ival;
    gp->sdata[pnt].noise.mean=data[7].data.fval;
    gp->sdata[pnt].noise.sd=data[8].data.fval;
    gp->sdata[pnt].gsct=data[9].data.ival;
    gp->sdata[pnt].vel.min=data[10].data.fval;
    gp->sdata[pnt].vel.max=data[11].data.fval;
    gp->sdata[pnt].pwr.min=data[12].data.fval;
    gp->sdata[pnt].pwr.max=data[13].data.fval;
    gp->sdata[pnt].wdt.min=data[14].data.fval;
    gp->sdata[pnt].wdt.max=data[15].data.fval;
    gp->sdata[pnt].verr.min=data[16].data.fval;
    gp->sdata[pnt].verr.max=data[17].data.fval;
   
  }
  return 1;
}




int OldGridDecodeThree(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct GridData *gp;
  gp=(struct GridData *)ptr;

  if (strcmp(name,BLOCK3) !=0) return 0;
 
  if (pnt==0) {
    
    if (npnt>0) {
      if (gp->data !=NULL) gp->data=realloc(gp->data,
        sizeof(struct GridGVec)*npnt);
      else gp->data=malloc( sizeof(struct GridGVec)*npnt);
    } else {
      free(gp->data);
      gp->data=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->vcnum=npnt;
    gp->xtd=0;
  }
  if (npnt>0) {
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].srng=0;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].chn=0;
    gp->data[pnt].index=data[4].data.ival;
    gp->data[pnt].vel.median=data[5].data.fval;
    gp->data[pnt].vel.sd=data[6].data.fval;
  }
  return 1;
}


int OldGridDecodeFour(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct GridData *gp;
  gp=(struct GridData *)ptr;


  if (strcmp(name,BLOCK4) !=0) return 0;
 
  if (pnt==0) {
    
    if (npnt>0) {
      if (gp->data !=NULL) gp->data=realloc(gp->data,
        sizeof(struct GridGVec)*npnt);
      else gp->data=malloc( sizeof(struct GridGVec)*npnt);
    } else {
      free(gp->data);
      gp->data=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->vcnum=npnt;
    gp->xtd=1;
  }
  if (npnt>0) {
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].srng=0;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].chn=0;
    gp->data[pnt].index=data[4].data.ival;
    gp->data[pnt].vel.median=data[5].data.fval;
    gp->data[pnt].vel.sd=data[6].data.fval;
    gp->data[pnt].pwr.median=data[7].data.fval;
    gp->data[pnt].pwr.sd=data[8].data.fval;
    gp->data[pnt].wdt.median=data[9].data.fval;
    gp->data[pnt].wdt.sd=data[10].data.fval;
  }
  return 1;
}




int OldGridDecodeFive(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct GridData *gp;
  gp=(struct GridData *)ptr;

  if (strcmp(name,BLOCK5) !=0) return 0;
 
  if (pnt==0) {
    
    if (npnt>0) {
      if (gp->data !=NULL) gp->data=realloc(gp->data,
        sizeof(struct GridGVec)*npnt);
      else gp->data=malloc( sizeof(struct GridGVec)*npnt);
    } else {
      free(gp->data);
      gp->data=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->vcnum=npnt;
  }
  if (npnt>0) {
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].srng=0;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].chn=data[4].data.ival;

    gp->data[pnt].index=data[5].data.ival;
    gp->data[pnt].vel.median=data[6].data.fval;
    gp->data[pnt].vel.sd=data[7].data.fval;
  }
  return 1;
}



int OldGridDecodeSix(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct GridData *gp;
  gp=(struct GridData *)ptr;

  if (strcmp(name,BLOCK6) !=0) return 0;
 
  if (pnt==0) {
    
    if (npnt>0) {
      if (gp->data !=NULL) gp->data=realloc(gp->data,
        sizeof(struct GridGVec)*npnt);
      else gp->data=malloc( sizeof(struct GridGVec)*npnt);
    } else {
      free(gp->data);
      gp->data=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->vcnum=npnt;
    gp->xtd=1;
    
  }
  if (npnt>0) {
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].srng=0;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].chn=data[4].data.ival;

    gp->data[pnt].index=data[5].data.ival;
    gp->data[pnt].vel.median=data[6].data.fval;
    gp->data[pnt].vel.sd=data[7].data.fval;
    gp->data[pnt].pwr.median=data[8].data.fval;
    gp->data[pnt].pwr.sd=data[9].data.fval;
    gp->data[pnt].wdt.median=data[10].data.fval;
    gp->data[pnt].wdt.sd=data[11].data.fval;
  }
  return 1;
}




int OldGridFread(FILE *fp,struct GridData *ptr) {

  RfileDecoder decode[6];
  void *dptr[6];

  decode[0]=OldGridDecodeOne;
  decode[1]=OldGridDecodeTwo;
  decode[2]=OldGridDecodeThree;

  decode[3]=OldGridDecodeFour;
  decode[4]=OldGridDecodeFive;
  decode[5]=OldGridDecodeSix;


  dptr[0]=ptr;
  dptr[1]=ptr;
  dptr[2]=ptr;
  dptr[3]=ptr;
  dptr[4]=ptr;
  dptr[5]=ptr;

  return RfileRead(fp,6,decode,dptr);
}

int OldGridFseek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,
              struct RfileIndex *inx,double *aval) {

  return RfileSeek(fp,yr,mo,dy,hr,mt,sc,inx,aval);
}


















