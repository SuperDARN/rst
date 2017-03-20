/* decodemsg.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "cnxstr.h"



#define read_bit(t,n) (((t)[n/8] & (1<<(n%8))) !=0)
#define ORIG_MAX_RANGE 75

int OldFitCnxDecodeIPMsg(struct RadarParm *prm,struct FitData *fit,
                         unsigned char *buffer) {
  unsigned char *buf;
  unsigned char *gsct;
  unsigned char *dflg;
  struct radar_parms oldprm;

  int pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
  
  int i;
  int rng_num=0;
  int index;

  if (buffer[0] !='d') return 0;
  buf=buffer+1;

  ConvertBlock(buf,pat);
  memcpy(&oldprm,buf,sizeof(struct radar_parms));
  buf+=sizeof(struct radar_parms);

  gsct=buf;
  dflg=buf+10;
  buf+=20;

  prm->revision.major=oldprm.REV.MAJOR;
  prm->revision.minor=oldprm.REV.MINOR;
  prm->cp=oldprm.CP;
  prm->stid=oldprm.ST_ID;
  prm->time.yr=oldprm.YEAR;
  prm->time.mo=oldprm.MONTH;
  prm->time.dy=oldprm.DAY;
  prm->time.hr=oldprm.HOUR;
  prm->time.mt=oldprm.MINUT;
  prm->time.sc=oldprm.SEC;
  prm->time.us=0;

  prm->txpow=oldprm.TXPOW;
  prm->nave=oldprm.NAVE;
  prm->atten=oldprm.ATTEN;
  prm->lagfr=oldprm.LAGFR;
  prm->smsep=oldprm.SMSEP;
  prm->ercod=oldprm.ERCOD;
  prm->stat.agc=oldprm.AGC_STAT;
  prm->stat.lopwr=oldprm.LOPWR_STAT;
  prm->noise.search=oldprm.NOISE;
  prm->noise.mean=oldprm.NOISE_MEAN;
      
  prm->channel=oldprm.CHANNEL;
  prm->bmnum=oldprm.BMNUM;
  prm->scan=oldprm.SCAN;
  
  prm->offset=oldprm.usr_resL1; 
  prm->rxrise=oldprm.RXRISE;
  prm->intt.sc=oldprm.INTT;
  prm->intt.us=0;
  prm->txpl=oldprm.TXPL;
    
  prm->mpinc=oldprm.MPINC;
  prm->mppul=oldprm.MPPUL;
  prm->mplgs=oldprm.MPLGS;
  prm->nrang=oldprm.NRANG;
  prm->frang=oldprm.FRANG;
  prm->rsep=oldprm.RSEP;
  prm->xcf=oldprm.XCF; 
  prm->tfreq=oldprm.TFREQ;
  prm->mxpwr=oldprm.MXPWR;
  prm->lvmax=oldprm.LVMAX;   

  FitSetRng(fit,ORIG_MAX_RANGE);

  for (i=0;i<ORIG_MAX_RANGE;i++) {
    fit->rng[i].qflg=0;
    fit->rng[i].gsct=0;
    fit->rng[i].v=0;
    fit->rng[i].v_err=0;
    fit->rng[i].p_0=0;
    fit->rng[i].p_l=0;
    fit->rng[i].p_l_err=0;
    fit->rng[i].p_s=0;
    fit->rng[i].p_s_err=0;
    fit->rng[i].w_l=0;
    fit->rng[i].w_l_err=0;
    fit->rng[i].w_s=0;
    fit->rng[i].w_s_err=0;
    fit->rng[i].phi0=0;
    fit->rng[i].phi0_err=0;
    fit->rng[i].sdev_l=0;
    fit->rng[i].sdev_s=0;
    fit->rng[i].sdev_phi=0;
    fit->rng[i].nump=0;
  }
  for (i=0;i<ORIG_MAX_RANGE;i++) {
    if (read_bit(dflg,i) !=0) {
      fit->rng[i].qflg=1;
      rng_num++;
    }
  }

  if (rng_num == 0) return 1;
  index=0;
  for (i=0;i<ORIG_MAX_RANGE;i++) {
    if (fit->rng[i].qflg !=0) {
      if (read_bit(gsct,i) !=0) fit->rng[i].gsct=1;
      ConvertToDouble(buf+8*index,&fit->rng[i].p_l);
      ConvertToDouble(buf+8*(index+rng_num),&fit->rng[i].v);
      ConvertToDouble(buf+8*(index+2*rng_num),&fit->rng[i].w_l);
      index++;
    }
  } 
  return 1;
}







