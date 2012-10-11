/* gtable.c
   ========
   Author: R.J.Barnes & W.Bristow
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "radar.h"
#include "rpos.h" 
#include "invmag.h"

#include "rmath.h"
#include "rtime.h"
#include "scandata.h"
#include "gtable.h"

double v_e_min=100;
double p_l_e_min=1;
double w_l_e_min=1;

struct GridTable *GridTableMake() {
  struct GridTable *ptr;
  ptr=malloc(sizeof(struct GridTable));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct GridTable));
  ptr->bm=NULL;
  ptr->pnt=NULL;
  ptr->st_time=-1;
  return ptr;
}

void GridTableFree(struct GridTable *ptr) {
  int n;
  if (ptr==NULL) return;
 

  if (ptr->bm !=NULL) {
    for (n=0;n<ptr->bnum;n++) {
      if (ptr->bm[n].azm !=NULL) free(ptr->bm[n].azm);
      if (ptr->bm[n].ival !=NULL) free(ptr->bm[n].ival);
      if (ptr->bm[n].inx !=NULL) free(ptr->bm[n].inx);
    }
    free(ptr->bm);
  }
  if (ptr->pnt !=NULL) free(ptr->pnt);
  free(ptr);
}

int GridTableZero(int pnum,struct GridPnt *ptr) {

  int i;
  
  for (i=0;i<pnum;i++) {
      ptr[i].azm=0;
      ptr[i].vel.median_n=0;
      ptr[i].vel.median_e=0;
      ptr[i].vel.sd=0;
      ptr[i].pwr.median=0;
      ptr[i].pwr.sd=0;
      ptr[i].wdt.median=0;
      ptr[i].wdt.sd=0;
      ptr[i].cnt=0;
  }
  return 0;
}

int GridTableTest(struct GridTable *ptr,struct RadarScan *scan,int tlen) {

  double tm;
  int i;
  tm=(scan->st_time+scan->ed_time)/2.0;
  if (ptr->st_time==-1) return 0;

  if (tm>ptr->ed_time) {
    /* average the grid */
   
    ptr->npnt=0;
    ptr->freq=ptr->freq/ptr->nscan;
    ptr->noise.mean=ptr->noise.mean/ptr->nscan;
    ptr->noise.sd=ptr->noise.sd/ptr->nscan;

    for (i=0;i<ptr->pnum;i++) {
      if (ptr->pnt[i].cnt==0) continue;
      
      if (ptr->pnt[i].cnt<=(0.25*ptr->nscan*ptr->pnt[i].max)) {
        ptr->pnt[i].cnt=0;
        continue;
      }
     
      ptr->npnt++;
      ptr->pnt[i].vel.median_n=ptr->pnt[i].vel.median_n/ptr->pnt[i].vel.sd;
      ptr->pnt[i].vel.median_e=ptr->pnt[i].vel.median_e/ptr->pnt[i].vel.sd;
      ptr->pnt[i].vel.median=sqrt(ptr->pnt[i].vel.median_n*ptr->pnt[i].vel.median_n +
				  ptr->pnt[i].vel.median_e*ptr->pnt[i].vel.median_e);
      ptr->pnt[i].azm=atan2(ptr->pnt[i].vel.median_e,ptr->pnt[i].vel.median_n)*180./acos(-1.);
      ptr->pnt[i].wdt.median=ptr->pnt[i].wdt.median/ptr->pnt[i].wdt.sd;
      ptr->pnt[i].pwr.median=ptr->pnt[i].pwr.median/ptr->pnt[i].pwr.sd;
         
      ptr->pnt[i].vel.sd=1/sqrt(ptr->pnt[i].vel.sd);
      ptr->pnt[i].wdt.sd=1/sqrt(ptr->pnt[i].wdt.sd);
      ptr->pnt[i].pwr.sd=1/sqrt(ptr->pnt[i].pwr.sd);

    }
    ptr->status=0;
    return 1;
  }

  return 0;
}

