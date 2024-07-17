/* readmap.c
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
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"
#include "oldgridread.h"
#include "cnvmap.h"





#define BLOCK1 "gmlong gmlat kvect vlos"
#define BLOCK2 "bnd_lon bnd_lat"
#define BLOCK3 "mlt_start mlt_end mlt_avg"
#define BLOCK4o "chi_sqr rms_err"
#define BLOCK4 "chi_sqr chi_sqr_dat rms_err"

#define BLOCK5 "pot pot_err"
#define BLOCK6 "Bx By Bz"
#define BLOCK7 "lon_shft lat_shft hemisphere order latmin error_wt model_wt"
#define BLOCK8 "N N+1 N+2 N+3"
#define BLOCK9 "doping_level imf_flag imf_delay mod_dir mod_mag"
#define BLOCK10 "source major_rev minor_rev"

int OldCnvMapDecodeOne(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;

  if (strcmp(name,BLOCK1) !=0) return 0;

  if (pnt==0) {
    if (npnt>0) {
      if (mp->model !=NULL) mp->model=realloc(mp->model,
                               sizeof(struct GridGVec)*npnt);
      else mp->model=malloc( sizeof(struct GridGVec)*npnt);
    } else {
      free(mp->model);
      mp->model=NULL;
    }
    mp->st_time=st_time;
    mp->ed_time=ed_time;
    mp->num_model=npnt;
  }

  if (npnt>0) {
    mp->model[pnt].mlat=data[1].data.fval;
    mp->model[pnt].mlon=data[0].data.fval;
    mp->model[pnt].azm=data[2].data.fval;
    mp->model[pnt].srng=0;
    mp->model[pnt].vel.median=data[3].data.fval;
    mp->model[pnt].vel.sd=0;
    mp->model[pnt].pwr.median=0;
    mp->model[pnt].pwr.sd=0;
    mp->model[pnt].wdt.median=0;
    mp->model[pnt].wdt.sd=0;
    mp->model[pnt].st_id=0;
    mp->model[pnt].chn=0;
    mp->model[pnt].index=0;
  }
  return 1;
}


int OldCnvMapDecodeTwo(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;

  if (strcmp(name,BLOCK2) !=0) return 0;

  if (pnt==0) {
    if (npnt>0) {
      if (mp->bnd_lon !=NULL) mp->bnd_lon=realloc(mp->bnd_lon,
                               sizeof(double)*npnt);
      else mp->bnd_lon=malloc( sizeof(double)*npnt);
      if (mp->bnd_lat !=NULL) mp->bnd_lat=realloc(mp->bnd_lat,
                               sizeof(double)*npnt);
      else mp->bnd_lat=malloc( sizeof(double)*npnt);
    } else {
      free(mp->bnd_lon);
      mp->bnd_lon=NULL;
      free(mp->bnd_lat);
      mp->bnd_lat=NULL;
    }
    mp->st_time=st_time;
    mp->ed_time=ed_time;
    mp->num_bnd=npnt;
  }

  if (npnt>0) {
    mp->bnd_lat[pnt]=data[1].data.fval;
    mp->bnd_lon[pnt]=data[0].data.fval;
  }
  return 1;
}


int OldCnvMapDecodeThree(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK3) !=0) return 0;

  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }

  if (npnt>0) {
    mp->mlt.start=data[0].data.fval;
    mp->mlt.end=data[1].data.fval;
    mp->mlt.av=data[2].data.fval;
  }
  return 1;
}

int OldCnvMapDecodeFour(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK4) !=0) return 0;

  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }

  if (npnt>0) {
    mp->chi_sqr=data[0].data.fval;
    mp->chi_sqr_dat=data[1].data.fval;
    mp->rms_err=data[2].data.fval;
  }
  return 1;
}


int OldCnvMapDecodeFourOld(char *name,char *unit,char *type,
                   double st_time,double ed_time,
                   int npnt,int nprm,int pnt,
                   struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK4o) !=0) return 0;

  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }

  if (npnt>0) {
    mp->chi_sqr=0;
    mp->chi_sqr_dat=data[0].data.fval;
    mp->rms_err=data[1].data.fval;
  }
  return 1;

}

int OldCnvMapDecodeFive(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK5) !=0) return 0;

  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }

  if (npnt>0) {
    if (pnt==0) mp->pot_drop=data[0].data.fval;
    if (pnt==1) mp->pot_max=data[0].data.fval;
    if (pnt==2) mp->pot_min=data[0].data.fval;
    if (pnt==0) mp->pot_drop_err=data[1].data.fval;
    if (pnt==1) mp->pot_max_err=data[1].data.fval;
    if (pnt==2) mp->pot_min_err=data[1].data.fval;
  }
  return 1;
}



int OldCnvMapDecodeSix(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK6) !=0) return 0;

  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }

  if (npnt>0) {
    mp->Bx=data[0].data.fval;
    mp->By=data[1].data.fval;
    mp->Bz=data[2].data.fval;
  }
  return 1;
}


int OldCnvMapDecodeSeven(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK7) !=0) return 0;

  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }

  if (npnt>0) {
    mp->lon_shft=data[0].data.fval;
    mp->lat_shft=data[1].data.fval;
    mp->hemisphere=data[2].data.ival;
    mp->fit_order=data[3].data.ival;
    mp->latmin=data[4].data.fval;
    mp->error_wt=data[5].data.ival;
    mp->model_wt=data[6].data.ival;
  }
  return 1;
}

int OldCnvMapDecodeEight(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;

  if (strcmp(name,BLOCK8) !=0) return 0;

  if (pnt==0) {
    if (npnt>0) {
      if (mp->coef !=NULL) mp->coef=realloc(mp->coef,
                               sizeof(double)*4*npnt);
      else mp->coef=malloc( sizeof(double)*4*npnt);
    } else {
      free(mp->coef);
      mp->coef=NULL;
    }
    mp->st_time=st_time;
    mp->ed_time=ed_time;
    mp->num_coef=npnt;
  }

  if (npnt>0) {
    mp->coef[4*pnt]=data[0].data.fval;
    mp->coef[4*pnt+1]=data[1].data.fval;
    mp->coef[4*pnt+2]=data[2].data.fval;
    mp->coef[4*pnt+3]=data[3].data.fval;
  }
  return 1;
}

int OldCnvMapDecodeNine(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {

  struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK9) !=0) return 0;

  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }

  if (npnt>0) {
    mp->doping_level=data[0].data.ival;
    mp->imf_flag=data[1].data.ival;
    mp->imf_delay=data[2].data.ival;
    strcpy(mp->imf_model[0],data[3].data.tval);
    strcpy(mp->imf_model[1],data[4].data.tval);

  }
  return 1;
}


int OldCnvMapDecodeTen(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr) {


 struct CnvMapData *mp;
  mp=(struct CnvMapData *)ptr;
  if (strcmp(name,BLOCK10) !=0) return 0;


  if (pnt==0) {
    mp->st_time=st_time;
    mp->ed_time=ed_time;
  }


  if (npnt>0) {
    strcpy(mp->source,data[0].data.tval);
    mp->major_rev=data[1].data.ival;
    mp->minor_rev=data[2].data.ival;
  }
  return 1;

}


int OldCnvMapFread(FILE *fp,struct CnvMapData *map,struct GridData *grd) {

  RfileDecoder decode[17];
  void *dptr[17];

  decode[0]=OldGridDecodeOne;
  decode[1]=OldGridDecodeTwo;
  decode[2]=OldGridDecodeThree;
  decode[3]=OldGridDecodeFour;
  decode[4]=OldGridDecodeFive;
  decode[5]=OldGridDecodeSix;

  decode[6]=OldCnvMapDecodeOne;
  decode[7]=OldCnvMapDecodeTwo;
  decode[8]=OldCnvMapDecodeThree;
  decode[9]=OldCnvMapDecodeFour;
  decode[10]=OldCnvMapDecodeFourOld;

  decode[11]=OldCnvMapDecodeFive;
  decode[12]=OldCnvMapDecodeSix;
  decode[13]=OldCnvMapDecodeSeven;
  decode[14]=OldCnvMapDecodeEight;
  decode[15]=OldCnvMapDecodeNine;
  decode[16]=OldCnvMapDecodeTen;


  dptr[0]=grd;
  dptr[1]=grd;
  dptr[2]=grd;
  dptr[3]=grd;
  dptr[4]=grd;
  dptr[5]=grd;

  dptr[6]=map;
  dptr[7]=map;
  dptr[8]=map;
  dptr[9]=map;
  dptr[10]=map;
  dptr[11]=map;
  dptr[12]=map;
  dptr[13]=map;
  dptr[14]=map;
  dptr[15]=map;
  dptr[16]=map;

  return RfileRead(fp,17,decode,dptr);

}

int OldCnvMapFseek(FILE *fp,
	        int yr,int mo,int dy,int hr,int mt,int sc,
                struct RfileIndex *inx,double *aval) {
  return RfileSeek(fp,yr,mo,dy,hr,mt,sc,inx,aval);
}















