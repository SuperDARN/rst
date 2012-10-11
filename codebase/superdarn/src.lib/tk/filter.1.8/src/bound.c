/* bound.c
   ======= 
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
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "scandata.h"
#include "filter.h"




int FilterBoundType(struct RadarScan *ptr,int type) {
  int bm,rng;
  for (bm=0;bm<ptr->num;bm++) {
    for (rng=0;rng<ptr->bm[bm].nrang;rng++) {
      if (ptr->bm[bm].sct[rng]==0) continue;
      if (ptr->bm[bm].rng[rng].gsct==type) ptr->bm[bm].sct[rng]=0;  
        
    }
  }
  return 0;
}

int FilterBound(int prm,struct RadarScan *ptr,double *min,double *max) {
  int bm,rng;
  for (bm=0;bm<ptr->num;bm++) {
    for (rng=0;rng<ptr->bm[bm].nrang;rng++) {
       
      if (ptr->bm[bm].sct[rng]==0) continue;
     
      if (fabs(ptr->bm[bm].rng[rng].v)<min[0]) ptr->bm[bm].sct[rng]=0;
      if (fabs(ptr->bm[bm].rng[rng].v)>max[0]) ptr->bm[bm].sct[rng]=0;

      if (ptr->bm[bm].rng[rng].p_l<min[1]) ptr->bm[bm].sct[rng]=0;
      if (ptr->bm[bm].rng[rng].p_l>max[1]) ptr->bm[bm].sct[rng]=0;
 
      if (ptr->bm[bm].rng[rng].w_l<min[2]) ptr->bm[bm].sct[rng]=0;
      if (ptr->bm[bm].rng[rng].w_l>max[2]) ptr->bm[bm].sct[rng]=0;

      if (ptr->bm[bm].rng[rng].v_e<min[3]) ptr->bm[bm].sct[rng]=0;
      if (ptr->bm[bm].rng[rng].v_e>max[3]) ptr->bm[bm].sct[rng]=0;
        
   }
  }
  return 0;
}
