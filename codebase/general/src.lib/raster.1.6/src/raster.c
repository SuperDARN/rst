/* raster.c
   ========
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raster.h"



struct RasterIsect {
  float x;
  int n;
  double val;
};


struct RasterIsectDouble {
  double x;
  int n;
  double val;
};


int RasterSCmpDouble(const void *a,const void  *b) {
  struct RasterIsectDouble *c,*d;
  
  c=(struct RasterIsectDouble *) a;
  d=(struct RasterIsectDouble *) b;

  if (c->n>d->n) return 1;
  if (c->n<d->n) return -1;

  if (c->x>d->x) return 1;  
  if (c->x<d->x) return -1;
  return 0; 
}

int RasterCmpDouble(const void *a,const void  *b) {
  struct RasterIsectDouble *c,*d;

  c=(struct RasterIsectDouble *) a;
  d=(struct RasterIsectDouble *) b;

  if (c->x>d->x) return 1;  
  if (c->x<d->x) return -1;

  if (c->n>d->n) return 1;
  if (c->n<d->n) return -1;

  return 0; 
}


int RasterSCmp(const void *a,const void  *b) {
  struct RasterIsect *c,*d;
  
  c=(struct RasterIsect *) a;
  d=(struct RasterIsect *) b;

  if (c->n>d->n) return 1;
  if (c->n<d->n) return -1;

  if (c->x>d->x) return 1;  
  if (c->x<d->x) return -1;
  return 0; 
}

int RasterCmp(const void *a,const void  *b) {
  struct RasterIsect *c,*d;

  c=(struct RasterIsect *) a;
  d=(struct RasterIsect *) b;

  if (c->x>d->x) return 1;  
  if (c->x<d->x) return -1;

  if (c->n>d->n) return 1;
  if (c->n<d->n) return -1;

  return 0; 
}

void *Raster(int wdt,int hgt,int sparse,int type,void *zero,
             int poly,float *pnt,int *vertex,void *value) {
   
  int *zint=NULL;
  float *zfloat=NULL;
  double *zdouble=NULL;

  int *vint=NULL;
  float *vfloat=NULL;
  double *vdouble=NULL;
  
  struct RasterIsect *itable=NULL;

  float *ybbox;
 
  int i,j,k,a,b,n;

  float l,r;
  float ix;
  float x,y;
  float ib,ir,it,il;
  float yb=1e6,yt=-1e6;

  double ivb,ivt,iv;
  double val;
 
  /* allocate memory for the zbuffer and bounding box
     point table and intersection table */

  if (type==raster_INT) zint=malloc(sizeof(int)*wdt*hgt);
  else if (type==raster_FLOAT) zfloat=malloc(sizeof(float)*wdt*hgt);
  else zdouble=malloc(sizeof(double)*wdt*hgt);

  if (type==raster_INT) vint=(int *) value;
  else if (type==raster_FLOAT) vfloat=(float *) value;
  else vdouble=(double *) value;
  

  ybbox=malloc(sizeof(float)*2*poly);
  
  itable=malloc(sizeof(struct RasterIsect)*poly*4);

  if (type==raster_INT) for (i=0;i<wdt*hgt;i++) 
     zint[i]=*((int *) zero); /* zero array */
  else if (type==raster_FLOAT) for (i=0;i<wdt*hgt;i++) 
     zfloat[i]=*((float *) zero); /* zero array */
  else  for (i=0;i<wdt*hgt;i++) 
     zdouble[i]=*((double *) zero); /* zero array */
 
  /* generate bounding boxes */
  for (i=0;i<poly;i++) {
    ybbox[i*2]=1e6;
    ybbox[i*2+1]=-1e6;
    for (k=0;k<4;k++) {
       y=pnt[2*vertex[4*i+k]+1];
       if (y<yb) yb=y;
       if (y>yt) yt=y;

       if (y<ybbox[i*2]) ybbox[i*2]=y;
       if (y>ybbox[i*2+1]) ybbox[i*2+1]=y;
    }
  }
  
  /* scan zbuffer */

  if (yb<0) yb=0;
  if (yt>hgt) yt=hgt;
  
  for (y=(int) yb;y<(int) yt;y++) {
    
    k=0;
    for (i=0;i<poly;i++) {
      
      if ((y<ybbox[i*2]) || (y>ybbox[i*2+1])) continue;   
      
      for (n=1;n<=4;n++) {
        a=(n==4) ? 0: n;
        b=n-1;
        a=vertex[i*4+a];
        b=vertex[i*4+b]; 

        if ( pnt[a*2+1] < pnt[b*2+1]) {
          ib=pnt[a*2+1];
          it=pnt[b*2+1];
          il=pnt[a*2];
          ir=pnt[b*2];
          if (type==raster_INT) {
            ivb=vint[a];
            ivt=vint[b];
	  } else if (type==raster_FLOAT) {
            ivb=vfloat[a];
            ivt=vfloat[b];
	  } else {
            ivb=vdouble[a];
            ivt=vdouble[b];
	  }
        } else {
          ib=pnt[b*2+1];
          it=pnt[a*2+1];
          il=pnt[b*2];
          ir=pnt[a*2];
          if (type==raster_INT) {
            ivb=vint[b];
            ivt=vint[a];
	  } else if (type==raster_FLOAT) {
            ivb=vfloat[b];
            ivt=vfloat[a];
	  } else {
            ivb=vdouble[b];
            ivt=vdouble[a];
	  }
        } 
	
        if ((y<ib) || (y>it)) continue;
        /* calculate where line segment intersects y */

        if (it-ib==0) {
          itable[k].x=il;
          itable[k].n=i;
          itable[k].val=ivb;
          k++;
          itable[k].x=ir;
          itable[k].n=i;
          itable[k].val=ivt;
          k++;


        } else {
          ix=il+(ir-il)*(y-ib)/(it-ib);
          itable[k].x=ix;       
          itable[k].n=i; 
          iv=ivb+(ivt-ivb)*(y-ib)/(it-ib);
          itable[k].val=iv;    
          k++;
	}
      }
    }

    /* quick sort the array */

    if (k==0) continue;
  
    j=1;
    if (sparse==0) { 
      qsort((void *) itable,k,sizeof(struct RasterIsect),RasterCmp);
      l=itable[0].x;
      r=itable[k-1].x;
      if (l<0) l=0;
      if (r>wdt) r=wdt;
      for (x=l;x<r;x++) {
        while ((x>=itable[j].x) && (j<k)) j++;
        if (itable[j].x==itable[j-1].x) ivb=0;
        else ivb=( (double) (x-itable[j-1].x))/(itable[j].x-itable[j-1].x);
        val=itable[j-1].val+ivb*(itable[j].val-itable[j-1].val);
        if (type==raster_INT) zint[(int) y*wdt+(int) x]=(int) val;
        else if (type==raster_FLOAT) zfloat[(int) y*wdt+(int) x]=(float) val;
        else zdouble[(int) y*wdt+(int) x]=val;
      }
   } else {
      qsort((void *) itable,k,sizeof(struct RasterIsect),RasterSCmp);

      while (j<k) {
        if (itable[j].n !=itable[j-1].n) {
	  j++;
          continue;
        }
        
        r=itable[j].x;
        l=itable[j-1].x;
        if (l<0) l=0;
        if (r>wdt) r=wdt;     
        for (x=l;x<r;x++) {
          if (itable[j].x==itable[j-1].x) ivb=0;
          else ivb=( (double) (x-itable[j-1].x))/(itable[j].x-itable[j-1].x);
          val=itable[j-1].val+ivb*(itable[j].val-itable[j-1].val);
          if (type==raster_INT) zint[(int) y*wdt+(int) x]=(int) val;
          else if (type==raster_FLOAT) zfloat[(int) y*wdt+(int) x]=(float) val;
          else zdouble[(int) y*wdt+(int) x]=val;
        }
        j+=2;
      }
   } 
  }

  free(itable);
  free(ybbox);
  if (type==raster_INT) return zint;
  else if (type==raster_FLOAT) return zfloat;
  return zdouble;
}  
     



