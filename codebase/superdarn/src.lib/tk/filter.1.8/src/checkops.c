/* checkops.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "rawdata.h"
#include "fitdata.h"
#include "scandata.h"
#include "filter.h"




int FilterCheckOps(int depth,struct RadarScan **ptr,int fmax) {
  int bm,bt,num,t;
  for (num=0;num<depth;num++) {
    if (num==depth/2) continue;
    for (bm=0;bm<ptr[depth/2]->num;bm++) {
      bt=ptr[depth/2]->bm[bm].bm;
      for (t=0;t<ptr[num]->num;t++) {
        if (ptr[num]->bm[t].bm !=bt) continue;
        if (ptr[num]->bm[t].frang !=ptr[depth/2]->bm[bm].frang) return -1;
        if (ptr[num]->bm[t].rsep !=ptr[depth/2]->bm[bm].rsep) return -1;
        if ((fmax !=0) && 
           (fabs(ptr[num]->bm[t].freq-ptr[depth/2]->bm[bm].freq)>fmax)) 
           return -1;
      }
    }
  }
  return 0;
}





