/* cfit.c
   =====
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
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "cfitdata.h"

#include "cfitread.h"
#include "cfitindex.h"
#include "cfitseek.h"
#include "tplot.h"



double cfit_find(struct CFitfp *cfitfp,struct CFitdata *cfit,
                double sdate,
                double stime) {
  int status;
  int yr,mo,dy,hr,mt;
  double sc;
  double atime;

  do {
    status=CFitRead(cfitfp,cfit);
    if (status==-1) break;
  } while (status !=0);
  if (status==-1) return -1;
  
  atime=cfit->time;

  if ((stime==-1) && (sdate==-1)) return atime;
  if (stime==-1) stime= ( (int) atime % (24*3600));
  if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
  else stime+=sdate;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  if (atime>=stime) return stime;

  status=CFitSeek(cfitfp,yr,mo,dy,hr,mt,sc,NULL,NULL);
  if (status==-1) return -1;
  
  do {
    status=CFitRead(cfitfp,cfit);
    if (status==-1) break;
  } while (status !=0);
  if (status==-1) return -1;
  return stime;
}

double cfit_scan(double ctime,struct CFitfp *cfitfp,int rflg,
                struct CFitdata *cfit,
                int bmnum,int chnum,int cpid,int sflg,int scan) {

  int s=0;

  double atime;
  int flg=1;
  if (rflg) {
    do {
      s=CFitRead(cfitfp,cfit);
      if (s==-1) break;
    } while (s !=0);
    if (s==-1) return -1;
  }
  while (1) {   
    atime=cfit->time;
    if ((bmnum !=-1) && (cfit->bmnum !=bmnum)) flg=0;
    if ((sflg) && (cfit->scan !=scan)) flg=0;
    if (chnum !=-1) {
       if ((chnum==2) && (cfit->channel!=2)) flg=0;
       if ((chnum==1) && (cfit->channel==2)) flg=0;
    }
    if ((cpid !=-1) && (cfit->cp !=cpid)) flg=0;
    if (flg==1) break;
    do {
      s=CFitRead(cfitfp,cfit);
      if (s==-1) break;
    } while (s !=0);
    if (s==-1) return -1;
    flg=1;
  }
  
  return atime;
}

void cfit_tplot(struct CFitdata *cfit,struct tplot *tptr) {
  int i;
  int rng;

  tptr->bmnum=cfit->bmnum;
  tptr->channel=cfit->channel;
  tptr->cpid=cfit->cp;
  tptr->scan=cfit->scan;
  tptr->nrang=cfit->nrang;
  tptr->nave=cfit->nave;
  tptr->intt.sc=cfit->intt.sc;
  tptr->intt.us=cfit->intt.us;
  tptr->frang=cfit->frang;
  tptr->rsep=cfit->rsep;
  tptr->rxrise=cfit->rxrise;
  tptr->noise=cfit->noise;
  tptr->tfreq=cfit->tfreq;
  tptr->atten=cfit->atten;

  tplotset(tptr,cfit->nrang);
  
  for (i=0;i<cfit->num;i++) {
    rng=cfit->rng[i];
    tptr->qflg[rng]=1;
    tptr->gsct[rng]=cfit->data[i].gsct;
    tptr->v[rng]=cfit->data[i].v;
    tptr->p_l[rng]=cfit->data[i].p_l;
    tptr->w_l[rng]=cfit->data[i].w_l;
    tptr->v_e[rng]=cfit->data[i].v;
    tptr->p_l_e[rng]=cfit->data[i].p_l_e;
    tptr->w_l_e[rng]=cfit->data[i].w_l_e;
  }

}
