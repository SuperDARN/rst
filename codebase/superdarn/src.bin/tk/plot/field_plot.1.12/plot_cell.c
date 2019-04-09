/* plot_cell.c
   =========== 
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
#include "plot_cell.h"





void plot_cell(struct Plot *plot,struct RadarBeam *sbm,
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
    if (sbm->sct[rng]==0) continue;

    if (cfn !=NULL) {
      if (prm==1) color=(*cfn)(sbm->rng[rng].p_l,cdata);
      else if (prm==2)  color=(*cfn)(sbm->rng[rng].v,cdata);
      else color=(*cfn)(sbm->rng[rng].w_l,cdata);
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
