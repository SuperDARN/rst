/* gtable.c
   ========
   Author: R.J.Barnes
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
#include "limit.h"
#include "radar.h"
#include "rpos.h" 
#include "invmag.h"

#include "rmath.h"
#include "scandata.h"
#include "gtable.h"

double v_e_min=100;
double p_l_e_min=1;
double w_l_e_min=1;



int GridTableZero(int pnum,struct GridPnt *ptr) {

  int i;
  
  for (i=0;i<pnum;i++) {
      ptr[i].azm=0;
      ptr[i].vel.median=0;
      ptr[i].vel.sd=0;
      ptr[i].pwr.median=0;
      ptr[i].pwr.sd=0;
      ptr[i].wdt.median=0;
      ptr[i].wdt.sd=0;
      ptr[i].cnt=0;
  }
  return 0;
}

int GridTableSort(const void *a,const void *b) {
  struct GridPnt *ap;
  struct GridPnt *bp;
  ap=(struct GridPnt *) a;
  bp=(struct GridPnt *) b;
 
  if (ap->ref<bp->ref) return -1;
  if (ap->ref>bp->ref) return 1;
  return 0;
}

struct GridPnt *GridTableMake(struct RadarSite *pos,
                       int *frang,int *rsep,int *rxrise,
                       int  *ltab,int *pnum,
                       double *azmt,double *itab) {

  struct GridPnt *ptr=NULL;

  double lat,lon,azm,geoazm,elv,lspc;
  int bm,rn,i=0,j,k=0;
  double velco;

  ptr=malloc(sizeof(struct GridPnt)*
	     (16*MAX_RANGE)); /* space for everything */

 
  velco=(2*PI/86400.0)*6356.779*1000*cos(PI*pos->geolat/180.0);
 
  for (bm=0;bm<16;bm++) {
  
    if ((frang[bm] !=-1) && (rsep[bm] !=0)) {
      for (rn=0;rn<MAX_RANGE;rn++) {
         
         RPosRngBmAzmElv(bm,rn,1995,pos,
                 frang[bm],rsep[bm],rxrise[bm],
                 300.0,&geoazm,&elv);

         

         RPosInvMag(bm,rn,1995,pos,
                 frang[bm],rsep[bm],rxrise[bm],
                 300.0,&lat,&lon,&azm);
        

         if (lon<0) lon+=360;
         if ((lat>0) && (lat<50)) lat=50;
         if ((lat<0) && (lat>-50)) lat=-50;

         /* fix lat and lon to grid spacing */
         
         if (lat>0) ptr[i].mlat=(int) (lat)+0.5;
         else ptr[i].mlat=(int) (lat)-0.5;

         lspc=((int) (360*cos(fabs(ptr[i].mlat)*PI/180)+0.5))/(360.0);
  
         ptr[i].mlon=((int) (lon*lspc)+0.5)/lspc;

         if (lat>0)      
           ptr[i].ref=1000*( (int) lat-50)+( (int) (lon*lspc) ); 
         else ptr[i].ref=-1000*( (int) -lat-50)-( (int) (lon*lspc) ); 
                    /* grid reference */   
         ptr[i].cnt=bm*MAX_RANGE+rn;
         azmt[bm*MAX_RANGE+rn]=azm;
         itab[bm*MAX_RANGE+rn]=velco*cos(PI*(90+geoazm)/180.0);
        

         i++;
      }
    }
  }
  /* sort the grid table into ascending order */

  qsort(ptr,i,sizeof(struct GridPnt),GridTableSort);


  /* now sift through and remove redundancy */


  ltab[ptr[0].cnt]=0; 
  ptr[0].npix=1;
  for (j=1;j<i;j++) {
    if (ptr[k].ref==ptr[j].ref) { /* repeat */
      ltab[ptr[j].cnt]=k;
      ptr[k].npix++;
    } else {
      k++;
      memcpy(&ptr[k],&ptr[j],sizeof(struct GridPnt));
      ltab[ptr[j].cnt]=k;
      ptr[k].npix=1;
    }    
  }

  *pnum=k+1;
  ptr=realloc(ptr,sizeof(struct GridPnt)*(k+1));
  return ptr;  
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
    for (i=0;i<ptr->pnum;i++) {
      if (ptr->pnt[i].cnt==0) continue;
      if (ptr->pnt[i].cnt<=(0.25*ptr->nscan*ptr->pnt[i].npix)) {
        ptr->pnt[i].cnt=0;
        continue;
      }
      ptr->npnt++;
      ptr->pnt[i].azm=ptr->pnt[i].azm/ptr->pnt[i].cnt;
      ptr->pnt[i].vel.median=ptr->pnt[i].vel.median/ptr->pnt[i].vel.sd;
      ptr->pnt[i].wdt.median=ptr->pnt[i].wdt.median/ptr->pnt[i].wdt.sd;
      ptr->pnt[i].pwr.median=ptr->pnt[i].pwr.median/ptr->pnt[i].pwr.sd;
      
     
      ptr->pnt[i].vel.sd=1/sqrt(ptr->pnt[i].vel.sd);
      ptr->pnt[i].wdt.sd=1/sqrt(ptr->pnt[i].wdt.sd);
      ptr->pnt[i].pwr.sd=1/sqrt(ptr->pnt[i].pwr.sd);

    }
    ptr->status=1;
    return 1;
  }
  return 0;
}



