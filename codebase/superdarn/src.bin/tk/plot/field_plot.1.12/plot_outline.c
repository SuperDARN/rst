/* plot_outline.c
   ============== 
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
#include "rmath.h"
#include "rtypes.h"
#include "aacgm.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "rfile.h"
#include "radar.h"
#include "scandata.h"
#include "geobeam.h"
#include "plot_outline.h"





void plot_outline(struct Plot *plot,struct RadarBeam *sbm,
                  struct GeoLocBeam *gbm,float latmin,int magflg,
                  int minbeam,int maxbeam,
                  float xoff,float yoff,float wdt,float hgt,
                  int (*trnf)(int,void *,int,void *,void *data),
                  void *data,unsigned int color) {

  float ax=0,ay=0,bx=0,by=0;
  int s=0;
  int nrang,rng;
  float map[2],pnt[2];

  nrang=sbm->nrang;

  if (gbm->bm==minbeam) {
    for (rng=0;rng<nrang;rng++) {
      if (magflg) {
         map[0]=gbm->mlat[0][rng];
         map[1]=gbm->mlon[0][rng];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;
         ax=xoff+wdt*pnt[0];
         ay=yoff+hgt*pnt[1]; 
    
         map[0]=gbm->mlat[0][rng+1];
         map[1]=gbm->mlon[0][rng+1];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;

         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];         
      } else {
         map[0]=gbm->glat[0][rng];
         map[1]=gbm->glon[0][rng];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;
         ax=xoff+wdt*pnt[0];
         ay=yoff+hgt*pnt[1];     
         map[0]=gbm->glat[0][rng+1];
         map[1]=gbm->glon[0][rng+1];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;
         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];   
      }
      PlotLine(plot,ax,ay,bx,by,color,0x0f,0,NULL);
    }
  }


  if (gbm->bm==maxbeam-1) {
    for (rng=0;rng<nrang;rng++) {
      if (magflg) {
         map[0]=gbm->mlat[2][rng];
         map[1]=gbm->mlon[2][rng];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;
         ax=xoff+wdt*pnt[0];
         ay=yoff+hgt*pnt[1]; 
    
         map[0]=gbm->mlat[2][rng+1];
         map[1]=gbm->mlon[2][rng+1];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;

         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];         
      } else {
         map[0]=gbm->glat[2][rng];
         map[1]=gbm->glon[2][rng];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;
         ax=xoff+wdt*pnt[0];
         ay=yoff+hgt*pnt[1];     
         map[0]=gbm->glat[2][rng+1];
         map[1]=gbm->glon[2][rng+1];
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) return;
         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];   
      }
      PlotLine(plot,ax,ay,bx,by,color,0x0f,0,NULL);
    }
  }

  if ((gbm->bm < minbeam) || (gbm->bm >= maxbeam)) return;

  if (magflg) {
       map[0]=gbm->mlat[0][0];
       map[1]=gbm->mlon[0][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) return;
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1]; 
    
       map[0]=gbm->mlat[2][0];
       map[1]=gbm->mlon[2][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) return;

       bx=xoff+wdt*pnt[0];
       by=yoff+hgt*pnt[1];         
    } else {
       map[0]=gbm->glat[0][0];
       map[1]=gbm->glon[0][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) return;
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1];     
       map[0]=gbm->glat[2][0];
       map[1]=gbm->glon[2][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) return;
       bx=xoff+wdt*pnt[0];
       by=yoff+hgt*pnt[1];   
    }
   
    PlotLine(plot,ax,ay,bx,by,color,0x0f,0,NULL);

    if (magflg) {
       map[0]=gbm->mlat[0][nrang];
       map[1]=gbm->mlon[0][nrang];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
    
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1]; 
       map[0]=gbm->mlat[2][nrang];
       map[1]=gbm->mlon[2][nrang];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
      
       bx=xoff+wdt*pnt[0];
       by=yoff+hgt*pnt[1];         
    } else {
       map[0]=gbm->glat[0][nrang];
       map[1]=gbm->glon[0][nrang];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
     
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1];     
       map[0]=gbm->glat[2][nrang];
       map[1]=gbm->glon[2][nrang];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
     
       bx=xoff+wdt*pnt[0];
       by=yoff+hgt*pnt[1];   
    }
   
    PlotLine(plot,ax,ay,bx,by,color,0x0f,0,NULL);


    
         
}




void plot_filled(struct Plot *plot,struct RadarBeam *sbm,
               struct GeoLocBeam *gbm,float latmin,int magflg,
               float xoff,float yoff,float wdt,float hgt,
               int (*trnf)(int,void *,int,void *,void *data),void *data,
               unsigned int color) {

  int rng;
  float px[4],py[4];
  int s=0;
  int t[4]={0,0,0,0};
  float map[2],pnt[2];
  int nrang=75;
  nrang=sbm->nrang;
  
  for (rng=0;rng<nrang;rng++) {
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
