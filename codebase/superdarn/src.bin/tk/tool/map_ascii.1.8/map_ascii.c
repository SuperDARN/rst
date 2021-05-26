/* map_ascii.c
   ===========
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "aacgm.h"

#include "errstr.h"
#include "hlpstr.h"

#include "rmath.h"
#include "griddata.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapsolve.h"
#include "make_pgrid.h"
#include "aacgmlib_v2.h"
#include "igrflib.h"


struct OptionData opt;

struct CnvMapData *map;
struct GridData *grd;

struct CnvGrid *grid;

double *zbuffer=NULL;
int zwdt,zhgt;
struct polydata *ctr;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: map_ascii --help\n");
  return(-1);
}

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

  int arg;
  struct RfileIndex *oinx=NULL;
  struct CnvMapIndex *inx=NULL;

  int yr,mo,dy,hr,mt;
  double sc;
  int tme;
  int yrsec;
  int first;
  int noigrf=0;
  float decyear;

  FILE *grdfp;
  int i;

  int status;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  float latmin=60.0;
  float mlt;

  char *stime_txt=NULL;
  char *sdate_txt=NULL;

  char *extime_txt=NULL;
  char *etime_txt=NULL;
  char *edate_txt=NULL;

  double stime=-1;
  double etime=-1;
  double extime=0;
  double sdate=-1;
  double edate=-1;

  double glat,glon,r;

  int step=1;

  int num;
  int lat,lon;

  int *count=NULL;

  unsigned char vb=0;

  int fnum=0;

  grd=GridMake();
  map=CnvMapMake();

  grid=CnvGridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"old_aacgm",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"st",'t',&stime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"et",'t',&etime_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"latmin",'f',&latmin);

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

  if (stime_txt !=NULL)  stime=strtime(stime_txt);
  if (etime_txt !=NULL)  etime=strtime(etime_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL)  sdate=strdate(sdate_txt);
  if (edate_txt !=NULL)  edate=strdate(edate_txt);

  if (old) {
    if (arg<(argc-1)) {
        FILE *fp;
       fprintf(stderr,"Loading index.\n");
       fp=fopen(argv[argc-1],"r");
       if (fp !=NULL) {
         oinx=RfileLoadIndex(fp);
         fclose(fp);
       }
    }
  } else {
    if (arg<(argc-1)) {
        FILE *fp;
       fprintf(stderr,"Loading index.\n");
       fp=fopen(argv[argc-1],"r");
       if (fp !=NULL) {
         inx=CnvMapIndexFload(fp);
         fclose(fp);
       }
    }
  }

  if (arg !=argc) grdfp=fopen(argv[arg],"r");
  else grdfp=stdin;
  if (grdfp==NULL) {
     fprintf(stderr,"File not found.\n");
     exit(-1);
  }

  if (old) {
    if (OldCnvMapFread(grdfp,map,grd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  } else {
    if (CnvMapFread(grdfp,map,grd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  }

  if ((map->hemisphere==-1) && (latmin>0)) latmin=-latmin;

  if (stime !=-1) { /* we must skip the start of the files */
    int yr,mo,dy,hr,mt;
    double sc;

    if (stime==-1) stime= ( (int) grd->st_time % (24*3600));
    if (sdate==-1) stime+=grd->st_time - ( (int) grd->st_time % (24*3600));
    else stime+=sdate;

    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    if (old) status=OldCnvMapFseek(grdfp,yr,mo,dy,hr,mt,sc,oinx,NULL);
    else status=CnvMapFseek(grdfp,yr,mo,dy,hr,mt,sc,NULL,inx);
    if (status ==-1) {
      fprintf(stderr,"File does not contain the requested interval.\n");
      exit(-1);
    }
    if (old) OldCnvMapFread(grdfp,map,grd);
    else CnvMapFread(grdfp,map,grd);
  } else stime=grd->st_time;

  if (etime !=-1) {
    if (edate==-1) etime+=grd->st_time - ( (int) grd->st_time % (24*3600));
    else etime+=edate;
  }

  if (extime !=0) etime=stime+extime;

  first = 1;
  do {

    if (vb !=0) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",dy,mo,yr,hr,mt,(int) sc);
    }

    tme = (grd->st_time + grd->ed_time)/2.0;
    TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int)sc);
    decyear = yr + (float)yrsec/TimeYMDHMSToYrsec(yr,12,31,23,59,59);

    noigrf = map->noigrf;

    if (first) {
      if (!noigrf)    IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      first = 0;
    }

    if (fnum==0)  {
      make_pgrid(latmin,grid);
      count=malloc(sizeof(int)*grid->num);
    }
    memset(count,0,grid->num*sizeof(int));
    grid->type=0;
    CnvMapSolve(map,grid,decyear,old_aacgm);

    num=0;
    for (i=0;i<grid->num;i++) {
      if (fabs(grid->lat[i])<=fabs(map->latmin)) continue;
      num++;
    }

    for (i=0;i<grd->vcnum;i++) {
      lat= (fabs(grd->data[i].mlat) - 60.0) / 1.0;
      if (lat<0) continue;
      lon= grd->data[i].mlon /2.0;
      count[lat*180+lon]++;
    }

    TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%d %d %d %d %d %d ",yr,mo,dy,hr,mt,(int) sc);
    TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%d %d %d %d %d %d %g\n",yr,mo,dy,hr,mt,(int) sc,
            map->latmin);

    fprintf(stdout,"%d\n",num);

    for (i=0;i<grid->num;i++) {
      if (fabs(grid->lat[i])<=fabs(map->latmin)) continue;
      fprintf(stdout,"%#10g %10d ",grid->mag[i],count[i]);
      if (old_aacgm) AACGMConvert(grid->lat[i],grid->lon[i],300.0,&glat,&glon,&r,1);
      else AACGM_v2_Convert(grid->lat[i],grid->lon[i],300.0,&glat,&glon,&r,1);
      fprintf(stdout,"%#10g %#10g ",glat,glon);
      mlt=grid->lon[i]/15.0+map->mlt.av;
      if (mlt>24.0) mlt-=24.0;
      fprintf(stdout,"%#10g %#10g %#10g\n",grid->lat[i],grid->lon[i],mlt);
    }

   fnum++;
   if (old) for (i=0;i<step;i++) status=OldCnvMapFread(grdfp,map,grd);
   else for (i=0;i<step;i++) status=CnvMapFread(grdfp,map,grd);
   if ((etime !=-1) && (grd->ed_time>=etime)) break;
  } while (status !=-1);

  if (grdfp !=stdin) fclose(grdfp);
  free(count);
  return 0;
}