int GridTableAddPoint(struct GridTable *ptr) {
  void *tmp=NULL;  
  if (ptr==NULL) return -1;
  if (ptr->pnt==NULL) tmp=malloc(sizeof(struct GridPnt));
  else tmp=realloc(ptr->pnt,sizeof(struct GridPnt)*(ptr->pnum+1));
  if (tmp==NULL) return -1;
  ptr->pnt=tmp;
  memset(&ptr->pnt[ptr->pnum],0,sizeof(struct GridPnt));
  ptr->pnum++;
  return ptr->pnum-1;
}

int GridTableFindPoint(struct GridTable *ptr,int ref) {
  int n;
  for (n=0;n<ptr->pnum;n++) if (ref==ptr->pnt[n].ref) break;
  if (n==ptr->pnum) return -1;
  return n;
}

int GridTableAddBeam(struct GridTable *ptr,
                     struct RadarSite *pos,double alt,
                     double tval,struct RadarBeam *bm) {

  int yr,mo,dy,hr,mt;
  double sc;
  double lat,lon,azm,geoazm,elv,lspc,velco;
  float grdlat,grdlon;
  int ref;
  int r=0,inx,s;
  void *tmp=NULL;
  struct GridBm *b=NULL;
  struct GridPnt *p=NULL;

  if (ptr==NULL) return -1;
  if (pos==NULL) return -1;
  if (bm==NULL) return -1;

  velco=(2*PI/86400.0)*6356.779*1000*cos(PI*pos->geolat/180.0);

  if (ptr->bm==NULL) tmp=malloc(sizeof(struct GridBm));
  else tmp=realloc(ptr->bm,sizeof(struct GridBm)*(ptr->bnum+1));

  if (tmp==NULL) return -1;
  ptr->bm=tmp;
  b=&ptr->bm[ptr->bnum];
  ptr->bnum++;
  
  b->bm=bm->bm;
  b->frang=bm->frang;
  b->rsep=bm->rsep;
  b->rxrise=bm->rxrise;
  b->nrang=bm->nrang;

  b->azm=malloc(sizeof(double)*b->nrang);
  if (b->azm==NULL) return -1;
  b->ival=malloc(sizeof(double)*b->nrang);
  if (b->ival==NULL) return -1;
  b->inx=malloc(sizeof(int)*b->nrang);
  if (b->inx==NULL) return -1;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  for (r=0;r<b->nrang;r++) {
    s=RPosRngBmAzmElv(b->bm,r,yr,pos,
                    b->frang,b->rsep,b->rxrise,
                    alt,&geoazm,&elv);
    if (s==-1) break;
    s=RPosInvMag(b->bm,r,yr,pos,
               b->frang,b->rsep,b->rxrise,
               alt,&lat,&lon,&azm);
    if (s==-1) break;
    if (azm<0.) azm+=360.;
    if (lon<0) lon+=360;
    if ((lat>0) && (lat<50)) lat=50;
    if ((lat<0) && (lat>-50)) lat=-50;

    if (lat>0) grdlat=(int) (lat)+0.5;
    else grdlat=(int) (lat)-0.5;

    lspc=((int) (360*cos(fabs(grdlat)*PI/180)+0.5))/(360.0);

    grdlon=((int) (lon*lspc)+0.5)/lspc;

    if (lat>0)
      ref=1000*( (int) lat-50)+( (int) (lon*lspc) );
    else ref=-1000*( (int) -lat-50)-( (int) (lon*lspc) );

    inx=GridTableFindPoint(ptr,ref);
    if (inx==-1) inx=GridTableAddPoint(ptr);
 
    p=&ptr->pnt[inx];  
    p->ref=ref;
    p->max++;
    p->mlat=grdlat;
    p->mlon=grdlon;
    

    b->inx[r]=inx;    
    b->azm[r]=azm;
    b->ival[r]=velco*cos(PI*(90+geoazm)/180.0);
  }
  if (r !=b->nrang) {
    return -1;
  }
  return ptr->bnum-1;
}

int GridTableFindBeam(struct GridTable *ptr,struct RadarBeam *bm) {
  int n;
  for (n=0;n<ptr->bnum;n++) {
    if (ptr->bm[n].bm !=bm->bm) continue;
    if (ptr->bm[n].frang !=bm->frang) continue;
    if (ptr->bm[n].rsep !=bm->rsep) continue;
    if (ptr->bm[n].rxrise !=bm->rxrise) continue;
    if (ptr->bm[n].nrang !=bm->nrang) continue;
    break;
  }
  if (n==ptr->bnum) return -1;
  return n;
}

