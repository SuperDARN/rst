/* fitcfit.c
   =========
   Author: R.J.Barnes
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"



int FitToCFit(double min_pwr,struct CFitdata *ptr,
              struct RadarParm *prm,
              struct FitData *fit) {
  
  int i,num=0,rng;
  ptr->version.major=CFIT_MAJOR_REVISION;
  ptr->version.minor=CFIT_MINOR_REVISION;

  /* time stamp the record */
  ptr->time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 

 
   
   ptr->stid=prm->stid;
   ptr->scan=prm->scan;  
   ptr->cp=prm->cp;
   ptr->bmnum=prm->bmnum;
   ptr->bmazm=prm->bmazm;
   ptr->channel=prm->channel; 
   ptr->intt.sc=prm->intt.sc;
   ptr->intt.us=prm->intt.us;
   ptr->frang=prm->frang;
   ptr->rsep=prm->rsep;
   ptr->rxrise=prm->rxrise;
   ptr->tfreq=prm->tfreq;
   ptr->noise=prm->noise.search;
   ptr->atten=prm->atten;
   ptr->nave=prm->nave;
   ptr->nrang=prm->nrang;

   

   for (i=0;i<prm->nrang;i++) {
    if (fit->rng[i].qflg!=1) continue; 
    if ((min_pwr !=0) && (fit->rng[i].p_0 <= min_pwr)) continue;
    num++;
  }

  CFitSetRng(ptr,num);
  num=0;
  for (i=0;i<prm->nrang;i++) {
    if (fit->rng[i].qflg!=1) continue;
    if ((min_pwr !=0) && (fit->rng[i].p_0 <= min_pwr)) continue;
    ptr->rng[num]=i;
    num++;
  }
  if (num>0) {
    for (i=0;i<num;i++) {
      rng=ptr->rng[i];
      ptr->data[i].gsct=fit->rng[rng].gsct;
      ptr->data[i].p_0=fit->rng[rng].p_0;
      ptr->data[i].p_0_e=0;
      ptr->data[i].v=fit->rng[rng].v;
      ptr->data[i].v_e=fit->rng[rng].v_err;
      ptr->data[i].p_l=fit->rng[rng].p_l;
      ptr->data[i].p_l_e=fit->rng[rng].p_l_err;
      ptr->data[i].w_l=fit->rng[rng].w_l;
      ptr->data[i].w_l_e=fit->rng[rng].w_l_err;
    }
  }
  return 0;
}
 






