/* plot_cell.c
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

/* Notes:
 *
 * - added old_aacgm parameter
 * - altitude is assumed to be 150 km
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rmath.h"
#include "rtypes.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "rfile.h"
#include "calcvector.h"
#include "griddata.h"
#include "radar.h"
#include "scandata.h"
#include "geobeam.h"


int cell_convert(float xoff,float yoff,float wdt,float hgt,
                 float lat,float lon,float *px,float *py,int magflg,
                 int (*trnf)(int,void *,int,void *,void *data),
                 void *data, int old_aacgm)
{
  int s;
  double mlat,mlon,glat,glon,r;
  float map[2],pnt[2];

  if (!magflg) {
    mlat = lat;
    mlon = lon;
    if (old_aacgm) s = AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
    else           s = AACGM_v2_Convert(mlat,mlon,150,&glat,&glon,&r,1);
    lat = glat;
    lon = glon;
  }
  map[0] = lat;
  map[1] = lon;
  s = (*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  if (s != 0) return -1;
  *px = xoff + wdt*pnt[0];
  *py = yoff + hgt*pnt[1]; 

  return 0;
}


void plot_field_cell(struct Plot *plot,struct RadarBeam *sbm,
                     struct GeoLocBeam *gbm,float latmin,int magflg,
                     float xoff,float yoff,float wdt,float hgt,
                     int (*trnf)(int,void *,int,void *,void *data),void *data,
                     unsigned int(*cfn)(double,void *),void *cdata,
                     int prm,unsigned int gscol,unsigned char gsflg) {

  int rng;
  unsigned int color=0;
  float px[4],py[4];
  int s=0;
  int t[4]={0,0,0,0};
  float map[2],pnt[2];
  for (rng=0;rng<sbm->nrang;rng++) {
    if ((sbm->sct[rng]==0) && (prm !=8)) continue;

    if (cfn !=NULL) {
      if (prm==1) color=(*cfn)(sbm->rng[rng].p_l,cdata);
      else if (prm==2)  color=(*cfn)(sbm->rng[rng].v,cdata);
      else if (prm==3)  color=(*cfn)(sbm->rng[rng].w_l,cdata);
      else if (prm==4)  color=(*cfn)(sbm->rng[rng].phi0,cdata);
      else if (prm==5)  color=(*cfn)(sbm->rng[rng].elv,cdata);
      else if (prm==6)  color=(*cfn)(sbm->rng[rng].v_e,cdata);
      else if (prm==7)  color=(*cfn)(sbm->rng[rng].w_l_e,cdata);
      else color=(*cfn)(sbm->rng[rng].p_0,cdata);
    }

    if ((prm==2) && (gsflg) && (sbm->rng[rng].gsct !=0)) color=gscol;

    if (magflg) {
      map[0]=gbm->mlat[0][rng];
      map[1]=gbm->mlon[0][rng];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[0]=xoff+wdt*pnt[0];
      py[0]=yoff+hgt*pnt[1];
      map[0]=gbm->mlat[2][rng];
      map[1]=gbm->mlon[2][rng];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[1]=xoff+wdt*pnt[0];
      py[1]=yoff+hgt*pnt[1];
      map[0]=gbm->mlat[2][rng+1];
      map[1]=gbm->mlon[2][rng+1];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[2]=xoff+wdt*pnt[0];
      py[2]=yoff+hgt*pnt[1];
      map[0]=gbm->mlat[0][rng+1];
      map[1]=gbm->mlon[0][rng+1];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[3]=xoff+wdt*pnt[0];
      py[3]=yoff+hgt*pnt[1];
    } else {
      map[0]=gbm->glat[0][rng];
      map[1]=gbm->glon[0][rng];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[0]=xoff+wdt*pnt[0];
      py[0]=yoff+hgt*pnt[1];
      map[0]=gbm->glat[2][rng];
      map[1]=gbm->glon[2][rng];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[1]=xoff+wdt*pnt[0];
      py[1]=yoff+hgt*pnt[1];
      map[0]=gbm->glat[2][rng+1];
      map[1]=gbm->glon[2][rng+1];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[2]=xoff+wdt*pnt[0];
      py[2]=yoff+hgt*pnt[1];
      map[0]=gbm->glat[0][rng+1];
      map[1]=gbm->glon[0][rng+1];
      s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      if (s !=0) continue;
      px[3]=xoff+wdt*pnt[0];
      py[3]=yoff+hgt*pnt[1];
    }

    PlotPolygon(plot,NULL,0,0,4,px,py,t,1,color,0x0f,0,NULL);

  }
}


void plot_grid_cell(struct Plot *plot,struct GridData *ptr,float latmin,int magflg,
                    float xoff,float yoff,float wdt,float hgt,
                    int (*trnf)(int,void *,int,void *,void *data),void *data,
                    unsigned int(*cfn)(double,void *),void *cdata, int cprm,
                    int old_aacgm)
{
  int i,s,nlon;
  double lon,lat,lstp;
 
  unsigned int color=0;
  float px[4],py[4];
  int t[4]={0,0,0,0};

  for (i=0;i<ptr->vcnum;i++) {
    if (cfn !=NULL) {
      if (cprm==0) color=(*cfn)(ptr->data[i].pwr.median,cdata);
      else if (cprm==2) color=(*cfn)(ptr->data[i].wdt.median,cdata);
      else color=(*cfn)(ptr->data[i].srng,cdata);
    }
    lon=ptr->data[i].mlon;
    lat=ptr->data[i].mlat;
    if (abs(lat)<abs(latmin)) continue;
    nlon=(int) (360*cos((lat-0.5)*PI/180)+0.5);
    lstp=360.0/nlon; 
    s=cell_convert(xoff,yoff,wdt,hgt,lat-0.5,lon-lstp/2,&px[0],&py[0],
                 magflg,trnf,data,old_aacgm);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat-0.5,lon+lstp/2,&px[1],&py[1],
                 magflg,trnf,data,old_aacgm);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat+0.5,lon+lstp/2,&px[2],&py[2],
                 magflg,trnf,data,old_aacgm);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat+0.5,lon-lstp/2,&px[3],&py[3],
                 magflg,trnf,data,old_aacgm);
    if (s !=0) continue;   
    PlotPolygon(plot,NULL,0,0,4,px,py,t,1,color,0x0f,0,NULL);
  } 
}