void GridTableMap(struct GridTable *ptr,struct RadarScan *scan,
                struct RadarSite *pos,int tlen,int iflg) {
  int gflg,i,j,l,fcnt=0;
  double freq=0,noise=0;
  double variance=0;
  double tm;

  tm=(scan->st_time+scan->ed_time)/2.0;
   
  /* test to see if we need to remake the grid */
 
  for (gflg=0;gflg<16;gflg++) {
    if (scan->bm[gflg].bm !=-1) {  
      if ((ptr->frang[gflg] !=scan->bm[gflg].frang) ||
          (ptr->rsep[gflg] !=scan->bm[gflg].rsep)) break;
    }
  }
 
  
  if ((ptr->st_time==-1) || (gflg<16)) {

    /* generate grid */

    ptr->st_id=scan->stid;
    for (i=0;i<16;i++) {
      if ((scan->bm[i].bm !=-1) && (scan->bm[i].frang !=-1)) 
         ptr->frang[i]=scan->bm[i].frang;
      else ptr->frang[i]=-1;
      if ((scan->bm[i].bm !=-1) && (scan->bm[i].rsep !=0)) 
         ptr->rsep[i]=scan->bm[i].rsep;
      else ptr->rsep[i]=0;
    }
    ptr->nscan=0;
    if (ptr->pnt !=NULL) free (ptr->pnt);
    ptr->pnt=GridTableMake(pos,ptr->frang,ptr->rsep,ptr->rxrise,
                         ptr->ltable,&ptr->pnum,
                         ptr->azm,ptr->itab);
    ptr->st_time=tlen*( (int) (tm/tlen)); 
    ptr->ed_time=ptr->st_time+tlen;
    GridTableZero(ptr->pnum,ptr->pnt);
  }
 

  /* if last grid was completed then reset the arrays */

  if (ptr->status==1) {
     ptr->status=0;
     ptr->freq=0;
     ptr->nscan=0;
     GridTableZero(ptr->pnum,ptr->pnt);
     ptr->st_time=tlen*( (int) (tm/tlen)); 
     ptr->ed_time=ptr->st_time+tlen;
  }

  /* okay map the pixels */

  for (i=0;i<16;i++) {
    if (scan->bm[i].bm==-1) continue;
    
    ptr->prog_id=scan->bm[i].cpid;
    freq+=scan->bm[i].freq;
    noise+=scan->bm[i].noise;
   
    fcnt++;
    for (j=0;j<MAX_RANGE;j++) {
      if (scan->bm[i].sct[j] !=0) {
        double v_e; 
        double p_l_e;
        double w_l_e;
           
        v_e=scan->bm[i].rng[j].v_e;
        p_l_e=scan->bm[i].rng[j].p_l_e;
        w_l_e=scan->bm[i].rng[j].w_l_e;
   
        /* apply floor on uncertainties */
      

        if (v_e<v_e_min) v_e=v_e_min;
        if (p_l_e<p_l_e_min) p_l_e=p_l_e_min;
        if (w_l_e<w_l_e_min) w_l_e=w_l_e_min;
 
        l=ptr->ltable[i*MAX_RANGE+j];
       
         
        ptr->pnt[l].azm+=ptr->azm[i*MAX_RANGE+j];

	  
        if (iflg !=0) ptr->pnt[l].vel.median+=
                        -(scan->bm[i].rng[j].v+ptr->itab[i*MAX_RANGE+j])*
                         1/(v_e*v_e);
        else 
          ptr->pnt[l].vel.median+=-scan->bm[i].rng[j].v*1/(v_e*v_e);

        ptr->pnt[l].pwr.median+=scan->bm[i].rng[j].p_l*1/(p_l_e*p_l_e);
        ptr->pnt[l].wdt.median+=scan->bm[i].rng[j].w_l*1/(w_l_e*w_l_e);

        ptr->pnt[l].vel.sd+=1/(v_e*v_e);
        ptr->pnt[l].pwr.sd+=1/(p_l_e*p_l_e);
        ptr->pnt[l].wdt.sd+=1/(w_l_e*w_l_e);
        ptr->pnt[l].cnt++;
      }
    }
  }

  freq=freq/fcnt;
  noise=noise/fcnt;
  

  /* find variance of the noise */
  
  for (i=0;i<16;i++) {
    if (scan->bm[i].bm==-1) continue;
    variance+=(scan->bm[i].noise-noise)*(scan->bm[i].noise-noise);
  }

  ptr->noise.mean=noise;
  ptr->noise.sd=sqrt(variance/fcnt);
  ptr->freq=freq;
  ptr->nscan++;
}














