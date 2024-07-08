/* writemap.c
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
#include "cnvmap.h"




int CnvMapWrite(int fid,struct CnvMapData *map,struct GridData *grd) {
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

  int16 map_major,map_minor;
  int16 doping_level;
  int16 model_wt;
  int16 error_wt;
  int16 imf_flag;
  int16 imf_delay;
  int16 hemisphere;
  int16 noigrf;       /* SGS */
  int16 fit_order;
  float latmin;

  float lon_shft;
  float lat_shft;
 
  char *src=NULL;
  char *mod_name=NULL;  /* SGS */
  char *mod_ang=NULL;
  char *mod_lev=NULL;
  char *mod_tilt=NULL;  /* SGS */

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

  int32 num_coef=0;
  int32 num_model=0;
  int32 num_bnd=0;

  double *n0=NULL;
  double *n1=NULL;
  double *n2=NULL;
  double *n3=NULL;

  float *mgmlon=NULL;
  float *mgmlat=NULL;
  float *mkvect=NULL;
  float *mvlos=NULL;

  float *bnd_lat=NULL;
  float *bnd_lon=NULL;

  int size=0;
  unsigned char *buf=NULL;
  unsigned char *bptr=NULL;

  int xtd=0;
  int n,p;

  for (n=0;n<grd->stnum;n++) if (grd->sdata[n].st_id !=-1) stnum++;
  if (stnum==0) return 0;

  size=stnum*(11*sizeof(float)+7*sizeof(int16));

  xtd=grd->xtd;

  for (n=0;n<grd->vcnum;n++) if (grd->data[n].st_id !=-1) npnt++;

  size+=npnt*(4*sizeof(float)+sizeof(int32)*2*sizeof(int16)+
                      (2+4*xtd)*sizeof(float));

  size+=map->num_coef*4*sizeof(double);
  size+=map->num_bnd*2*sizeof(float);
  size+=map->num_model*4*sizeof(float);
  
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
  for (p=0;p<grd->stnum;p++) if (grd->sdata[p].st_id !=-1) {
     stid[n]=grd->sdata[p].st_id;
     chn[n]=grd->sdata[p].chn;
     nvec[n]=grd->sdata[p].npnt;
     freq[n]=grd->sdata[p].freq0;
     major_rev[n]=grd->sdata[p].major_revision;
     minor_rev[n]=grd->sdata[p].minor_revision;
     progid[n]=grd->sdata[p].prog_id;
     noise_mean[n]=grd->sdata[p].noise.mean;
     noise_sd[n]=grd->sdata[p].noise.sd;
     gsct[n]=grd->sdata[p].gsct;
     v_min[n]=grd->sdata[p].vel.min;
     v_max[n]=grd->sdata[p].vel.max;
     p_min[n]=grd->sdata[p].pwr.min;
     p_max[n]=grd->sdata[p].pwr.max;
     w_min[n]=grd->sdata[p].wdt.min;
     w_max[n]=grd->sdata[p].wdt.max;
     ve_min[n]=grd->sdata[p].verr.min;
     ve_max[n]=grd->sdata[p].verr.max;
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
    for (p=0;p<grd->vcnum;p++) if (grd->data[p].st_id !=-1) {
      gmlat[n]=grd->data[p].mlat;
      gmlon[n]=grd->data[p].mlon;
      kvect[n]=grd->data[p].azm;
      srng[n]=grd->data[p].srng;
      vstid[n]=grd->data[p].st_id;
      vchn[n]=grd->data[p].chn;
      index[n]=grd->data[p].index;
      vlos[n]=grd->data[p].vel.median;
      vlos_sd[n]=grd->data[p].vel.sd;
      if (xtd) {
        pwr[n]=grd->data[p].pwr.median;
        pwr_sd[n]=grd->data[p].pwr.sd;
        wdt[n]=grd->data[p].wdt.median;
        wdt_sd[n]=grd->data[p].wdt.sd;
      }
      n++;
    }
  }

  if (map->num_coef !=0) {
    num_coef=map->num_coef;
    n0=(double *) bptr;
    bptr+=num_coef*sizeof(double);
    n1=(double *) bptr;
    bptr+=num_coef*sizeof(double);
    n2=(double *) bptr;
    bptr+=num_coef*sizeof(double);
    n3=(double *) bptr;
    bptr+=num_coef*sizeof(double);
    for (n=0;n<num_coef;n++) {
      n0[n]=map->coef[4*n];
      n1[n]=map->coef[4*n+1];
      n2[n]=map->coef[4*n+2];
      n3[n]=map->coef[4*n+3];
    }
  }


  if (map->num_model !=0) {
    num_model=map->num_model;
    mgmlon=(float *) bptr;
    bptr+=num_model*sizeof(float);
    mgmlat=(float *) bptr;
    bptr+=num_model*sizeof(float);
    mkvect=(float *) bptr;
    bptr+=num_model*sizeof(float);
    mvlos=(float *) bptr;
    bptr+=num_model*sizeof(float);

    for (n=0;n<num_model;n++) {
      mgmlat[n]=map->model[n].mlat;
      mgmlon[n]=map->model[n].mlon;
      mkvect[n]=map->model[n].azm;
      mvlos[n]=map->model[n].vel.median;
    }
  }

  if (map->num_bnd !=0) {
    num_bnd=map->num_bnd;
    bnd_lon=(float *) bptr;
    bptr+=num_bnd*sizeof(float);
    bnd_lat=(float *) bptr;
    bptr+=num_bnd*sizeof(float);

    for (n=0;n<num_bnd;n++) {
      bnd_lat[n]=map->bnd_lat[n];
      bnd_lon[n]=map->bnd_lon[n];
    }

  }


  TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
  eyr=yr;
  emo=mo;
  edy=dy;
  ehr=hr;
  emt=mt;
  esc=sc;
  TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
  syr=yr;
  smo=mo;
  sdy=dy;
  shr=hr;
  smt=mt;
  ssc=sc;

  map_major    = map->major_rev;
  map_minor    = map->minor_rev;
  doping_level = map->doping_level;
  model_wt     = map->model_wt;
  error_wt     = map->error_wt;
  imf_flag     = map->imf_flag;
  imf_delay    = map->imf_delay;
 
  latmin       = map->latmin;

  hemisphere   = map->hemisphere;
  noigrf       = map->noigrf;
  fit_order    = map->fit_order;
  
  lat_shft=map->lat_shft;
  lon_shft=map->lon_shft;

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

  DataMapAddScalar(data,"map.major.revision",DATASHORT,&map_major);
  DataMapAddScalar(data,"map.minor.revision",DATASHORT,&map_minor);

  if (strlen(map->source) !=0) {
    src=map->source;
    DataMapAddScalar(data,"source",DATASTRING,&src);
  }

  DataMapAddScalar(data,"doping.level",DATASHORT,&doping_level);
  DataMapAddScalar(data,"model.wt",DATASHORT,&model_wt);
  DataMapAddScalar(data,"error.wt",DATASHORT,&error_wt);
  DataMapAddScalar(data,"IMF.flag",DATASHORT,&imf_flag);
  DataMapAddScalar(data,"IMF.delay",DATASHORT,&imf_delay);
  DataMapAddScalar(data,"IMF.Bx",DATADOUBLE,&map->Bx);
  DataMapAddScalar(data,"IMF.By",DATADOUBLE,&map->By);
  DataMapAddScalar(data,"IMF.Bz",DATADOUBLE,&map->Bz);
  DataMapAddScalar(data,"IMF.Vx",DATADOUBLE,&map->Vx);    /* SGS */
  DataMapAddScalar(data,"IMF.tilt",DATADOUBLE,&map->tilt);/* SGS */
  DataMapAddScalar(data,"IMF.Kp",DATADOUBLE,&map->Kp);    /* EGT */
 
  if (strlen(map->imf_model[0]) !=0) {
    mod_ang  = map->imf_model[0];
    mod_lev  = map->imf_model[1];
    mod_tilt = map->imf_model[2]; /* SGS */
    mod_name = map->imf_model[3]; /* SGS */
    DataMapAddScalar(data,"model.angle",DATASTRING,&mod_ang);
    DataMapAddScalar(data,"model.level",DATASTRING,&mod_lev);
    DataMapAddScalar(data,"model.tilt",DATASTRING,&mod_tilt); /* SGS */
    DataMapAddScalar(data,"model.name",DATASTRING,&mod_name); /* SGS */
  }
    

  DataMapAddScalar(data,"hemisphere",DATASHORT,&hemisphere);
  DataMapAddScalar(data,"noigrf",DATASHORT,&noigrf);/* SGS */
  DataMapAddScalar(data,"fit.order",DATASHORT,&fit_order);
  DataMapAddScalar(data,"latmin",DATAFLOAT,&latmin);
  DataMapAddScalar(data,"chi.sqr",DATADOUBLE,&map->chi_sqr);
  DataMapAddScalar(data,"chi.sqr.dat",DATADOUBLE,&map->chi_sqr_dat);
  DataMapAddScalar(data,"rms.err",DATADOUBLE,&map->rms_err);
  DataMapAddScalar(data,"lon.shft",DATAFLOAT,&lon_shft);
  DataMapAddScalar(data,"lat.shft",DATAFLOAT,&lat_shft);

  DataMapAddScalar(data,"mlt.start",DATADOUBLE,&map->mlt.start);
  DataMapAddScalar(data,"mlt.end",DATADOUBLE,&map->mlt.end);
  DataMapAddScalar(data,"mlt.av",DATADOUBLE,&map->mlt.av);

  DataMapAddScalar(data,"pot.drop",DATADOUBLE,&map->pot_drop);
  DataMapAddScalar(data,"pot.drop.err",DATADOUBLE,&map->pot_drop_err);

  DataMapAddScalar(data,"pot.max",DATADOUBLE,&map->pot_max);
  DataMapAddScalar(data,"pot.max.err",DATADOUBLE,&map->pot_max_err);

  DataMapAddScalar(data,"pot.min",DATADOUBLE,&map->pot_min);
  DataMapAddScalar(data,"pot.min.err",DATADOUBLE,&map->pot_min_err);

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
  if (num_coef !=0) {
    DataMapAddArray(data,"N",DATADOUBLE,1,&num_coef,n0);
    DataMapAddArray(data,"N+1",DATADOUBLE,1,&num_coef,n1);
    DataMapAddArray(data,"N+2",DATADOUBLE,1,&num_coef,n2);
    DataMapAddArray(data,"N+3",DATADOUBLE,1,&num_coef,n3);
  }

  if (num_model !=0) {
    DataMapAddArray(data,"model.mlat",DATAFLOAT,1,&num_model,mgmlat);
    DataMapAddArray(data,"model.mlon",DATAFLOAT,1,&num_model,mgmlon);
    DataMapAddArray(data,"model.kvect",DATAFLOAT,1,&num_model,mkvect);
    DataMapAddArray(data,"model.vel.median",DATAFLOAT,1,&num_model,mvlos);
  }

  if (num_bnd !=0) {
    DataMapAddArray(data,"boundary.mlat",DATAFLOAT,1,&num_bnd,bnd_lat);
    DataMapAddArray(data,"boundary.mlon",DATAFLOAT,1,&num_bnd,bnd_lon);
  }

  if (fid !=-1) s = DataMapWrite(fid,data);
  else          s = DataMapSize(data);

  DataMapFree(data);
  free(buf);

  return s;
}


int CnvMapFwrite(FILE *fp,struct CnvMapData *map,struct GridData *grd) {
  return CnvMapWrite(fileno(fp),map,grd);
}

