/* plot_vec.c
   ========== 
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
#include "radar.h"
#include "calcvector.h"
#include "griddata.h"
#include "scandata.h"
#include "geobeam.h"
#include "plot_cell.h"
#include "text_box.h"


void plot_field_vec(struct Plot *plot,struct RadarBeam *sbm,
                    struct GeoLocBeam *gbm,float latmin,int magflg,
                    float xoff,float yoff,float wdt,float hgt,float sf,
                    int (*trnf)(int,void *,int,void *,void *data),void *data,
                    unsigned int(*cfn)(double,void *),void *cdata,
                    unsigned int gscol,unsigned char gsflg,float width,float rad) {

  int rng;
  unsigned int color=0;
  float ax=0,ay=0,bx=0,by=0;
  int s=0;
  float map[2],pnt[2];
  double lat,lon;
  for (rng=0;rng<sbm->nrang;rng++) {
    if (sbm->sct[rng]==0) continue;

    if (cfn !=NULL) color=(*cfn)(fabs(sbm->rng[rng].v),cdata);
    if ((gsflg) && (sbm->rng[rng].gsct !=0)) color=gscol;
     
    if (magflg) {
       map[0]=gbm->mlat[1][rng];
       map[1]=gbm->mlon[1][rng];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1];     

       if (sbm->rng[rng].gsct==0) {
         RPosCalcVector(map[0],map[1],sbm->rng[rng].v*sf,
                      gbm->mazm[rng],&lat,&lon);


         map[0]=lat;
         map[1]=lon;
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) continue;
         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];
       }     
     } else {
       map[0]=gbm->glat[1][rng];
       map[1]=gbm->glon[1][rng];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1];     
       if (sbm->rng[rng].gsct==0) {
      
         RPosCalcVector(map[0],map[1],sbm->rng[rng].v*sf,
                        gbm->gazm[rng],&lat,&lon);


         map[0]=lat;
         map[1]=lon;
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) continue;
         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];     
       }

    }
   
    if (sbm->rng[rng].gsct==0)
      PlotLine(plot,ax,ay,bx,by,color,0x0f,width,NULL);    
    PlotEllipse(plot,NULL,ax,ay,
                 rad,rad,1,color,0x0f,0,NULL);
    
  } 
}



void plot_grid_vec(struct Plot *plot,float px,float py,int or,double max, int magflg,
                   float xoff,float yoff,float wdt,float hgt,float sf,float rad,
                   int (*trnf)(int,void *,int,void *,void *data),void *data,
                   unsigned int color,char mask,float width,
                   char *fontname,float fontsize, void *txtdata, int old_aacgm)
{
  int s;
  char txt[256];
  double lon,lat;
  float mag;
  float map[2],pnt[2];
  float ax,ay,bx,by;
  float txbox[3];
  float *arg;
  double mlat,mlon,glat,glon,r;

  lat = 90.0;
  lon = 0.0;

  if (data != NULL) {
    arg = (float *)data;
    if (arg[0] >= 90) lat =  90.0;
    else              lat = -90.0;
  }

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
  if (s == -1) return;
  ax = xoff + pnt[0]*wdt;
  ay = yoff + pnt[1]*hgt;    

  /* need v2 here! */
  RPosCalcVector(lat,0.0,max*sf,0.0,&lat,&lon);

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
  if (s == -1) return;

  bx = xoff + pnt[0]*wdt;
  by = yoff + pnt[1]*hgt;    

  mag = sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));

  if (rad > 0) PlotEllipse(plot,NULL,px,py, rad,rad,1,color,mask,0,NULL);

  if (or == 0) PlotLine(plot,px,py,px+mag,py,color,0x0f,width,NULL);    
  else if (or == 1) PlotLine(plot,px,py,px-mag,py,color,0x0f,width,NULL);    
  else if (or == 2) PlotLine(plot,px,py-mag,px,py,color,0x0f,width,NULL);    
  else if (or == 3) PlotLine(plot,px,py,px,py+mag,color,0x0f,width,NULL);    

  sprintf(txt,"%g m/s",max);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);

  if (or == 0) PlotText(plot,NULL,fontname,fontsize,px-4,py-(txbox[2]-txbox[1]),
                       strlen(txt),txt,color,mask,1);
}

