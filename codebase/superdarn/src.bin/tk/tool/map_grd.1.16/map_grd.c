 /* map_grd.c
   ========== 
   Author: R.J.Barnes and others

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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rmath.h"

#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "oldcnvmapwrite.h"
#include "cnvmapwrite.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"
#include "mlt.h"
#include "mlt_v2.h"
#include "igrflib.h"

#include "radar.h"

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

#include "hlpstr.h"

struct CnvMapData *map;
struct GridData *grd;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: map_grd --help\n");
  return(-1);
}

/* Convert input date from YYYMMDD format to epoch time */
double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;

  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);

  if (yr<1970) yr+=1900;

  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}

/* Convert input time from HHMM format to number of seconds */
double strtime(char *text) {
  int hr,mn;
  int i;

  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}

int main(int argc,char *argv[]) {

  int old=0;
  int old_aacgm=0;
  int ecdip=0;

  int i, arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  char *envstr;
  FILE *fp;

  char *fname=NULL;
  int tme;
  int yrsec;
  int cnt=0;
  float latmin=60;
  float latshft=0;
  unsigned char sh=0;

  int yr,mo,dy,hr,mt;
  double sc;

  int empty=0;
  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;
  double stime=-1;
  double etime=-1;
  double extime=0;
  double sdate=-1;
  double edate=-1;
  int tlen=120;

  /* function pointers for file reading/writing (old and new) and MLT */
  int (*Grid_Read)(FILE *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);
  double (*MLTCnv)(int, int, double);

  grd=GridMake();
  map=CnvMapMake();

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");
  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"sh",'x',&sh);
  OptionAdd(&opt,"l",'f',&latmin);
  /*OptionAdd(&opt,"s",'f',&latshft);*/ /* latshft option removed at suggestion of JMR */
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"ecdip",'x',&ecdip);

  OptionAdd(&opt,"empty",'x',&empty);   /* Create empty map file without real data */
  OptionAdd(&opt,"st",'t',&stmestr);    /* Start time in HH:MM format */
  OptionAdd(&opt,"et",'t',&etmestr);    /* End time in HH:MM format */
  OptionAdd(&opt,"sd",'t',&sdtestr);    /* Start date in YYYYMMDD format */
  OptionAdd(&opt,"ed",'t',&edtestr);    /* End date in YYYYMMDD format */
  OptionAdd(&opt,"ex",'t',&exstr);      /* Use interval with extent HH:MM */
  OptionAdd(&opt,"tl",'i',&tlen);       /* Time interval of records in seconds */

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  if (arg !=argc) fname=argv[arg];

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL && empty==0) {
    fprintf(stderr,"Grid file not found.\n");
    exit(-1);
  }

  memset(map,0,sizeof(struct CnvMapData));
  map->fit_order=4;
  map->doping_level=1;
  map->model_wt=1;
  map->error_wt=1;
  map->hemisphere=1;
  map->lat_shft=latshft;

  /* set function pointer to read/write old or new */
  if (old) {
    Grid_Read = &OldGridFread;
    Map_Write = &OldCnvMapFwrite;
  } else {
    Grid_Read = &GridFread;
    Map_Write = &CnvMapFwrite;
  }

  /* set function pointer to compute MLT or MLT_v2 */
  if (old_aacgm) MLTCnv = &MLTConvertYrsec;
  else           MLTCnv = &MLTConvertYrsec_v2;

  if (!ecdip && !old_aacgm) AACGM_v2_Lock();

  if (sh==1) map->hemisphere=-1;

  if (empty) {
    if (exstr !=NULL) extime=strtime(exstr);
    if (stmestr !=NULL) stime=strtime(stmestr);
    if (etmestr !=NULL) etime=strtime(etmestr);
    if (sdtestr !=NULL) sdate=strdate(sdtestr);
    if (edtestr !=NULL) edate=strdate(edtestr);

    if (sdate !=-1) {
      /* If start time not provided then use 00:00 UT */
      if (stime==-1) stime=sdate;
      else stime+=sdate;

      /* If end time provided then determine end date */
      if (etime !=-1) {
        if (edate==-1) etime+=sdate;
        else etime+=edate;
      } else etime+=(sdate+24*3600);

      /* If time extent provided then use that to calculate end time */
      if (extime !=0) etime=stime+extime;
    } else {
      fprintf(stderr,"Must provide start date for empty file.\n");
      exit(-1);
    }

    /* Set up our empty griddata structure */
    grd->st_time=stime;
    grd->ed_time=stime+tlen;
    grd->stnum=1;
    grd->vcnum=0;
    grd->xtd=0;

    grd->sdata=malloc(sizeof(struct GridSVec));
    grd->sdata[0].st_id=999;
    grd->sdata[0].chn=0;
    grd->sdata[0].npnt=0;
    grd->sdata[0].freq0=0;
    grd->sdata[0].major_revision=0;
    grd->sdata[0].minor_revision=0;
    grd->sdata[0].prog_id=0;
    grd->sdata[0].noise.mean=0;
    grd->sdata[0].noise.sd=0;
    grd->sdata[0].gsct=0;
    grd->sdata[0].vel.min=0;
    grd->sdata[0].vel.max=0;
    grd->sdata[0].pwr.min=0;
    grd->sdata[0].pwr.max=0;
    grd->sdata[0].wdt.min=0;
    grd->sdata[0].wdt.max=0;
    grd->sdata[0].verr.min=0;
    grd->sdata[0].verr.max=0;

    while(grd->st_time<etime) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (ecdip) IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      else if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);

      if (ecdip) {
        map->mlt.start = ecdip_mlt(yr,mo,dy,hr,mt,(int)sc,0.0);
      } else {
        yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
        map->mlt.start=(*MLTCnv)(yr,yrsec,0.0);
      }

      TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (ecdip) {
        map->mlt.end = ecdip_mlt(yr,mo,dy,hr,mt,(int)sc,0.0);
      } else {
        yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
        map->mlt.end=(*MLTCnv)(yr,yrsec,0.0);
      }

      tme=(grd->st_time+grd->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      if (ecdip) {
        map->mlt.av = ecdip_mlt(yr,mo,dy,hr,mt,(int)sc,0.0);
      } else {
        yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
        map->mlt.av=(*MLTCnv)(yr,yrsec,0.0);
      }

      if (latshft !=0) {
        map->lon_shft=(map->mlt.av-12)*15.0;
        map->latmin-=latshft;
      }

      if (map->hemisphere==1) map->latmin=latmin;
      else map->latmin=-latmin;

      map->st_time=grd->st_time;
      map->ed_time=grd->ed_time;

      Map_Write(stdout,map,grd);
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1)
        fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);

      grd->st_time+=tlen;
      grd->ed_time+=tlen;
    }

    if (fp !=stdin) fclose(fp);
    return 0;
  }

  while ((*Grid_Read)(fp,grd) !=-1) {
    TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (ecdip) IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
    else if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);

    if (cnt==0) {
      for (i=0;i<grd->stnum;i++) {
        radar=RadarGetRadar(network,grd->sdata[i].st_id);
        if (radar!=NULL) 
          site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
        if ((site !=NULL) && (site->geolat<0)) {
          map->hemisphere=-1;
          break;
        }
      }
    } 

    if (ecdip) {
      map->mlt.start = ecdip_mlt(yr,mo,dy,hr,mt,(int)sc,0.0);
    } else {
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map->mlt.start=(*MLTCnv)(yr,yrsec,0.0);
    }

    TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (ecdip) {
      map->mlt.end = ecdip_mlt(yr,mo,dy,hr,mt,(int)sc,0.0);
    } else {
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map->mlt.end=(*MLTCnv)(yr,yrsec,0.0);
    }

    tme=(grd->st_time+grd->ed_time)/2.0;
    TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
    if (ecdip) {
      map->mlt.av = ecdip_mlt(yr,mo,dy,hr,mt,(int)sc,0.0);
    } else {
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map->mlt.av=(*MLTCnv)(yr,yrsec,0.0);
    }

    if (latshft !=0) {
      map->lon_shft=(map->mlt.av-12)*15.0;
      map->latmin-=latshft;
    }

    if (map->hemisphere==1) map->latmin=latmin;
    else map->latmin=-latmin;

    map->st_time=grd->st_time;
    map->ed_time=grd->ed_time;

    Map_Write(stdout,map,grd);
    TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (vb==1) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);

    cnt++;
  }

  return 0;
}

