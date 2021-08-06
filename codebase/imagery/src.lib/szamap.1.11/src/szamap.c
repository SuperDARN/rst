/* szamap.c
   ========
   Author: R.J.Barnes and others

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
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "rtime.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"
#include "polygon.h"
#include "raster.h"
#include "contour.h"
#include "sza.h"


double SZARound(double x) {
  double rem;
  double val;
  rem=fabs(x)-(int) fabs(x);
  rem=(rem>0.5) ? 1 : 0;
  val=(int) fabs(x)+rem;
  val=(x<0) ? -val : val;
  return val;
}


int SZATransform(int ssze,void *src,int dsze,void *dst,void *data) {
  float *pnt;
  float lat=0,lon=0;
  float step=((float *) data)[0];
  float flg=((float *) data)[1];

  pnt=(float *) src;
  
  lon=step*pnt[0]*360.0-180;
  if (flg==0) lat=-90+step*pnt[1]*180;
  if (flg==1) lat=step*pnt[1]*90.0;
  if (flg==-1) lat=-90+step*pnt[1]*90.0;
  pnt=(float *) dst;
  pnt[0]=lat;
  pnt[1]=lon;
  return 0;
}

struct PolygonData **SZAContour(int yr,int mo,int dy,int hr,int mt,int sc,
                                int flg,int mode,float step,int znum,
                                double *zenith)
{
  int s;
  float arg[2];
  int c=0;
  double *zbuffer;
  double dec;
  double LsoT; 
  double Hangle;
  double Z,eqt;
  double lat,lon;
  double tlon,tlat,r;

  struct PolygonData **ctr=NULL;

  int (*AACGM_Cnv)(double,double,double,double*,double*,double*,int);
 
  /* if mode !=0 then we are working in geomagnetic co-ordinates
   * and must transform to geographic when calculating zenith angle */
  /* if mode == 1 then default AACGM_v2 == 2 then old AACGM */
  if (mode == 1) AACGM_Cnv = &AACGM_v2_Convert;
  else           AACGM_Cnv = &AACGMConvert;

  arg[0]=step;
  arg[1]=flg;

  dec=SZASolarDec(yr,mo,dy,hr,mt,sc);
  eqt=SZAEqOfTime(yr,mo,dy,hr,mt,sc);
  
  zbuffer=malloc(sizeof(double)*360*182);
 
  c=0; 
  if (flg==0) {
    for (lat=-90;lat<=90;lat+=step) {
      for (lon=-180;lon<180;lon+=step) {

        if (mode==0) {
           tlat=lat;
           tlon=lon;
        } else
        {
            // TODO: comment what is going on here... 
            s = (*AACGM_Cnv)(lat,lon,0,&tlat,&tlon,&r,1);
            if (s == -1)
            {
                fprintf(stderr, "Error: AACGM convert returned an error\n");
            }
        }
        LsoT=(hr*3600+mt*60)+(tlon*4*60)+eqt;
        Hangle=15*((LsoT/3600)-12);
        Z=SZAAngle(tlon,tlat,dec,Hangle);
        zbuffer[c]=Z;
        c++;     
      }
    }
  } else if (flg==1) {
    for (lon=-180;lon<180;lon+=step) {
      zbuffer[c]=0;
      c++;
    }
    for (lat=step;lat<=90;lat+=step) {
      for (lon=-180;lon<180;lon+=step) {

        if (mode==0) {
           tlat=lat;
           tlon=lon;
        } else s = (*AACGM_Cnv)(lat,lon,0,&tlat,&tlon,&r,1);
        
        LsoT=(hr*3600+mt*60)+(tlon*4*60)+eqt;
        Hangle=15*((LsoT/3600)-12);
        Z=SZAAngle(tlon,tlat,dec,Hangle);
        zbuffer[c]=Z;
        c++;     
      }
    }
  } else {
    for (lat=-90;lat<0;lat+=step) {
      for (lon=-180;lon<180;lon+=step) {

        if (mode==0) {
           tlat=lat;
           tlon=lon;
        } else s = (*AACGM_Cnv)(lat,lon,0,&tlat,&tlon,&r,1);
        
        LsoT=(hr*3600+mt*60)+(tlon*4*60)+eqt;
        Hangle=15*((LsoT/3600)-12);
        Z=SZAAngle(tlon,tlat,dec,Hangle);
        zbuffer[c]=Z;
        c++;     
      }
    }
    for (lon=-180;lon<180;lon+=step) {
      zbuffer[c]=0;
      c++;
    }
  }
  /* okay now generate contours */
  if (flg==0) ctr=Contour(zbuffer,contour_DOUBLE,360,181,znum,
              zenith,3,0,1,0x00);
  else ctr=Contour(zbuffer,contour_DOUBLE,360,91,znum,
              zenith,3,0,1,0x00);

  /* transform them so that they are stored in longitude and latitudes */

  free(zbuffer);

  ContourModify(znum,ctr,SZATransform,arg);
  return ctr;
}

