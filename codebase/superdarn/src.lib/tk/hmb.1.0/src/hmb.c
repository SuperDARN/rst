/* hmb.c
   =====
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

#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"
#include "igrflib.h"
#include "map_addhmb.h"

float latref=59;
int nlat=36;

float bndarr[24][36];
float lathmb[36];

#define HMBSTEP 50

struct hmbtab {
  int num;
  double *time;
  float *median;
  float *actual;
};


struct hmbtab *load_hmb(FILE *fp)
{
  struct hmbtab *ptr;
  int yr,mo,dy,hr,mt;
  float sc;
  float median,actual;
  char line[256];
  int i;
  int cnt=0;
  ptr=malloc(sizeof(struct hmbtab));
  ptr->time=malloc(sizeof(double)*HMBSTEP);
  ptr->median=malloc(sizeof(float)*HMBSTEP);
  ptr->actual=malloc(sizeof(float)*HMBSTEP);

  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]==' ') || (line[i]=='\t') ||
             (line[i] =='\n'));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;

    if (sscanf(line,"%d %d %d %d %d %g %g %g",&yr,&mo,&dy,&hr,&mt,&sc,
              &median,&actual) != 8) continue;

    ptr->time[cnt]=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    ptr->median[cnt]=median;
    ptr->actual[cnt]=actual;
    cnt++;
    if ((cnt % HMBSTEP)==0) {
        int blk;
        blk=1+cnt/HMBSTEP;
        ptr->time=realloc(ptr->time,sizeof(double)*HMBSTEP*blk);
        ptr->median=realloc(ptr->median,sizeof(float)*HMBSTEP*blk);
        ptr->actual=realloc(ptr->actual,sizeof(float)*HMBSTEP*blk);
    }

  }

  ptr->num=cnt;
  ptr->time=realloc(ptr->time,sizeof(double)*cnt);
  ptr->median=realloc(ptr->median,sizeof(float)*cnt);
  ptr->actual=realloc(ptr->actual,sizeof(float)*cnt);
 
  return ptr;
}


void add_hmb_grd(float latmin,int yr,int yrsec,struct CnvMapData *map,
                 int magflg)
{
  int i;
  float lat;
  float lstp;
  int nlon;
  float mlt,latx,lon;
  float bfac,del_L;

  char *lflg=NULL;
  int c=0;
  int off;

  int mo,dy,hr,mt,sc;

  double (*MLTCnv)(int, int, double);

  if (magflg == 2) {
    TimeYrsecToYMDHMS(yrsec,yr,&mo,&dy,&hr,&mt,&sc);
  } else if (magflg == 1) {
    MLTCnv = &MLTConvertYrsec;
  } else {
    MLTCnv = &MLTConvertYrsec_v2;
  }

  /* We should do something about the hemisphere here */

  for (lat=latmin+0.5;lat<90;lat++) {  
    nlon=(int) (360*cos(lat*PI/180)+0.5);
    lstp=360.0/nlon;

    if (lflg==NULL) lflg=malloc(nlon);
    else lflg=realloc(lflg,nlon);
    memset(lflg,0,nlon);

    /* walk round in longitude locating cells that should be filled */

    c=0;
    lon=0.5*lstp;
    for (i=0;i<nlon;i++) {

      if (magflg == 2) mlt = ecdip_mlt(yr,mo,dy,hr,mt,sc,lon);
      else             mlt = (*MLTCnv)(yr,yrsec,lon);

      bfac=(90-latmin)/(90-latref);
      del_L=bfac*5.5;
      latx=latmin;
      if ((mlt>=11) && (mlt<=19)) 
        latx=latmin+del_L*(1+cos((PI/8)*(mlt-11)));
      else if ((mlt<11) && (mlt>=5)) 
        latx=latmin+del_L*(1+cos((PI/6)*(11-mlt)));

      if (lat<=latx) {
        lflg[i]=1;
        c++;
      }
      lon+=lstp;
    }
    if (c==0) break; /* no vectors at this latitude */

    off=map->num_model;
    map->num_model+=c;
    if (map->model==NULL) 
      map->model=malloc(sizeof(struct GridGVec)*map->num_model);
    else map->model=realloc(map->model,sizeof(struct GridGVec)*map->num_model);

    lon=0.5*lstp;

    for (i=0;i<nlon;i++) {
      if (lflg[i] !=0) {
        if (map->hemisphere==1) map->model[off].mlat=lat;
        else map->model[off].mlat=-lat;
        map->model[off].mlon=lon;
        map->model[off].azm=45.0;
        map->model[off].vel.median=1.0;
        map->model[off].vel.sd=0.0;
        off++;
      }
      lon+=lstp;
    }
  }

}


