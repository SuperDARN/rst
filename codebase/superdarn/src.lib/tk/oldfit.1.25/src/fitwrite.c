/* fitwrite.c
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



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitwrite.h"

#include "fitstr.h"



#if defined _QNX4
#define HSTRING "%s version %s (QNX4)\n"
#elif defined _QNX6
#define HSTRING "%s version %s (QNX6)\n"
#elif defined HP_UX
#define HSTRING "%s version %s (UNX)\n"
#elif defined VMS
#define HSTRING "%s version %s (VAX)\n"
#elif defined DOS
#define HSTRING "%s version %s (DOS)\n"
#else
#define HSTRING "%s version %s (UNX)\n"
#endif


int16 OldFitCMP16(int cmp, double val) {
  /* this routine multiplies val by cmp and then converts
     the result to a 16-bit integer.  If the resulting
     value would exceed the bounds of a 16-bit integer
     the returned value is set to the maximum (or minimum)
     integer value */

#define MAXINT (32767)
#define MININT (-32768)

  double vint,vrem;
  double vtemp;
  short int vout;

  vtemp=cmp*val;
  if (vtemp > MAXINT) vout = MAXINT;
  else if (vtemp < MININT) vout = MININT;
  else {
    vrem=modf(vtemp,&vint);
    if (vrem>0.5) vint++;
    if (vrem<-0.5) vint--; 
  
    vout = (int16) vint;
  }
  return vout;
}


int OldFitHeaderWrite(int fitfp, char *text, char *name, char *version) {

  char head_buf[1024];
  int n;

  char hstring[80];
  time_t ctime;

  /* first, clear the header buffer */
  memset(head_buf, 0, FIT_RECL);
  ConvertFromShort(FIT_RECL,(unsigned char *) head_buf);
  ConvertFromShort(INX_RECL,(unsigned char *) (head_buf+sizeof(int16)));
 
  n = sprintf(hstring, HSTRING ,name,version);

  strcpy(head_buf+2*sizeof(int16), hstring);
  ctime = time((time_t) 0);
  strcat(head_buf+2*sizeof(int16), asctime(gmtime(&ctime)));
  strcat(head_buf+2*sizeof(int16),text);
  strcat(head_buf+2*sizeof(int16),"\n");

  if ((fitfp !=-1) && 
     (write(fitfp,head_buf, FIT_RECL) != FIT_RECL)) return -1;
  return 0;
}

int OldFitInxClose(int inxfp,struct RadarParm *prm,int irec) {

  int pat[]={4,4,0,0};
  int32 index_rec[4];
  long ctime;

 ctime = TimeYMDHMSToYrsec(prm->time.yr,
                            prm->time.mo,
                            prm->time.dy,
                            prm->time.hr,
                            prm->time.mt,
                            prm->time.sc);

  if (lseek(inxfp, 0, SEEK_SET) != 0) return -1;

  if (read(inxfp,index_rec, 4*sizeof(int32)) != (4*sizeof(int32))) return -1;
  ConvertBlock( (unsigned char *) index_rec,pat);
  index_rec[1] = ctime;
  index_rec[3] = irec - 1;	/* irec is pointing to the next record */
  ConvertBlock( (unsigned char *) index_rec,pat);
  if (lseek(inxfp, 0, SEEK_SET) !=0) return -1;
  
  if (write(inxfp,index_rec, 4*sizeof(int32)) !=(4*sizeof(int32))) return -1;
  return 0;
}
	
int OldFitInxHeaderWrite(int inxfp,struct RadarParm *prm) {
  int pat[]={4,4,0,0};
  long int ctime;
  int32 index_rec[4];

  ctime = TimeYMDHMSToYrsec(prm->time.yr,
                            prm->time.mo,
                            prm->time.dy,
                            prm->time.hr,
                            prm->time.mt,
                            prm->time.sc);


  index_rec[0]=ctime;
  index_rec[1]=0;
  index_rec[2]=2;
  index_rec[3]=0;
  ConvertBlock( (unsigned char *) index_rec,pat);			 	
  if (write(inxfp,index_rec, sizeof(int32)*4) !=(4*sizeof(int32))) return -1;
			
  return 0;
}

