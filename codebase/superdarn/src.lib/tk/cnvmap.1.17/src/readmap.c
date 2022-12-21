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
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "griddata.h"
#include "cnvmap.h"

int CnvMapRead(int fid,struct CnvMapData *map,struct GridData *grd) {
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
                 "end.minute","end.second",
                 "map.major.revision","map.minor.revision",
                 "source",
                 "doping.level",
                 "model.wt",
                 "error.wt",
                 "IMF.flag",
                 "IMF.delay",
                 "IMF.Bx",
                 "IMF.By",
                 "IMF.Bz",
                 "IMF.Vx",        /* SGS */
                 "IMF.tilt",      /* SGS */
                 "IMF.Kp",        /* EGT */
                 "model.angle",
                 "model.level",
                 "model.tilt",    /* SGS */
                 "model.name",    /* SGS */
                 "hemisphere",
                 "noigrf",        /* SGS */
                 "fit.order",
                 "latmin",
                 "chi.sqr",
                 "chi.sqr.dat",
                 "rms.err",
                 "lon.shft",
                 "lat.shft",
                 "mlt.start",
                 "mlt.end",
                 "mlt.av",
                 "pot.drop",
                 "pot.drop.err",
                 "pot.max",
                 "pot.max.err",
                 "pot.min",
                 "pot.min.err",


                 0};

  int stype[]={DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATADOUBLE,
               DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATADOUBLE,
               DATASHORT,DATASHORT,
               DATASTRING,
               DATASHORT,
               DATASHORT,
               DATASHORT,
               DATASHORT,
               DATASHORT,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,  /* SGS */
               DATADOUBLE,  /* SGS */
               DATADOUBLE,  /* EGT */
               DATASTRING,
               DATASTRING,
               DATASTRING,  /* SGS */
               DATASTRING,  /* SGS */
               DATASHORT,
               DATASHORT,   /* SGS */
               DATASHORT,
               DATAFLOAT,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATAFLOAT,
               DATAFLOAT,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               DATADOUBLE,
               0
              };

  struct DataMapScalar *sdata[48];

  char *aname[]={"stid","channel","nvec",
                 "freq","major.revision","minor.revision",
                 "program.id","noise.mean","noise.sd","gsct",
                 "v.min","v.max","p.min","p.max","w.min","w.max","ve.min",
                 "ve.max",
                 "vector.mlat","vector.mlon","vector.kvect",
                 "vector.stid","vector.channel","vector.index",
                 "vector.vel.median","vector.vel.sd",
                 "vector.pwr.median","vector.pwr.sd",
                 "vector.wdt.median","vector.wdt.sd",
                 "N","N+1","N+2","N+3",
                 "model.mlat","model.mlon","model.kvect","model.vel.median",
                 "boundary.mlat","boundary.mlon",
                 0};

  int atype[]={DATASHORT,DATASHORT,DATASHORT,
               DATAFLOAT,DATASHORT,DATASHORT,
               DATASHORT,DATAFLOAT,DATAFLOAT,DATASHORT,
               DATAFLOAT,DATAFLOAT,DATAFLOAT,DATAFLOAT,DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT,DATAFLOAT,
               DATASHORT,DATASHORT,DATAINT,
               DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT,
               DATADOUBLE,DATADOUBLE,DATADOUBLE,DATADOUBLE,
               DATAFLOAT,DATAFLOAT,DATAFLOAT,DATAFLOAT,
               DATAFLOAT,DATAFLOAT};

  struct DataMapArray *adata[50];

  ptr=DataMapReadBlock(fid,&size);

  if (ptr==NULL) return -1;

  for (c=0;sname[c] !=0;c++) sdata[c]=NULL;
  for (c=0;aname[c] !=0;c++) adata[c]=NULL;

  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    for (x=0;sname[x] !=0;x++)
      if ((strcmp(s->name,sname[x])==0) && (s->type==stype[x])) {
        sdata[x]=s;
        if (s == NULL) break;   /* SGS */
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

  for (x=0;sname[x] !=0;x++) {
    if (x==14) continue;
    if (x==23) continue;
    if (x==24) continue;
    if (x==25) continue;  /* EGT */
    if (x==26) continue;  /* SGS */
    if (x==27) continue;
    if (x==28) continue;
    if (x==29) continue;  /* SGS */
    if (x==31) continue;
    if (sdata[x]==NULL) break;
  }

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
  grd->st_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  map->st_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  yr=*(sdata[6]->data.sptr);
  mo=*(sdata[7]->data.sptr);
  dy=*(sdata[8]->data.sptr);
  hr=*(sdata[9]->data.sptr);
  mt=*(sdata[10]->data.sptr);
  sc=*(sdata[11]->data.dptr);
  grd->ed_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  map->ed_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  map->major_rev=*(sdata[12]->data.sptr);
  map->minor_rev=*(sdata[13]->data.sptr);

  if (sdata[14] !=NULL)
    strncpy(map->source,*((char **) sdata[14]->data.vptr),256);

  map->doping_level=*(sdata[15]->data.sptr);
  map->model_wt=*(sdata[16]->data.sptr);
  map->error_wt=*(sdata[17]->data.sptr);
  map->imf_flag=*(sdata[18]->data.sptr);
  map->imf_delay=*(sdata[19]->data.sptr);

  map->Bx=*(sdata[20]->data.dptr);
  map->By=*(sdata[21]->data.dptr);
  map->Bz=*(sdata[22]->data.dptr);
  if (sdata[23] !=NULL)
    map->Vx=*(sdata[23]->data.dptr);
  if (sdata[24] !=NULL)
    map->tilt=*(sdata[24]->data.dptr);
  if (sdata[25] !=NULL)
    map->Kp=*(sdata[25]->data.dptr);

  if (sdata[26] !=NULL)
    strncpy(map->imf_model[0],*((char **) sdata[26]->data.vptr),64);
  if (sdata[27] !=NULL)
    strncpy(map->imf_model[1],*((char **) sdata[27]->data.vptr),64);
  if (sdata[28] !=NULL)
    strncpy(map->imf_model[2],*((char **) sdata[28]->data.vptr),64);
  if (sdata[29] !=NULL)
    strncpy(map->imf_model[3],*((char **) sdata[29]->data.vptr),64);

  map->hemisphere=*(sdata[30]->data.sptr);
  if (sdata[31] !=NULL)
    map->noigrf=*(sdata[31]->data.sptr);
  map->fit_order=*(sdata[32]->data.sptr);
  map->latmin=*(sdata[33]->data.fptr);

  map->chi_sqr=*(sdata[34]->data.dptr);
  map->chi_sqr_dat=*(sdata[35]->data.dptr);
  map->rms_err=*(sdata[36]->data.dptr);

  map->lat_shft=*(sdata[37]->data.fptr);
  map->lon_shft=*(sdata[38]->data.fptr);

  map->mlt.start=*(sdata[39]->data.dptr);
  map->mlt.end=*(sdata[40]->data.dptr);
  map->mlt.av=*(sdata[41]->data.dptr);

  map->pot_drop=*(sdata[42]->data.dptr);
  map->pot_drop_err=*(sdata[43]->data.dptr);

  map->pot_max=*(sdata[44]->data.dptr);
  map->pot_max_err=*(sdata[45]->data.dptr);

  map->pot_min=*(sdata[46]->data.dptr);
  map->pot_min_err=*(sdata[47]->data.dptr);

  grd->stnum=adata[0]->rng[0];
  if (grd->stnum==0) {
    DataMapFree(ptr);
    return -1;
  }

  if (grd->sdata !=NULL) {
    tmp=realloc(grd->sdata,sizeof(struct GridSVec)*grd->stnum);
    if (tmp==NULL) {
      DataMapFree(ptr);
      return -1;
    }
    grd->sdata=tmp;
  } else grd->sdata=malloc(sizeof(struct GridSVec)*grd->stnum);

  if (grd->sdata==NULL) {
     DataMapFree(ptr);
     return -1;
  }

  for (n=0;n<grd->stnum;n++) {
    grd->sdata[n].st_id=adata[0]->data.sptr[n];
    grd->sdata[n].chn=adata[1]->data.sptr[n];
    grd->sdata[n].npnt=adata[2]->data.sptr[n];
    grd->sdata[n].freq0=adata[3]->data.fptr[n];
    grd->sdata[n].major_revision=adata[4]->data.sptr[n];
    grd->sdata[n].minor_revision=adata[5]->data.sptr[n];
    grd->sdata[n].prog_id=adata[6]->data.sptr[n];
    grd->sdata[n].noise.mean=adata[7]->data.fptr[n];
    grd->sdata[n].noise.sd=adata[8]->data.fptr[n];
    grd->sdata[n].gsct=adata[9]->data.sptr[n];
    grd->sdata[n].vel.min=adata[10]->data.fptr[n];
    grd->sdata[n].vel.max=adata[11]->data.fptr[n];
    grd->sdata[n].pwr.min=adata[12]->data.fptr[n];
    grd->sdata[n].pwr.max=adata[13]->data.fptr[n];
    grd->sdata[n].wdt.min=adata[14]->data.fptr[n];
    grd->sdata[n].wdt.max=adata[15]->data.fptr[n];
    grd->sdata[n].verr.min=adata[16]->data.fptr[n];
    grd->sdata[n].verr.max=adata[17]->data.fptr[n];
  }

  if (adata[18] !=NULL) {
    grd->vcnum=adata[18]->rng[0];

    if (grd->data !=NULL) {
      tmp=realloc(grd->data,sizeof(struct GridGVec)*grd->vcnum);
      if (tmp==NULL) {
        DataMapFree(ptr);
        return -1;
      }
      grd->data=tmp;
    }   else grd->data=malloc(sizeof(struct GridGVec)*grd->vcnum);
    if (grd->data==NULL) {
      DataMapFree(ptr);
      return -1;
    }
  } else {
    grd->vcnum=0;
    if (grd->data !=NULL) free(grd->data);
    grd->data=NULL;
  }

  grd->xtd=0;

  if (grd->data !=NULL) {
    for (n=26;n<30;n++) if (adata[n] !=NULL) {
      grd->xtd=1;
      break;
    }
    for (n=0;n<grd->vcnum;n++) {
      grd->data[n].mlat=adata[18]->data.fptr[n];
      grd->data[n].mlon=adata[19]->data.fptr[n];
      grd->data[n].azm=adata[20]->data.fptr[n];

      grd->data[n].st_id=adata[21]->data.sptr[n];
      grd->data[n].chn=adata[22]->data.sptr[n];
      grd->data[n].index=adata[23]->data.iptr[n];
      grd->data[n].vel.median=adata[24]->data.fptr[n];
      grd->data[n].vel.sd=adata[25]->data.fptr[n];
      grd->data[n].pwr.median=0;
      grd->data[n].pwr.sd=0;
      grd->data[n].wdt.median=0;
      grd->data[n].wdt.sd=0;

      if (adata[26] !=NULL) grd->data[n].pwr.median=adata[26]->data.fptr[n];
      if (adata[27] !=NULL) grd->data[n].pwr.sd=adata[27]->data.fptr[n];
      if (adata[28] !=NULL) grd->data[n].wdt.median=adata[28]->data.fptr[n];
      if (adata[29] !=NULL) grd->data[n].wdt.sd=adata[29]->data.fptr[n];
    }
  }

  if (adata[30] !=NULL) {
   map->num_coef=adata[30]->rng[0];
   if (map->coef !=NULL) {
      tmp=realloc(map->coef,sizeof(double)*4*map->num_coef);
      if (tmp==NULL) {
        DataMapFree(ptr);
        return -1;
      }
      map->coef=tmp;
    } else map->coef=malloc(sizeof(double)*4*map->num_coef);
    if (map->coef==NULL) {
      DataMapFree(ptr);
      return -1;
    }
  } else {
    map->num_coef=0;
    if (map->coef!=NULL) free(map->coef);
    map->coef=NULL;
  }
  if (map->coef !=NULL) {
    for (n=0;n<map->num_coef;n++) {
       map->coef[4*n]=adata[30]->data.dptr[n];
       map->coef[4*n+1]=adata[31]->data.dptr[n];
       map->coef[4*n+2]=adata[32]->data.dptr[n];
       map->coef[4*n+3]=adata[33]->data.dptr[n];
    }
  }


  if (adata[34] !=NULL) {
   map->num_model=adata[34]->rng[0];
   if (map->model !=NULL) {
      tmp=realloc(map->model,sizeof(struct GridGVec)*map->num_model);
      if (tmp==NULL) {
        DataMapFree(ptr);
        return -1;
      }
      map->model=tmp;
    } else map->model=malloc(sizeof(struct GridGVec)*map->num_model);
    if (map->model==NULL) {
      DataMapFree(ptr);
      return -1;
    }
  } else {
    map->num_model=0;
    if (map->model!=NULL) free(map->model);
    map->model=NULL;
  }
  if (map->model !=NULL) {
    for (n=0;n<map->num_model;n++) {
      map->model[n].mlat=adata[34]->data.fptr[n];
      map->model[n].mlon=adata[35]->data.fptr[n];
      map->model[n].azm=adata[36]->data.fptr[n];
      map->model[n].vel.median=adata[37]->data.fptr[n];
      map->model[n].vel.sd=0;
      map->model[n].pwr.median=0;
      map->model[n].pwr.sd=0;
      map->model[n].wdt.median=0;
      map->model[n].wdt.sd=0;
      map->model[n].st_id=0;
      map->model[n].chn=0;
      map->model[n].index=0;
    }
  }


  if (adata[38] !=NULL) {

    map->num_bnd=adata[38]->rng[0];
    if (map->bnd_lat !=NULL) {
       tmp=realloc(map->bnd_lat,sizeof(double)*map->num_bnd);
       if (tmp==NULL) {
         DataMapFree(ptr);
         return -1;
       }
       map->bnd_lat=tmp;
     } else map->bnd_lat=malloc(sizeof(double)*map->num_bnd);
     if (map->bnd_lat==NULL) {
       DataMapFree(ptr);
       return -1;
     }
     if (map->bnd_lon !=NULL) {
       tmp=realloc(map->bnd_lon,sizeof(double)*map->num_bnd);
       if (tmp==NULL) {
         DataMapFree(ptr);
         return -1;
       }
       map->bnd_lon=tmp;
     } else map->bnd_lon=malloc(sizeof(double)*map->num_bnd);
     if (map->bnd_lon==NULL) {
       DataMapFree(ptr);
       return -1;
     }
   } else {
     map->num_bnd=0;
     if (map->bnd_lat !=NULL) free(map->bnd_lat);
     if (map->bnd_lon !=NULL) free(map->bnd_lon);
     map->bnd_lat=NULL;
     map->bnd_lon=NULL;
   }
   if (map->bnd_lat !=NULL) {
     for (n=0;n<map->num_bnd;n++) {
        map->bnd_lat[n]=adata[38]->data.fptr[n];
        map->bnd_lon[n]=adata[39]->data.fptr[n];
     }
  }

  DataMapFree(ptr);

  return size;
}

int CnvMapFread(FILE *fp,struct CnvMapData *map,struct GridData *grd) {
  return CnvMapRead(fileno(fp),map,grd);
}