int GridTableMap(struct GridTable *ptr,struct RadarScan *scan,
                struct RadarSite *pos,int tlen,int iflg,double alt) {
  double freq=0,noise=0;
  double variance=0;
  double tm;
  int inx,cnt=0;
  int n,r,b;

  double v_e,p_l_e,w_l_e;

  struct GridBm *bm=NULL;

  tm=(scan->st_time+scan->ed_time)/2.0;

  if (ptr->status==0) {
     ptr->status=1;
     ptr->noise.mean=0;
     ptr->noise.sd=0;
     ptr->freq=0;
     ptr->nscan=0;
     GridTableZero(ptr->pnum,ptr->pnt);
     ptr->st_time=tlen*( (int) (tm/tlen)); 
     ptr->ed_time=ptr->st_time+tlen;
     ptr->st_id=scan->stid;
  }

  for (n=0;n<scan->num;n++) {
    if (scan->bm[n].bm==-1) continue;
    b=GridTableFindBeam(ptr,&scan->bm[n]);
    if (b==-1) {
      /* map a new beam */
      b=GridTableAddBeam(ptr,pos,alt,tm,&scan->bm[n]);
      if (b==-1) break;
    }
    bm=&ptr->bm[b];

    for (r=0;r<scan->bm[n].nrang;r++) {
      if (scan->bm[n].sct[r]==0) continue;              
        v_e=scan->bm[n].rng[r].v_e;
        p_l_e=scan->bm[n].rng[r].p_l_e;
        w_l_e=scan->bm[n].rng[r].w_l_e;
    
        if (v_e<v_e_min) v_e=v_e_min;
        if (p_l_e<p_l_e_min) p_l_e=p_l_e_min;
        if (w_l_e<w_l_e_min) w_l_e=w_l_e_min;
 
        
	inx=bm->inx[r];
        
        ptr->pnt[inx].azm+=bm->azm[r];

        if (iflg !=0) { 
	  ptr->pnt[inx].vel.median_n+=
	    -(scan->bm[n].rng[r].v+bm->ival[r])*
	    1/(v_e*v_e)*cosd(bm->azm[r]);
	  ptr->pnt[inx].vel.median_e+=
	    -(scan->bm[n].rng[r].v+bm->ival[r])*
	    1/(v_e*v_e)*sind(bm->azm[r]);
	} else { 
          ptr->pnt[inx].vel.median_n+=-scan->bm[n].rng[r].v*cosd(bm->azm[r])/(v_e*v_e);
          ptr->pnt[inx].vel.median_e+=-scan->bm[n].rng[r].v*sind(bm->azm[r])/(v_e*v_e);
	}
        ptr->pnt[inx].pwr.median+=scan->bm[n].rng[r].p_l*1/(p_l_e*p_l_e);
        ptr->pnt[inx].wdt.median+=scan->bm[n].rng[r].w_l*1/(w_l_e*w_l_e);

        ptr->pnt[inx].vel.sd+=1/(v_e*v_e);
        ptr->pnt[inx].pwr.sd+=1/(p_l_e*p_l_e);
        ptr->pnt[inx].wdt.sd+=1/(w_l_e*w_l_e);
        ptr->pnt[inx].cnt++;
    }
  }
  if (n !=scan->num) return -1;


  for (n=0;n<scan->num;n++) {
    if (scan->bm[n].bm==-1) continue;
    ptr->prog_id=scan->bm[n].cpid;
    freq+=scan->bm[n].freq;
    noise+=scan->bm[n].noise;
    cnt++;
  }

  freq=freq/cnt;
  noise=noise/cnt;
  
  for (n=0;n<scan->num;n++) {
    if (scan->bm[n].bm==-1) continue;
    variance+=(scan->bm[n].noise-noise)*(scan->bm[n].noise-noise);
  }
  ptr->noise.mean+=noise;
  ptr->noise.sd+=sqrt(variance/cnt);
  ptr->freq+=freq;
  ptr->nscan++;
  return 0;
}









