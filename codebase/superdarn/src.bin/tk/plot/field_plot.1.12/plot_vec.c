/* plot_vec.c
   ========== 
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
#include "calcvector.h"
#include "geobeam.h"
#include "plot_cell.h"





void plot_vec(struct Plot *plot,struct RadarBeam *sbm,
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
