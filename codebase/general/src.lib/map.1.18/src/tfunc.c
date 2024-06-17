/* transform.c
   =========== 
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
#include <string.h>
#include <math.h> 
#include <sys/types.h>
#include "rtypes.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "rmath.h"
#include "rmap.h"



int MapCylindrical(int ssze,void *src,int dsze,void *dst,void *data) {

  float *pnt;
  float lat,lon;
  float plon=0;
  int wrap=1;
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (data !=NULL) plon=((float *) data)[1];
  if (data !=NULL) wrap=((float *) data)[2];
  lon=lon-plon;
  pnt=(float *)dst;

  pnt[0]=lon/360.0;
  
  if (wrap) {
    if (pnt[0]<0) pnt[0]+=1.0;
    if (pnt[0]>=1) pnt[0]-=1.0;
  }
  pnt[1]=0.5-lat/180.0;
  return 0;
}



int MapMercator(int ssze,void *src,int dsze,void *dst,void *data) {

  float *pnt;
  float lat,lon;
  float plon=0;
  float wrap=1;

  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (data !=NULL) plon=((float *) data)[1];
  if (data !=NULL) wrap=((float *) data)[2];

  lon=lon-plon;
 
  pnt=(float *)dst;

  pnt[0]=lon/360.0;
  if (wrap) {
    if (pnt[0]<0) pnt[0]+=1.0;
    if (pnt[0]>=1) pnt[0]-=1.0;
  }
  if (lat==90) pnt[1]=0.0;
  else if (lat==-90) pnt[1]=1.0;
  else pnt[1]=0.5-0.25*log(tan(PI/4+lat*PI/360));
  return 0;
}


int MapTransverseMercator(int ssze,void *src,int dsze,void *dst,void *data) {

  float B;
  float plon=0;
  float *pnt;
  float lat,lon;
  float wrap=1;

  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (data !=NULL) plon=((float *) data)[1];
  if (data !=NULL) wrap=((float *) data)[2];

  lon=lon-plon;
  if (wrap) {
    if (lon<0) lon+=360;
    if (lon>=360) lon-=360;
  }
  B=cos(PI*lat/180.0)*sin(PI*lon/180.0);
  if (B==1)  return -1;
  if (B==-1) return -1;

  pnt=(float *)dst;
  pnt[1]=0.5+0.5*(1.0/(2.0*PI))*log((1+B)/(1-B));
  pnt[0]=0.5+(1.0/(2.0*PI))*atan2(tan(PI*lat/180.0),cos(PI*lon/180.0));
  return 0;
} 
  
int MapStereographic(int ssze,void *src,int dsze,void *dst,void *data) {
  float *pnt;
 
  float k=1.0,f=0;
  float z;
  float plon=0;
  float plat=0;
  float lat,lon;
  float xshift=0;
  float yshift=0;
 
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (isinf(pnt[0])) return -1;
  if (isinf(pnt[1])) return -1;

  if (data !=NULL) {
    float *arg;
    arg=(float *) data;
    plat=arg[0];
    plon=arg[1];
    k=arg[2];
    f=arg[3];
    xshift=arg[4];
    yshift=arg[5];
  }
 
  lat=lat*PI/180;
  lon=lon*PI/180;
  plat=plat*PI/180;
  plon=plon*PI/180;
 
  if ((plat>0) && (lat<=0)) return -1;
  if ((plat<0) && (lat>=0)) return -1;

  z=sin(plat)*sin(lat)+cos(plat)*cos(lat)*cos(lon-plon);
  if (z==-1) return -1;

  k=2*k/(1+z);

  pnt=(float *) dst;
  if (f==0) pnt[0]=(1.0+k*cos(lat)*sin(lon-plon))/2.0 + xshift;
  else pnt[0]=(1.0-k*cos(lat)*sin(lon-plon))/2.0 + xshift;
  pnt[1]=(1.0-k*(cos(plat)*sin(lat)-sin(plat)*cos(lat)*
                 cos(lon-plon)))/2.0 + yshift;
 
 return 0;
}


int MapOrthographic(int ssze,void *src,int dsze,void *dst,void *data) {
  float *pnt;
  float z=0,f=0;
  float r=1.0;
  float sf=1.0;
  float plat=0,plon=0;

  float lat,lon;
 
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (isinf(pnt[0])) return -1;
  if (isinf(pnt[1])) return -1;

  if (data !=NULL) {
    float *arg;
    arg=(float *) data;
    plat=arg[0];
    plon=arg[1];
    sf=arg[2];
    f=arg[3];
  }

  z=sin(plat*PI/180)*sin(lat*PI/180)+
   cos(plat*PI/180)*cos(lat*PI/180)*
   cos(PI*(lon-plon)/180);

  if (z<0) return -1;
 
   pnt=(float *) dst;

  if (f==0) pnt[0]=(1.0+sf*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180))/2.0;
  else pnt[0]=(1.0-sf*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180))/2.0;
  pnt[1]=(1.0-sf*r*(cos(PI*plat/180)*
       sin(PI*lat/180)-
       sin(PI*plat/180)*cos(PI*lat/180)*
       cos(PI*(lon-plon)/180)))/2.0;
 return 0;
}



int MapGeneralVerticalPerspective(int ssze,void *src,int dsze,
                                  void *dst,void *data) {
  float *pnt;
  float z=0,f=0;
  float r=1.0;
  float K=1.0,P=0;
  float plat=0,plon=0;

  float lat,lon;
 
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];
 
  if (data !=NULL) {
    float *arg;
    arg=(float *) data;
    plat=arg[0];
    plon=arg[1];
    P=arg[2];
    f=arg[3];
  }


  z=sin(plat*PI/180)*sin(lat*PI/180)+
   cos(plat*PI/180)*cos(lat*PI/180)*
   cos(PI*(lon-plon)/180);

  if (z<(1.0/P)) return -1;

  K=(P-1)/(P-z);
  r=P/(P-1); 
  pnt=(float *) dst;

  if (f==0) pnt[0]=0.5+0.5*K*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180);
  else pnt[0]=0.5-0.5*K*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180);
  pnt[1]=0.5-0.5*K*r*(cos(PI*plat/180)*
       sin(PI*lat/180)-
       sin(PI*plat/180)*cos(PI*lat/180)*
       cos(PI*(lon-plon)/180));
 return 0;
}