struct PolygonData *SZATerminator(int yr,int mo,int dy,int hr,int mt,int sc,
                                    int flg,int mode,float step,
                                    float zenith) {

  double cval[1]={102};
  struct PolygonData **ctr;
  struct PolygonData *trm=NULL;
  if (zenith !=0) cval[0]=zenith;
 
  if (flg==0) {
    struct PolygonData *ptr=NULL;
    float pp[2],lon;
    float *p,*o;
    int n;
    int asc=0;
    int dsc=0;
    int num;

    ctr=SZAContour(yr,mo,dy,hr,mt,sc,0,mode,step,1,cval);
    ptr=ctr[0];
    num=ptr->num[0];

    trm=PolygonMake(sizeof(float)*2,NULL);
    PolygonAddPolygon(trm,1);


    p=PolygonRead(ptr,0,0);
    PolygonAdd(trm,p);

    PolygonAddPolygon(trm,2);
    for (n=1;n<num;n++) {
      o=PolygonRead(ptr,0,n-1);
      p=PolygonRead(ptr,0,n);
      PolygonAdd(trm,p);

      if ((o[0]<0) && (p[0]>0)) asc=n;    
      if ((o[0]>0) && (p[0]<0)) dsc=n;    
    }


    trm=PolygonMake(sizeof(float)*2,NULL);

    n=asc;
    PolygonAddPolygon(trm,2);
    p=PolygonRead(ptr,0,n);   
    lon=SZARound(p[1]);
    if (lon>=180) lon-=360;
    while (n !=dsc) {
    
      p=PolygonRead(ptr,0,n);
      PolygonAdd(trm,p);
      n++;
      if (n>=num) n=0;
    } 
    pp[0]=0;
    pp[1]=p[1];
    PolygonAdd(trm,pp);
    pp[1]=SZARound(p[1]);
    do {
       PolygonAdd(trm,pp);
       pp[1]--;
       if (pp[1]<-180) pp[1]+=360;
    } while (pp[1] != lon);

    p=PolygonRead(ptr,0,n);   
    lon=SZARound(p[1]);
    if (lon>=180) lon-=180;
    PolygonAddPolygon(trm,2);
    while (n !=asc) {
      p=PolygonRead(ptr,0,n);
      PolygonAdd(trm,p);
      n++;
      if (n>=num) n=0;
    } 
    pp[0]=0;
    pp[1]=p[1];
    PolygonAdd(trm,pp);
    pp[1]=SZARound(p[1]);
    do {
       PolygonAdd(trm,pp);
       pp[1]++;
       if (pp[1]>=180) pp[1]-=360;

    } while (pp[1] != lon);

    ContourFree(1,ctr);
  } else {
    int num,n=0;
    float *p;
    struct PolygonData *ptr=NULL;
    ctr=SZAContour(yr,mo,dy,hr,mt,sc,flg,mode,step,1,cval);

    ptr=ctr[0];
    num=ptr->num[0];

    trm=PolygonMake(sizeof(float)*2,NULL);
    PolygonAddPolygon(trm,1);
    for (n=0;n<num;n++) {
      p=PolygonRead(ptr,0,n);
      PolygonAdd(trm,p);
    }

    PolygonAddPolygon(trm,2);
    for (n=0;n<num;n++) {
      p=PolygonRead(ptr,0,n);
      PolygonAdd(trm,p);
    }

    ContourFree(1,ctr);
  }
 
    return trm;
}