void make_hmb()
{
  int m,n;
  float  mlt,latx;
  float latmin;
  float bfac,del_L;

  for (m=0;m<24;m++) {
    for (n=0;n<nlat;n++) {

      mlt=m;

      lathmb[n]=n+40.0; /* The +40.0 means we start making the grid used by the iterative HMB-finding algorithm at geomagnetic latitude of 40 degrees. */  
      latmin=lathmb[n];

      bfac=(90-latmin)/(90-latref);
      del_L=bfac*5.5;

      latx=latmin;
      if ((mlt>=11) && (mlt<=19)) latx=latmin+del_L*(1+cos((PI/8)*(mlt-11)));
      else if ((mlt<11) && (mlt>=5))
         latx=latmin+del_L*(1+cos((PI/6)*(11-mlt)));
      bndarr[m][n]=latx;
    }
  }
}


void map_addhmb(int yr, int yrsec, struct CnvMapData *map, int bndnp,
                float bndstep, float latref, float latmin, int magflg)
{
  int i;
  float bfac,del_L;
  float mlt;

  int mo,dy,hr,mt,sc;

  double (*MLTCnv)(int, int, double);

  if (magflg == 2) {
    TimeYrsecToYMDHMS(yrsec,yr,&mo,&dy,&hr,&mt,&sc);
  } else if (magflg == 1) {
    MLTCnv = &MLTConvertYrsec;
  } else {
    MLTCnv = &MLTConvertYrsec_v2;
  }

  map->num_bnd=bndnp;

  if (map->bnd_lat==NULL) map->bnd_lat=malloc(sizeof(double)*map->num_bnd);
  else map->bnd_lat=realloc(map->bnd_lat,sizeof(double)*map->num_bnd);

  if (map->bnd_lon==NULL) map->bnd_lon=malloc(sizeof(double)*map->num_bnd);
  else map->bnd_lon=realloc(map->bnd_lon,sizeof(double)*map->num_bnd);

  for (i=0;i<map->num_bnd;i++) {
    map->bnd_lon[i]=i*bndstep;
    if (magflg == 2) mlt = ecdip_mlt(yr,mo,dy,hr,mt,sc,map->bnd_lon[i]);
    else             mlt = (*MLTCnv)(yr,yrsec,map->bnd_lon[i]);

    bfac=(90-latmin)/(90-latref);
    del_L=bfac*5.5;

    if (map->hemisphere==1) map->bnd_lat[i]=latmin;
    else  map->bnd_lat[i]=-latmin;
    if (map->hemisphere==1) {
      if ((mlt>=11) && (mlt<=19))
        map->bnd_lat[i]=latmin+del_L*(1+cos((PI/8)*(mlt-11)));
      else if ((mlt<11) && (mlt>=5)) 
        map->bnd_lat[i]=latmin+del_L*(1+cos((PI/6)*(11-mlt)));
    } else {
      if ((mlt>=11) && (mlt<=19)) 
        map->bnd_lat[i]=-latmin-del_L*(1+cos((PI/8)*(mlt-11)));
      else if ((mlt<11) && (mlt>=5)) 
        map->bnd_lat[i]=-latmin-del_L*(1+cos((PI/6)*(11-mlt)));
    }

  }

  add_hmb_grd(latmin,yr,yrsec,map,magflg);
  if (map->hemisphere==1) map->latmin=latmin;
  else                    map->latmin=-latmin;
}


int latcmp(const void *a,const void *b)
{
  float *x,*y;
  x=(float *) a;
  y=(float *) b;

  if (*x<*y) return -1;
  if (*x>*y) return 1;
  return 0;
}

