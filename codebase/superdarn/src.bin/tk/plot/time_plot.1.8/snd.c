/* snd.c
   =====
   Author: E.G.Thomas
Copyright (C) <year>  <name of author>

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
#include "rtime.h"
#include "rtypes.h"
#include "dmap.h"
#include "snddata.h"
#include "sndread.h"
#include "sndseek.h"
#include "tplot.h"


double snd_find(FILE *fp,struct SndData *snd,
                double sdate,double stime) {
  int status;
  int yr,mo,dy,hr,mt;
  double sc;
  double atime;
  if (SndFread(fp,snd)==-1) return -1;
  atime=TimeYMDHMSToEpoch(snd->time.yr,snd->time.mo,snd->time.dy,
                          snd->time.hr,snd->time.mt,
                          snd->time.sc+snd->time.us/1.0e6);

  if ((stime==-1) && (sdate==-1)) return atime;
  if (stime==-1) stime= ( (int) atime % (24*3600));
  if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
  else stime+=sdate;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  if (atime>=stime) return stime;

  status=SndFseek(fp,yr,mo,dy,hr,mt,sc,NULL);
  if (status==-1) return -1;
  if (SndFread(fp,snd)==-1) return -1;

  return stime;
}


double snd_scan(double ctime,FILE *fp,int rflg,struct SndData *snd,
                int bmnum,int cpid,int sflg,int scan) {

  double atime;
  int flg=1;

  if (rflg) {
    if (SndFread(fp,snd)==-1) return -1;
  }
  while (1) {

    atime=TimeYMDHMSToEpoch(snd->time.yr,snd->time.mo,snd->time.dy,
                            snd->time.hr,snd->time.mt,
                            snd->time.sc+snd->time.us/1.0e6);

    if (atime<ctime) flg=0;
    if ((bmnum !=-1) && (snd->bmnum !=bmnum)) flg=0;
    if ((sflg) && (snd->scan !=scan)) flg=0;
    if ((cpid !=-1) && (snd->cp !=cpid)) flg=0;
    if (flg==1) break;
    if (SndFread(fp,snd)==-1) return -1;
    flg=1;
  }

  return atime;
}


void snd_tplot(struct SndData *snd,struct tplot *tptr) {
  int i;

  tptr->bmnum=snd->bmnum;
  tptr->channel=snd->channel;
  tptr->cpid=snd->cp;
  tptr->scan=snd->scan;
  tptr->nrang=snd->nrang;
  tptr->nave=snd->nave;
  tptr->intt.sc=snd->intt.sc;
  tptr->intt.us=snd->intt.us;
  tptr->frang=snd->frang;
  tptr->rsep=snd->rsep;
  tptr->rxrise=snd->rxrise;
  tptr->noise=snd->noise.search;
  tptr->tfreq=snd->tfreq;
  tptr->atten=0;

  tplotset(tptr,snd->nrang);
  
  for (i=0;i<snd->nrang;i++) {
    tptr->qflg[i]=snd->rng[i].qflg;
    tptr->gsct[i]=snd->rng[i].gsct;
    tptr->v[i]=snd->rng[i].v;
    tptr->p_l[i]=snd->rng[i].p_l;
    tptr->w_l[i]=snd->rng[i].w_l;
    tptr->v_e[i]=snd->rng[i].v;
    if (snd->rng[i].x_qflg == 1) tptr->phi0[i]=snd->rng[i].phi0;
    else tptr->phi0[i]=-4;
  }

}

