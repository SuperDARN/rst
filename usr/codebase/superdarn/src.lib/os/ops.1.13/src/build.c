/* build.c
   =======
   Author: R.J.Barnes, J.Spaleta
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
#include <time.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "limit.h"
#include "dmap.h"
#include "rprm.h"
#include "iq.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "global.h"
#include "rosversion.h"


void OpsBuildPrm(struct RadarParm *prm,int *ptab,int (*lags)[2]) {
  int i,j;
  time_t ctime;
  float offset;
  
  int16 *lagtab=NULL;
  int16 *pulsetab=NULL;
  
  char tmstr[80];

  prm->revision.major = MAJOR_VERSION;
  prm->revision.minor = MINOR_VERSION;

  prm->origin.code=0;
  
  ctime = time((time_t) 0);
  RadarParmSetOriginCommand(prm,(char *) command);

  strcpy(tmstr,asctime(gmtime(&ctime)));
  tmstr[24]=0;
  RadarParmSetOriginTime(prm,tmstr);

  prm->stid = stid;
  prm->channel=0;
  prm->time.yr=yr;
  prm->time.mo=mo;
  prm->time.dy=dy;
  prm->time.hr=hr;
  prm->time.mt=mt;
  prm->time.sc=sc;
  prm->time.us=us;
  prm->txpow = txpow;
  prm->nave=nave;
  prm->atten = atten;
  prm->lagfr = lagfr;
  prm->smsep = smsep;
  prm->noise.search=noise;
  prm->noise.mean=noisestat[0];
  prm->rxrise= rxrise;
  prm->intt.sc  = intsc;
  prm->intt.us  = intus;
  prm->txpl  = txpl;
  prm->mpinc = mpinc;
  prm->mppul = mppul;
  prm->mplgs = mplgs;
  prm->nrang = nrang;
  prm->frang = frang;
  prm->rsep = rsep;
  prm->bmnum = bmnum;

  offset=site->maxbeam/2.0-0.5;
  prm->bmazm=site->boresite+site->bmsep*(bmnum-offset);
  prm->ifmode = ifmode;


  prm->xcf   = xcf;
  prm->tfreq = tfreq;
  prm->scan  = scan;
  prm->mxpwr = mxpwr;
  prm->lvmax = lvmax;
  prm->cp	= cp;

  prm->stat.agc=agcstat;
  prm->stat.lopwr=lopwrstat;

  pulsetab=malloc(sizeof(int16)*mppul);
  for (i=0;i<mppul;i++) pulsetab[i]=ptab[i];
  RadarParmSetPulse(prm,mppul,pulsetab);

  if (mplgexs !=0) {
     lagtab=malloc(sizeof(int16)*2*(mplgexs+1));
    for (i=0;i<2;i++) {
      for (j=0;j<=mplgexs;j++) lagtab[2*j+i]=lags[j][i];
    } 
    free(lagtab);
    RadarParmSetLag(prm,mplgexs,lagtab); 
  } else {
    lagtab=malloc(sizeof(int16)*2*(mplgs+1));
    for (i=0;i<2;i++) {
      for (j=0;j<=mplgs;j++) lagtab[2*j+i]=lags[j][i];
    } 
    free(lagtab);
    RadarParmSetLag(prm,mplgs,lagtab);
  }
  RadarParmSetCombf(prm,combf);
}

void OpsBuildIQ(struct IQ *iq,unsigned int **badtr) {

  int i,badtrnum=0,badtroff=0;
  char *badtradr=NULL;
  void *tmp=NULL;

  iq->seqnum=nave;
  iq->chnnum=rxchn;
  iq->smpnum=smpnum;  
  iq->skpnum=skpnum;

  IQSetTime(iq,nave,seqtval);
  IQSetAtten(iq,nave,seqatten);
  IQSetNoise(iq,nave,seqnoise);
  IQSetOffset(iq,nave,seqoff);
  IQSetSize(iq,nave,seqsze);

  if (iq->badtr==NULL) tmp=malloc(sizeof(int)*nave);
  else tmp=realloc(iq->badtr,sizeof(int)*nave);

  if (tmp==NULL) return;

  iq->badtr=tmp;
  
  for (i=0;i<nave;i++) {
    iq->badtr[i]=seqbadtr[i].num;
    badtrnum+=seqbadtr[i].num;
  }
  iq->tbadtr=badtrnum;
  if (*badtr !=NULL) free(*badtr);
  badtradr=malloc(sizeof(unsigned int)*2*badtrnum);
  *badtr=(unsigned int *) badtradr;

  for (i=0;i<nave;i++) { 
    memcpy(badtradr+badtroff,seqbadtr[i].start,
           sizeof(unsigned int)*seqbadtr[i].num); 
    badtroff+= sizeof(unsigned int)*seqbadtr[i].num; 
    memcpy(badtradr+badtroff,seqbadtr[i].length,
           sizeof(unsigned int)*seqbadtr[i].num); 
    badtroff+= sizeof(unsigned int)*seqbadtr[i].num; 
  }
}




void OpsBuildRaw(struct RawData *raw) {  
  RawSetPwr(raw,nrang,pwr0,0,NULL);
  RawSetACF(raw,nrang,mplgs,acfd,0,NULL);
  RawSetXCF(raw,nrang,mplgs,xcfd,0,NULL);
}

