/* cmpfit.c
   ======== 
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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rtypes.h"
#include "option.h"

#include "hlpstr.h"
#include "errstr.h"

#include "lmt.h"
#include "raw.h"
#include "fit.h"
#include "fit_read.h"



struct fitdata fitA;
struct fitdata fitB;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: cmpfit --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;


  int arg=0;

  struct fitfp *fitfpA=NULL;
  struct fitfp *fitfpB=NULL;
  
  int sA,sB;
  int c;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
   
  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  if (arg==argc) { 
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

 

  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  fitfpA=fit_open(argv[1],NULL);
  fitfpB=fit_open(argv[2],NULL);

  while (1) {
    sA=fit_read(fitfpA,&fitA);
    sB=fit_read(fitfpB,&fitB);
    if ((sA==-1) || (sB==-1)) break;
  
    fprintf(stdout,
    "%.4d-%.2d-%.2d %.2d:%.2d:%.2d\t%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
	    fitA.prms.YEAR,fitA.prms.MONTH,fitA.prms.DAY,
            fitA.prms.HOUR,fitA.prms.MINUT,fitA.prms.SEC,
            fitB.prms.YEAR,fitB.prms.MONTH,fitB.prms.DAY,
            fitB.prms.HOUR,fitB.prms.MINUT,fitB.prms.SEC);




    if (fitA.prms.REV.MAJOR !=fitB.prms.REV.MAJOR) fprintf(stdout,"fit.prms.REV.MAJOR: %d !=%d \n",fitA.prms.REV.MAJOR,fitB.prms.REV.MAJOR);


    if (fitA.prms.REV.MINOR !=fitB.prms.REV.MINOR) fprintf(stdout,"fit.prms.REV.MINOR: %d !=%d \n",fitA.prms.REV.MINOR,fitB.prms.REV.MINOR);

    if (fitA.prms.NPARM !=fitB.prms.NPARM) fprintf(stdout,"fit.prms.NPARM: %d !=%d \n",fitA.prms.NPARM,fitB.prms.NPARM);

   if (fitA.prms.ST_ID !=fitB.prms.ST_ID) fprintf(stdout,"fit.prms.ST_ID: %d !=%d \n",fitA.prms.ST_ID,fitB.prms.ST_ID);

   if (fitA.prms.YEAR !=fitB.prms.YEAR) fprintf(stdout,"fit.prms.YEAR: %d !=%d \n",fitA.prms.YEAR,fitB.prms.YEAR);

   if (fitA.prms.MONTH !=fitB.prms.MONTH) fprintf(stdout,"fit.prms.MONTH: %d !=%d \n",fitA.prms.MONTH,fitB.prms.MONTH);

   if (fitA.prms.DAY !=fitB.prms.DAY) fprintf(stdout,"fit.prms.DAY: %d !=%d \n",fitA.prms.DAY,fitB.prms.DAY);

   if (fitA.prms.HOUR !=fitB.prms.HOUR) fprintf(stdout,"fit.prms.HOUR: %d !=%d \n",fitA.prms.HOUR,fitB.prms.HOUR);

   if (fitA.prms.MINUT !=fitB.prms.MINUT) fprintf(stdout,"fit.prms.MINUT: %d !=%d \n",fitA.prms.MINUT,fitB.prms.MINUT);

   if (fitA.prms.SEC !=fitB.prms.SEC) fprintf(stdout,"fit.prms.SEC: %d !=%d \n",fitA.prms.SEC,fitB.prms.SEC);

   if (fitA.prms.TXPOW !=fitB.prms.TXPOW) fprintf(stdout,"fit.prms.TXPOW: %d !=%d \n",fitA.prms.TXPOW,fitB.prms.TXPOW);

   if (fitA.prms.NAVE !=fitB.prms.NAVE) fprintf(stdout,"fit.prms.NAVE: %d !=%d \n",fitA.prms.NAVE,fitB.prms.NAVE);

   if (fitA.prms.ATTEN !=fitB.prms.ATTEN) fprintf(stdout,"fit.prms.ATTEN: %d !=%d \n",fitA.prms.ATTEN,fitB.prms.ATTEN);

   if (fitA.prms.LAGFR !=fitB.prms.LAGFR) fprintf(stdout,"fit.prms.LAGFR: %d !=%d \n",fitA.prms.LAGFR,fitB.prms.LAGFR); 

   if (fitA.prms.SMSEP !=fitB.prms.SMSEP) fprintf(stdout,"fit.prms.SMSEP: %d !=%d \n",fitA.prms.SMSEP,fitB.prms.SMSEP);


   if (fitA.prms.ERCOD !=fitB.prms.ERCOD) fprintf(stdout,"fit.prms.ERCOD: %d !=%d \n",fitA.prms.ERCOD,fitB.prms.ERCOD);

   if (fitA.prms.AGC_STAT !=fitB.prms.AGC_STAT) fprintf(stdout,"fit.prms.AGC_STAT: %d !=%d \n",fitA.prms.AGC_STAT,fitB.prms.AGC_STAT);

   if (fitA.prms.LOPWR_STAT !=fitB.prms.LOPWR_STAT) fprintf(stdout,"fit.prms.LOPWR_STAT: %d !=%d \n",fitA.prms.LOPWR_STAT,fitB.prms.LOPWR_STAT);

   if (fitA.prms.NBAUD !=fitB.prms.NBAUD) fprintf(stdout,"fit.prms.NBAUD: %d !=%d \n",fitA.prms.NBAUD,fitB.prms.NBAUD);

   if (fitA.prms.NOISE !=fitB.prms.NOISE) fprintf(stdout,"fit.prms.NOISE: %d !=%d \n",fitA.prms.NOISE,fitB.prms.NOISE);

   if (fitA.prms.NOISE_MEAN !=fitB.prms.NOISE_MEAN) fprintf(stdout,"fit.prms.NOISE_MEAN: %d !=%d \n",fitA.prms.NOISE_MEAN,fitB.prms.NOISE_MEAN);

   if (fitA.prms.CHANNEL !=fitB.prms.CHANNEL) fprintf(stdout,"fit.prms.CHANNEL: %d !=%d \n",fitA.prms.CHANNEL,fitB.prms.CHANNEL);


   if (fitA.prms.RXRISE !=fitB.prms.RXRISE) fprintf(stdout,"fit.prms.RXRISE: %d !=%d \n",fitA.prms.RXRISE,fitB.prms.RXRISE);


   if (fitA.prms.INTT !=fitB.prms.INTT) fprintf(stdout,"fit.prms.INTT: %d !=%d \n",fitA.prms.INTT,fitB.prms.INTT);


   if (fitA.prms.TXPL !=fitB.prms.TXPL) fprintf(stdout,"fit.prms.TXPL: %d !=%d \n",fitA.prms.TXPL,fitB.prms.TXPL);


   if (fitA.prms.MPINC !=fitB.prms.MPINC) fprintf(stdout,"fit.prms.MPINC: %d !=%d \n",fitA.prms.MPINC,fitB.prms.MPINC);




   if (fitA.prms.MPPUL !=fitB.prms.MPPUL) fprintf(stdout,"fit.prms.MPPUL: %d !=%d \n",fitA.prms.MPPUL,fitB.prms.MPPUL);


   if (fitA.prms.MPLGS !=fitB.prms.MPLGS) fprintf(stdout,"fit.prms.MPLGS: %d !=%d \n",fitA.prms.MPLGS,fitB.prms.MPLGS);


   if (fitA.prms.NRANG !=fitB.prms.NRANG) fprintf(stdout,"fit.prms.NRANG: %d !=%d \n",fitA.prms.NRANG,fitB.prms.NRANG);


   if (fitA.prms.FRANG !=fitB.prms.FRANG) fprintf(stdout,"fit.prms.FRANG: %d !=%d \n",fitA.prms.FRANG,fitB.prms.FRANG);


   if (fitA.prms.RSEP !=fitB.prms.RSEP) fprintf(stdout,"fit.prms.RSEP: %d !=%d \n",fitA.prms.RSEP,fitB.prms.RSEP);

   if (fitA.prms.BMNUM !=fitB.prms.BMNUM) fprintf(stdout,"fit.prms.BMNUM: %d !=%d \n",fitA.prms.BMNUM,fitB.prms.BMNUM);


   if (fitA.prms.XCF !=fitB.prms.XCF) fprintf(stdout,"fit.prms.XCF: %d !=%d \n",fitA.prms.XCF,fitB.prms.XCF);

 
   if (fitA.prms.TFREQ !=fitB.prms.TFREQ) fprintf(stdout,"fit.prms.TFREQ: %d !=%d \n",fitA.prms.TFREQ,fitB.prms.TFREQ);

   if (fitA.prms.SCAN !=fitB.prms.SCAN) fprintf(stdout,"fit.prms.SCAN: %d !=%d \n",fitA.prms.SCAN,fitB.prms.SCAN);

   if (fitA.prms.MXPWR !=fitB.prms.MXPWR) fprintf(stdout,"fit.prms.MXPWR: %d !=%d \n",fitA.prms.MXPWR,fitB.prms.MXPWR);


   if (fitA.prms.LVMAX !=fitB.prms.LVMAX) fprintf(stdout,"fit.prms.LVMAX: %d !=%d \n",fitA.prms.LVMAX,fitB.prms.LVMAX);


   if (fitA.prms.CP !=fitB.prms.CP) fprintf(stdout,"fit.prms.CP: %d !=%d \n",fitA.prms.CP,fitB.prms.CP);

   if (strcmp(fitA.combf,fitB.combf) !=0)  fprintf(stdout,"fit.combf: %s !=%s \n",fitA.combf,fitB.combf);

   for (c=0;c<fitA.prms.MPPUL;c++) if (fitA.pulse[c] !=fitB.pulse[c]) break;
   if (c !=fitA.prms.MPPUL) fprintf(stdout,"Pulse pattern does not match\n");

   for (c=0;c<fitA.prms.MPLGS;c++) {
     if (fitA.lag[0][c] !=fitB.lag[0][c]) break;
     if (fitA.lag[1][c] !=fitB.lag[1][c]) break;

   }
   if (c !=fitA.prms.MPLGS) fprintf(stdout,"Lag table does not match\n");

   if (fitA.noise.vel !=fitB.noise.vel) fprintf(stdout,"fit.noise.vel: %g !=%g\n",fitA.noise.vel,fitB.noise.vel);

   if (fitA.noise.lag0 !=fitB.noise.lag0) fprintf(stdout,"fit.noise.lag0: %g !=%g\n",fitA.noise.lag0,fitB.noise.lag0);

   if (fitA.noise.skynoise !=fitB.noise.skynoise) fprintf(stdout,"fit.noise.skynoise: %g !=%g\n",fitA.noise.skynoise,fitB.noise.skynoise);

 
   for (c=0;c<fitA.prms.NRANG;c++) {
     if (fitA.rng[c].v !=fitB.rng[c].v) {
       fprintf(stdout,"v[%d]:%g != %g\n",c,fitA.rng[c].v,fitB.rng[c].v);
     } 

     if (fitA.rng[c].v_err !=fitB.rng[c].v_err) {
       fprintf(stdout,"v_err[%d]:%g != %g\n",c,fitA.rng[c].v_err,fitB.rng[c].v_err);
     
     } 

     if (fitA.rng[c].p_0 !=fitB.rng[c].p_0) {
       fprintf(stdout,"p_0[%d]:%g != %g\n",c,fitA.rng[c].p_0,fitB.rng[c].p_0);
     } 

     if (fitA.rng[c].p_l !=fitB.rng[c].p_l) {
        fprintf(stdout,"p_l[%d]:%g != %g\n",c,fitA.rng[c].p_l,fitB.rng[c].p_l);
     } 

     if (fitA.rng[c].p_l_err !=fitB.rng[c].p_l_err) {
       fprintf(stdout,"p_l_err[%d]:%g != %g\n",c,fitA.rng[c].p_l_err,fitB.rng[c].p_l_err);
     } 


     if (fitA.rng[c].p_s !=fitB.rng[c].p_s) {
       fprintf(stdout,"p_s[%d]:%g != %g\n",c,fitA.rng[c].p_s,fitB.rng[c].p_s);
     } 

     if (fitA.rng[c].p_s_err !=fitB.rng[c].p_s_err) {
       fprintf(stdout,"p_s_err[%d]:%g != %g\n",c,fitA.rng[c].p_s_err,fitB.rng[c].p_s_err);
     } 

     if (fitA.rng[c].w_l !=fitB.rng[c].w_l) {
       fprintf(stdout,"w_l[%d]:%g != %g\n",c,fitA.rng[c].w_l,fitB.rng[c].w_l);
     } 

     if (fitA.rng[c].w_l_err !=fitB.rng[c].w_l_err) {
       fprintf(stdout,"w_l_err[%d]:%g != %g\n",c,fitA.rng[c].w_l_err,fitB.rng[c].w_l_err);
     } 


     if (fitA.rng[c].w_s !=fitB.rng[c].w_s) {
       fprintf(stdout,"w_s[%d]:%g != %g\n",c,fitA.rng[c].w_s,fitB.rng[c].w_s);
     } 

     if (fitA.rng[c].w_s_err !=fitB.rng[c].w_s_err) {
       fprintf(stdout,"w_s_err[%d]:%g != %g\n",c,fitA.rng[c].w_s_err,fitB.rng[c].w_s_err);
     } 


    if (fitA.rng[c].phi0 !=fitB.rng[c].phi0) {
       fprintf(stdout,"phi0[%d]:%g != %g\n",c,fitA.rng[c].phi0,fitB.rng[c].phi0);
     } 

     if (fitA.rng[c].phi0_err !=fitB.rng[c].phi0_err) {
       fprintf(stdout,"phi0_err[%d]:%g != %g\n",c,fitA.rng[c].phi0_err,fitB.rng[c].phi0_err);
     } 


    if (fitA.rng[c].sdev_l !=fitB.rng[c].sdev_l) {
       fprintf(stdout,"sdev_l[%d]:%g != %g\n",c,fitA.rng[c].sdev_l,fitB.rng[c].sdev_l);
     } 
 

    if (fitA.rng[c].sdev_s !=fitB.rng[c].sdev_s) {
       fprintf(stdout,"sdev_s[%d]:%g != %g\n",c,fitA.rng[c].sdev_s,fitB.rng[c].sdev_s);
     } 
 
     if (fitA.rng[c].sdev_phi !=fitB.rng[c].sdev_phi) {
       fprintf(stdout,"sdev_phi[%d]:%g != %g\n",c,fitA.rng[c].sdev_phi,fitB.rng[c].sdev_phi);
     } 
 

     if (fitA.rng[c].qflg !=fitB.rng[c].qflg) {
       fprintf(stdout,"qflg[%d]:%d != %d\n",c,fitA.rng[c].qflg,fitB.rng[c].qflg);
     } 
     if (fitA.rng[c].gsct !=fitB.rng[c].gsct) {
       fprintf(stdout,"gsct[%d]:%d != %d\n",c,fitA.rng[c].gsct,fitB.rng[c].gsct);
     } 
     if (fitA.rng[c].nump !=fitB.rng[c].nump) {
      fprintf(stdout,"nump[%d]:%d != %d\n",c,fitA.rng[c].nump,fitB.rng[c].nump);
     }    
   }
 
   if (fitA.prms.XCF==0) continue;
  
   for (c=0;c<fitA.prms.NRANG;c++) {
     if (fitA.xrng[c].v !=fitB.xrng[c].v) {
     fprintf(stdout,"X v[%d]:%g != %g\n",c,fitA.xrng[c].v,fitB.xrng[c].v);
     } 

     if (fitA.xrng[c].v_err !=fitB.xrng[c].v_err) {
     fprintf(stdout,"X v_err[%d]:%g != %g\n",c,fitA.xrng[c].v_err,fitB.xrng[c].v_err);
     
     } 

     if (fitA.xrng[c].p_0 !=fitB.xrng[c].p_0) {
     fprintf(stdout,"X p_0[%d]:%g != %g\n",c,fitA.xrng[c].p_0,fitB.xrng[c].p_0);
     } 

     if (fitA.xrng[c].p_l !=fitB.xrng[c].p_l) {
     fprintf(stdout,"X p_l[%d]:%g != %g\n",c,fitA.xrng[c].p_l,fitB.xrng[c].p_l);
     } 

     if (fitA.xrng[c].p_l_err !=fitB.xrng[c].p_l_err) {
     fprintf(stdout,"X p_l_err[%d]:%g != %g\n",c,fitA.xrng[c].p_l_err,fitB.xrng[c].p_l_err);
     } 


     if (fitA.xrng[c].p_s !=fitB.xrng[c].p_s) {
     fprintf(stdout,"X p_s[%d]:%g != %g\n",c,fitA.xrng[c].p_s,fitB.xrng[c].p_s);
     } 

     if (fitA.xrng[c].p_s_err !=fitB.xrng[c].p_s_err) {
     fprintf(stdout,"X p_s_err[%d]:%g != %g\n",c,fitA.xrng[c].p_s_err,fitB.xrng[c].p_s_err);
     } 

     if (fitA.xrng[c].w_l !=fitB.xrng[c].w_l) {
     fprintf(stdout,"X w_l[%d]:%g != %g\n",c,fitA.xrng[c].w_l,fitB.xrng[c].w_l);
     } 

     if (fitA.xrng[c].w_l_err !=fitB.xrng[c].w_l_err) {
     fprintf(stdout,"X w_l_err[%d]:%g != %g\n",c,fitA.xrng[c].w_l_err,fitB.xrng[c].w_l_err);
     } 


     if (fitA.xrng[c].w_s !=fitB.xrng[c].w_s) {
     fprintf(stdout,"X w_s[%d]:%g != %g\n",c,fitA.xrng[c].w_s,fitB.xrng[c].w_s);
     } 

     if (fitA.xrng[c].w_s_err !=fitB.xrng[c].w_s_err) {
     fprintf(stdout,"w_s_err[%d]:%g != %g\n",c,fitA.xrng[c].w_s_err,fitB.xrng[c].w_s_err);
     } 


    if (fitA.xrng[c].phi0 !=fitB.xrng[c].phi0) {
     fprintf(stdout,"X phi0[%d]:%g != %g\n",c,fitA.xrng[c].phi0,fitB.xrng[c].phi0);
     } 

     if (fitA.xrng[c].phi0_err !=fitB.xrng[c].phi0_err) {
     fprintf(stdout,"X phi0_err[%d]:%g != %g\n",c,fitA.xrng[c].phi0_err,fitB.xrng[c].phi0_err);
     } 


    if (fitA.xrng[c].sdev_l !=fitB.xrng[c].sdev_l) {
     fprintf(stdout,"X sdev_l[%d]:%g != %g\n",c,fitA.xrng[c].sdev_l,fitB.xrng[c].sdev_l);
     } 
 

    if (fitA.xrng[c].sdev_s !=fitB.xrng[c].sdev_s) {
     fprintf(stdout,"X sdev_s[%d]:%g != %g\n",c,fitA.xrng[c].sdev_s,fitB.xrng[c].sdev_s);
     } 
 
     if (fitA.xrng[c].sdev_phi !=fitB.xrng[c].sdev_phi) {
     fprintf(stdout,"X sdev_phi[%d]:%g != %g\n",c,fitA.xrng[c].sdev_phi,fitB.xrng[c].sdev_phi);
     } 
 

     if (fitA.xrng[c].qflg !=fitB.xrng[c].qflg) {
     fprintf(stdout,"X qflg[%d]:%d != %d\n",c,fitA.xrng[c].qflg,fitB.xrng[c].qflg);
     } 
     if (fitA.xrng[c].gsct !=fitB.xrng[c].gsct) {
     fprintf(stdout,"X gsct[%d]:%d != %d\n",c,fitA.xrng[c].gsct,fitB.xrng[c].gsct);
     } 
     if (fitA.xrng[c].nump !=fitB.xrng[c].nump) {
     fprintf(stdout,"X nump[%d]:%d != %d\n",c,fitA.xrng[c].nump,fitB.xrng[c].nump);
     }    


    if (fitA.elev[c].normal !=fitB.elev[c].normal) {
     fprintf(stdout,"elevation.normal[%d]:%g != %g\n",c,fitA.elev[c].normal,fitB.elev[c].normal);
     }    

     if (fitA.elev[c].low !=fitB.elev[c].low) {
     fprintf(stdout,"elevation.low[%d]:%g != %g\n",c,fitA.elev[c].low,fitB.elev[c].low);
     }    

   if (fitA.elev[c].high !=fitB.elev[c].high) {
     fprintf(stdout,"elevation.high[%d]:%g != %g\n",c,fitA.elev[c].high,fitB.elev[c].high);
     }    


   }
   if (c !=fitA.prms.NRANG) fprintf(stdout,"Data does not match\n");

 


  }
  return 0;
}
   