float *SZAMap(int yr,int mo,int dy,int hr,int mt,int sc,float latmin,
              int wdt,int hgt,int mode,
              int (*trf)(int ssze,void *src,int dsze, void *dst,void *data),
              void *data)
{
  float lat,lon;
  float xstep=1.0;
  float ystep=1.0;
  int xnum,ynum;

  int x,y;
  int s,i,j;
  float *pnt=NULL;
  float *zbuf=NULL;
  int *vertex=NULL;
  int *mapping=NULL;
  float *image=NULL;
  int num=0,tnum,poly=0,tpoly;
  float zeroval=-400;

  double LsoT;
  double Hangle;
  double Z,eqt;
  double dec;

  double tlon,tlat,r;

  int (*AACGM_Cnv)(double,double,double,double*,double*,double*,int);

  /* if mode > 0 then default AACGM_v2 else AACGM */
  if (mode > 0) AACGM_Cnv = &AACGM_v2_Convert;
  else          AACGM_Cnv = &AACGMConvert;

  dec=SZASolarDec(yr,mo,dy,hr,mt,sc);
  eqt=SZAEqOfTime(yr,mo,dy,hr,mt,sc);

  xnum=(360/xstep)+1;
  ynum=180.0/ystep+1;

  zbuf=malloc(sizeof(float)*xnum*ynum);
  pnt=malloc(sizeof(float)*xnum*ynum*2);
  vertex=malloc(sizeof(int)*xnum*ynum*4);
  mapping=malloc(sizeof(int)*xnum*ynum);

  for (y=0;y<ynum;y++) {
    for (x=0;x<xnum;x++) {
      lon=x*xstep;
      lat=-90+y*ystep;

      if (mode==0) {
        tlat=lat;
        tlon=lon;
      } else s = (*AACGM_Cnv)(lat,lon,0,&tlat,&tlon,&r,1);

      LsoT=(hr*3600+mt*60+sc)+(tlon*4*60)+eqt;
      Hangle=15*((LsoT/3600)-12);
      Z=SZAAngle(tlon,tlat,dec,Hangle);
      
      if ((y !=ynum-1) && (x !=xnum-1)) {
        vertex[4*poly]=num;
        vertex[4*poly+1]=num+1;
        vertex[4*poly+2]=vertex[4*poly+1]+xnum;
        vertex[4*poly+3]=num+xnum;
        poly++;
      }

      zbuf[num]=Z;
      pnt[2*num]=lat;
      pnt[2*num+1]=lon;
      num++;
    }
  }
 
  tnum=0;
  for (i=0;i<num;i++) {
    mapping[i]=-1;
    s=(*trf)(2*sizeof(float),&pnt[2*i],2*sizeof(float),
              &pnt[2*tnum],data);
    if (s !=0) continue;
    mapping[i]=tnum;
    zbuf[tnum]=zbuf[i];
    pnt[2*tnum]=pnt[2*tnum]*wdt;
    pnt[2*tnum+1]=pnt[2*tnum+1]*hgt;
    tnum++;
  }

  tpoly=0;
  for (i=0;i<poly;i++) {
    for (j=0;j<4;j++) {
      if (mapping[vertex[4*i+j]]==-1) break;
      vertex[4*tpoly+j]=mapping[vertex[4*i+j]];
    }
    if (j<4) continue;
    tpoly++;
  }

  image=Raster(wdt,hgt,0,raster_FLOAT,&zeroval,tpoly,pnt,vertex, zbuf);
  
  free(pnt);
  free(vertex);
  free(zbuf);
  free(mapping);
  return image;

}