void *RasterHigh(int wdt,int hgt,int sparse,int type,void *zero,
             int poly,double *pnt,int *vertex,void *value) {
   
  int *zint=NULL;
  float *zfloat=NULL;
  double *zdouble=NULL;

  int *vint=NULL;
  float *vfloat=NULL;
  double *vdouble=NULL;
  
  struct RasterIsectDouble *itable=NULL;

  double *ybbox;
 
  int i,j,k,a,b,n;

  double l,r;
  double ix;
  double x,y;
  double ib,ir,it,il;
  double yb=1e6,yt=-1e6;

  double ivb,ivt,iv;
  double val;
 
  /* allocate memory for the zbuffer and bounding box
     point table and intersection table */

  if (type==raster_INT) zint=malloc(sizeof(int)*wdt*hgt);
  else if (type==raster_FLOAT) zfloat=malloc(sizeof(float)*wdt*hgt);
  else zdouble=malloc(sizeof(double)*wdt*hgt);

  if (type==raster_INT) vint=(int *) value;
  else if (type==raster_FLOAT) vfloat=(float *) value;
  else vdouble=(double *) value;
  

  ybbox=malloc(sizeof(double)*2*poly);
  
  itable=malloc(sizeof(struct RasterIsectDouble)*poly*4);

  if (type==raster_INT) for (i=0;i<wdt*hgt;i++) 
     zint[i]=*((int *) zero); /* zero array */
  else if (type==raster_FLOAT) for (i=0;i<wdt*hgt;i++) 
     zfloat[i]=*((float *) zero); /* zero array */
  else  for (i=0;i<wdt*hgt;i++) 
     zdouble[i]=*((double *) zero); /* zero array */
 
  /* generate bounding boxes */
  for (i=0;i<poly;i++) {
    ybbox[i*2]=1e6;
    ybbox[i*2+1]=-1e6;
    for (k=0;k<4;k++) {
       y=pnt[2*vertex[4*i+k]+1];
       if (y<yb) yb=y;
       if (y>yt) yt=y;

       if (y<ybbox[i*2]) ybbox[i*2]=y;
       if (y>ybbox[i*2+1]) ybbox[i*2+1]=y;
    }
  }
  
  /* scan zbuffer */

  if (yb<0) yb=0;
  if (yt>hgt) yt=hgt;
  
  for (y=(int) yb;y<(int) yt;y++) {
    
    k=0;
    for (i=0;i<poly;i++) {
      
      if ((y<ybbox[i*2]) || (y>ybbox[i*2+1])) continue;   
      
      for (n=1;n<=4;n++) {
        a=(n==4) ? 0: n;
        b=n-1;
        a=vertex[i*4+a];
        b=vertex[i*4+b]; 

        if ( pnt[a*2+1] < pnt[b*2+1]) {
          ib=pnt[a*2+1];
          it=pnt[b*2+1];
          il=pnt[a*2];
          ir=pnt[b*2];
          if (type==raster_INT) {
            ivb=vint[a];
            ivt=vint[b];
	  } else if (type==raster_FLOAT) {
            ivb=vfloat[a];
            ivt=vfloat[b];
	  } else {
            ivb=vdouble[a];
            ivt=vdouble[b];
	  }
        } else {
          ib=pnt[b*2+1];
          it=pnt[a*2+1];
          il=pnt[b*2];
          ir=pnt[a*2];
          if (type==raster_INT) {
            ivb=vint[b];
            ivt=vint[a];
	  } else if (type==raster_FLOAT) {
            ivb=vfloat[b];
            ivt=vfloat[a];
	  } else {
            ivb=vdouble[b];
            ivt=vdouble[a];
	  }
        } 
	
        if ((y<ib) || (y>it)) continue;
        /* calculate where line segment intersects y */

        if (it-ib==0) continue;
        else ix=il+(ir-il)*(y-ib)/(it-ib);

        itable[k].x=ix;       
        itable[k].n=i; 

        iv=ivb+(ivt-ivb)*(y-ib)/(it-ib);
        itable[k].val=iv;    
        k++;
      }
    }

    /* quick sort the array */

    if (k==0) continue;
  
    j=1;
    if (sparse==0) { 
      qsort((void *) itable,k,sizeof(struct RasterIsectDouble),
            RasterCmpDouble);
      l=itable[0].x;
      r=itable[k-1].x;
      if (l<0) l=0;
      if (r>wdt) r=wdt;
      for (x=l;x<r;x++) {
        while ((x>=itable[j].x) && (j<k)) j++;
        if (itable[j].x==itable[j-1].x) ivb=0;
        else ivb=( (double) (x-itable[j-1].x))/(itable[j].x-itable[j-1].x);
        val=itable[j-1].val+ivb*(itable[j].val-itable[j-1].val);
        if (type==raster_INT) zint[(int) y*wdt+(int) x]=(int) val;
        else if (type==raster_FLOAT) zfloat[(int) y*wdt+(int) x]=(float) val;
        else zdouble[(int) y*wdt+(int) x]=val;
      }
   } else {
      qsort((void *) itable,k,sizeof(struct RasterIsectDouble),
            RasterSCmpDouble);

      while (j<k) {
        if (itable[j].n !=itable[j-1].n) {
	  j++;
          continue;
        }
        
        r=itable[j].x;
        l=itable[j-1].x;
        if (l<0) l=0;
        if (r>wdt) r=wdt;     
        for (x=l;x<r;x++) {
          if (itable[j].x==itable[j-1].x) ivb=0;
          else ivb=( (double) (x-itable[j-1].x))/(itable[j].x-itable[j-1].x);
          val=itable[j-1].val+ivb*(itable[j].val-itable[j-1].val);
          if (type==raster_INT) zint[(int) y*wdt+(int) x]=(int) val;
          else if (type==raster_FLOAT) zfloat[(int) y*wdt+(int) x]=(float) val;
          else zdouble[(int) y*wdt+(int) x]=val;
        }
        j+=2;
      }
   } 
  }

  free(itable);
  free(ybbox);
  if (type==raster_INT) return zint;
  else if (type==raster_FLOAT) return zfloat;
  return zdouble;
}  
     