int OldFitInxWrite(int inxfp,int drec,int dnum,struct RadarParm *prm) {
  int pat[]={4,4,0,0};
  long int ctime;
  int32 index_rec[4];
  ctime = TimeYMDHMSToYrsec(prm->time.yr,
                            prm->time.mo,
                            prm->time.dy,
                            prm->time.hr,
                            prm->time.mt,
                            prm->time.sc);

  index_rec[0]=ctime;
  index_rec[1]=drec;
  index_rec[2]=dnum;
  index_rec[3]=(prm->xcf != 0);
  ConvertBlock( (unsigned char *) index_rec,pat);
  if (write(inxfp,index_rec, sizeof(int32)*4) !=(4*sizeof(int32))) return -1;
  return 0;
}

int OldFitWrite(int fitfp,struct RadarParm *prm,struct FitData *fit,
		int *rtab) {
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,ORIG_PULSE_PAT_LEN,2,2*ORIG_LAG_TAB_LEN,1,ORIG_COMBF_SIZE,
                4,3,2,2*ORIG_MAX_RANGE,1,ORIG_MAX_RANGE,0,0};

  int r2_pat[]={4,3,1,25,1,25,2,475,0,0};
  int dnum=0;
  union fit_out r;		
  int slist[ORIG_MAX_RANGE];
  int i,j, k=0, hlength;
  int ctime;
  long int rrn;
  struct fitdata oldfit;
  struct fitdata *fptr;
  int c,rv;

  memset(&oldfit.prms,0,sizeof(struct radar_parms));

  oldfit.prms.REV.MAJOR=prm->revision.major;
  oldfit.prms.REV.MINOR=prm->revision.minor;
  oldfit.prms.NPARM=48;
  oldfit.prms.CP=prm->cp;
  oldfit.prms.ST_ID=prm->stid;
  oldfit.prms.YEAR=prm->time.yr;
  oldfit.prms.MONTH=prm->time.mo;
  oldfit.prms.DAY=prm->time.dy;
  oldfit.prms.HOUR=prm->time.hr;
  oldfit.prms.MINUT=prm->time.mt;
  oldfit.prms.SEC=prm->time.sc;
   
  oldfit.prms.TXPOW=prm->txpow;
  oldfit.prms.NAVE=prm->nave;
  oldfit.prms.ATTEN=prm->atten;
  oldfit.prms.LAGFR=prm->lagfr;
  oldfit.prms.SMSEP=prm->smsep;
  oldfit.prms.ERCOD=prm->ercod;
  oldfit.prms.AGC_STAT=prm->stat.agc;
  oldfit.prms.LOPWR_STAT=prm->stat.lopwr;
  oldfit.prms.NOISE=prm->noise.search;
  oldfit.prms.NOISE_MEAN=prm->noise.mean;
      
  oldfit.prms.CHANNEL=prm->channel;
  oldfit.prms.BMNUM=prm->bmnum;
  oldfit.prms.SCAN=prm->scan;
 
  oldfit.prms.RXRISE=prm->rxrise;
  oldfit.prms.INTT=prm->intt.sc;
  oldfit.prms.TXPL=prm->txpl;
    
  oldfit.prms.MPINC=prm->mpinc;
  oldfit.prms.MPPUL=prm->mppul;
  oldfit.prms.MPLGS=prm->mplgs;
  oldfit.prms.NRANG=prm->nrang;
  oldfit.prms.FRANG=prm->frang;
  oldfit.prms.RSEP=prm->rsep;
  oldfit.prms.XCF=prm->xcf; 
  oldfit.prms.TFREQ=prm->tfreq;
  oldfit.prms.MXPWR=prm->mxpwr;
  oldfit.prms.LVMAX=prm->lvmax;   

  if (oldfit.prms.NRANG>ORIG_MAX_RANGE) oldfit.prms.NRANG=ORIG_MAX_RANGE;
    
  for (c=0;c<prm->mppul;c++) oldfit.pulse[c]=prm->pulse[c];
  for (c=0;c<prm->mplgs;c++) {
    oldfit.lag[0][c]=prm->lag[0][c];
    oldfit.lag[1][c]=prm->lag[1][c];
  } 
  strcpy(oldfit.combf,prm->combf);

  oldfit.noise.vel=fit->noise.vel;
  oldfit.noise.lag0=fit->noise.lag0;
  oldfit.noise.skynoise=fit->noise.skynoise;

  for (c=0;c<ORIG_MAX_RANGE;c++) { 
    oldfit.rng[c].qflg=0;
    oldfit.xrng[c].qflg=0;
  }

  for (c=0;c<oldfit.prms.NRANG;c++) {
    if (rtab !=NULL) rv=rtab[c];
    else rv=c;
    oldfit.rng[c].v=fit->rng[rv].v;
    oldfit.rng[c].v_err=fit->rng[rv].v_err;
    oldfit.rng[c].p_0=fit->rng[rv].p_0;
    oldfit.rng[c].p_l=fit->rng[rv].p_l;
    oldfit.rng[c].p_l_err=fit->rng[rv].p_l_err;
    oldfit.rng[c].p_s=fit->rng[rv].p_s;
    oldfit.rng[c].p_s_err=fit->rng[rv].p_s_err;
    oldfit.rng[c].w_l=fit->rng[rv].w_l;
    oldfit.rng[c].w_l_err=fit->rng[rv].w_l_err;
    oldfit.rng[c].w_s=fit->rng[rv].w_s;
    oldfit.rng[c].w_s_err=fit->rng[rv].w_s_err;
    oldfit.rng[c].phi0=fit->rng[rv].phi0;
    oldfit.rng[c].phi0_err=fit->rng[rv].phi0_err;
    oldfit.rng[c].sdev_l=fit->rng[rv].sdev_l;
    oldfit.rng[c].sdev_s=fit->rng[rv].sdev_s;
    oldfit.rng[c].sdev_phi=fit->rng[rv].sdev_phi;
    oldfit.rng[c].qflg=fit->rng[rv].qflg;
    oldfit.rng[c].gsct=fit->rng[rv].gsct;
    oldfit.rng[c].nump=fit->rng[rv].nump; 
  }

  if (prm->xcf !=0) for (c=0;c<oldfit.prms.NRANG;c++) {
    if (rtab !=NULL) rv=rtab[c];
    else rv=c;
    oldfit.xrng[c].v=fit->xrng[rv].v;
    oldfit.xrng[c].v_err=fit->xrng[rv].v_err;
    oldfit.xrng[c].p_0=fit->xrng[rv].p_0;
    oldfit.xrng[c].p_l=fit->xrng[rv].p_l;
    oldfit.xrng[c].p_l_err=fit->xrng[rv].p_l_err;
    oldfit.xrng[c].p_s=fit->xrng[rv].p_s;
    oldfit.xrng[c].p_s_err=fit->xrng[rv].p_s_err;
    oldfit.xrng[c].w_l=fit->xrng[rv].w_l;
    oldfit.xrng[c].w_l_err=fit->xrng[rv].w_l_err;
    oldfit.xrng[c].w_s=fit->xrng[rv].w_s;
    oldfit.xrng[c].w_s_err=fit->xrng[rv].w_s_err;
    oldfit.xrng[c].phi0=fit->xrng[rv].phi0;
    oldfit.xrng[c].phi0_err=fit->xrng[rv].phi0_err;
    oldfit.xrng[c].sdev_l=fit->xrng[rv].sdev_l;
    oldfit.xrng[c].sdev_s=fit->xrng[rv].sdev_s;
    oldfit.xrng[c].sdev_phi=fit->xrng[rv].sdev_phi;
    oldfit.xrng[c].qflg=fit->xrng[rv].qflg;
    oldfit.xrng[c].gsct=fit->xrng[rv].gsct;
    oldfit.xrng[c].nump=fit->xrng[rv].nump; 
    oldfit.elev[c].low=fit->elv[rv].low;
    oldfit.elev[c].normal=fit->elv[rv].normal;
    oldfit.elev[c].high=fit->elv[rv].high;
  }

 
  fptr=&oldfit;
  memset(&r,0,sizeof(union fit_out));


  ctime = TimeYMDHMSToYrsec(fptr->prms.YEAR,
				   fptr->prms.MONTH,
				   fptr->prms.DAY,
				   fptr->prms.HOUR,
				   fptr->prms.MINUT,
				   fptr->prms.SEC);
   
  /* transfer the parameter list, the pulse and lag tables, the lag-0 pwrs
	and the selection list to the first record (rel_rec_no 0) */
  r.r1.rrn = 0;
  hlength = sizeof(struct radar_parms) + 
            sizeof(int16)*(ORIG_PULSE_PAT_LEN + 
            2*ORIG_LAG_TAB_LEN) + ORIG_COMBF_SIZE;
  r.r1.r_time = ctime;
  memcpy(&(r.r1.plist[0]),fptr, hlength);

  /*  Now transfer the noise parameters */

  r.r1.r_noise_lev = (long) fptr->noise.skynoise;
  r.r1.r_noise_lag0 = fptr->noise.lag0;
  r.r1.r_noise_vel = OldFitCMP16(10,fptr->noise.vel);

  /* Now transfer the lag0 powers and the selection list */

  memset(slist,0,sizeof(int)*ORIG_MAX_RANGE);

  for (i=0; i<fptr->prms.NRANG; i++) {
    if ((fptr->rng[i].qflg ==1) ||
       (fptr->xrng[i].qflg ==1)) {
      slist[k]=i+1;
      k++;
    }
  }

  for (i=0; i<fptr->prms.NRANG; i++) {
    
    r.r1.r_pwr0[i] = OldFitCMP16(100,fptr->rng[i].p_0);
    r.r1.r_slist[i] = slist[i];
    if (slist[i] > 0) r.r1.r_numlags[i]=fptr->rng[slist[i]-1].nump; 
  }

  ConvertBlock( (unsigned char *) &r,r1_pat);
  if ((fitfp !=-1) && 
     (write(fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out))) 
     return -1;

  dnum++;

 
  /* Now start transferring the main data block */

  rrn=1;
  j = 0;

  /* first check and make sure that there is something to transfer.
	if not, return */
  if (slist[0]==0) return dnum;
  
  for (i=0; i<ORIG_MAX_RANGE; i++) {
        r.r2.r_time = ctime;
        r.r2.r_xflag = 0;
        r.r2.rrn=rrn;
        r.r2.range[j] = slist[i];
        k=slist[i]-1;
	r.r2.r_qflag[j] = fptr->rng[k].qflg;
	r.r2.r_pwr_l[j] = OldFitCMP16(100,fptr->rng[k].p_l);
	r.r2.r_pwr_l_err[j] = OldFitCMP16(100,fptr->rng[k].p_l_err);
        r.r2.r_pwr_s[j] = OldFitCMP16(100,fptr->rng[k].p_s);
	r.r2.r_pwr_s_err[j] = OldFitCMP16(100,fptr->rng[k].p_s_err);
	r.r2.r_vel[j] = OldFitCMP16(10,fptr->rng[k].v);
	r.r2.r_vel_err[j] = OldFitCMP16(10,fptr->rng[k].v_err);
	r.r2.r_w_l[j] = OldFitCMP16(10,fptr->rng[k].w_l);
	r.r2.r_w_l_err[j] = OldFitCMP16(10,fptr->rng[k].w_l_err);
	r.r2.r_w_s[j] = OldFitCMP16(10,fptr->rng[k].w_s);
	r.r2.r_w_s_err[j] = OldFitCMP16(10,fptr->rng[k].w_s_err);
	r.r2.r_sdev_l[j] = OldFitCMP16(1000,fptr->rng[k].sdev_l);
	r.r2.r_sdev_s[j] = OldFitCMP16(1000,fptr->rng[k].sdev_s);
	r.r2.r_sdev_phi[j] = OldFitCMP16(100,fptr->rng[k].sdev_phi);
	r.r2.r_gscat[j] = fptr->rng[k].gsct;
	j++;
	if (j == 25) {
          ConvertBlock( (unsigned char *) &r,r2_pat);
          if ((fitfp !=-1) && 
          (write(fitfp,&r,sizeof(union fit_out)) 
           !=sizeof(union fit_out))) return -1;
          dnum++;			
	  rrn++;
	  j=0;
	  if ( i >= (ORIG_MAX_RANGE-1)) break;
	  if (slist[i+1] == 0) break;
	}	
  }
  
  /*  Now write out the xcf data if there is any */
 
  j = 0;
  for (i=0; (i<ORIG_MAX_RANGE) && (fptr->prms.XCF != 0); i++) {
        r.r2.r_time = ctime;
        r.r2.r_xflag = 1;
        r.r2.rrn=rrn;
	r.r2.range[j] = slist[i];
        k=slist[i]-1;
	r.r2.r_qflag[j] = fptr->xrng[k].qflg;
	r.r2.r_pwr_l[j] = OldFitCMP16(100,fptr->xrng[k].p_l);
	r.r2.r_pwr_l_err[j] = OldFitCMP16(100,fptr->xrng[k].p_l_err);
        r.r2.r_pwr_s[j] = OldFitCMP16(100,fptr->xrng[k].p_s);
	r.r2.r_pwr_s_err[j] = OldFitCMP16(100,fptr->xrng[k].p_s_err);
	r.r2.r_vel[j] = OldFitCMP16(10,fptr->xrng[k].v);
	r.r2.r_vel_err[j] = OldFitCMP16(10,fptr->xrng[k].v_err);
	r.r2.r_w_l[j] = OldFitCMP16(10,fptr->xrng[k].w_l);
	r.r2.r_w_l_err[j] = OldFitCMP16(10,fptr->xrng[k].w_l_err);
	r.r2.r_w_s[j] = OldFitCMP16(10,fptr->xrng[k].w_s);
	r.r2.r_w_s_err[j] = OldFitCMP16(10,fptr->xrng[k].w_s_err);

	r.r2.r_phi0[j] = OldFitCMP16(100,fptr->xrng[k].phi0);
	r.r2.r_phi0_err[j] = OldFitCMP16(100,fptr->xrng[k].phi0_err);
	r.r2.r_elev[j] = OldFitCMP16(100,fptr->elev[k].normal);
	r.r2.r_elev_low[j] = OldFitCMP16(100,fptr->elev[k].low);
	r.r2.r_elev_high[j] = OldFitCMP16(100,fptr->elev[k].high);

	r.r2.r_sdev_l[j] = OldFitCMP16(1000,fptr->xrng[k].sdev_l);
	r.r2.r_sdev_s[j] = OldFitCMP16(1000,fptr->xrng[k].sdev_s);
	r.r2.r_sdev_phi[j] = OldFitCMP16(100,fptr->xrng[k].sdev_phi);
	r.r2.r_gscat[j] = fptr->rng[k].gsct;
	j++;
	if (j == 25) {
          ConvertBlock( (unsigned char *) &r,r2_pat);
          if ((fitfp !=-1) && 
          (write(fitfp,&r,sizeof(union fit_out)) 
           !=sizeof(union fit_out))) return -1;
          dnum++;
          rrn++;
	  j=0;
	  if ( i >= ORIG_MAX_RANGE) break;
	  if (slist[i+1] == 0) break;
    }
  }

  return dnum; 
}




int OldFitFwrite(FILE *fitfp,struct RadarParm *prm,struct FitData *fit,
                 int *rtab) {
  if (fitfp==NULL) return OldFitWrite(-1,prm,fit,rtab);
  return OldFitWrite(fileno(fitfp),prm,fit,rtab);
}
   
int OldFitHeaderFwrite(FILE *fitfp, char *text, char *name, char *version) {
  if (fitfp==NULL) return OldFitHeaderWrite(-1,text,name,version);
  return OldFitHeaderWrite(fileno(fitfp),text,name,version);
}

int OldFitInxHeaderFwrite(FILE *inxfp,struct RadarParm *prm) {
  if (inxfp==NULL) return -1;
  return OldFitInxHeaderWrite(fileno(inxfp),prm);
}


int OldFitInxFclose(FILE *inxfp,struct RadarParm *prm,int irec) {
  if (inxfp==NULL) return -1;
  return OldFitInxClose(fileno(inxfp),prm,irec);
}


int OldFitInxFwrite(FILE *inxfp,int drec,int dnum,struct RadarParm *prm) {
  if (inxfp==NULL) return -1;
  return OldFitInxWrite(fileno(inxfp),drec,dnum,prm);
}



