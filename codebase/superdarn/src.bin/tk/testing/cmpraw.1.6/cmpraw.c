/* cmpraw.c
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
#include "raw_read.h"



struct rawdata rawA;
struct rawdata rawB;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: cmpraw --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;


  int arg=0;

  struct rawfp *rawfpA=NULL;
  struct rawfp *rawfpB=NULL;
  
  int sA,sB;
  int c,x;

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


  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  rawfpA=raw_open(argv[arg],NULL);
  rawfpB=raw_open(argv[arg+1],NULL);

  while (1) {
    sA=raw_read(rawfpA,&rawA);
    sB=raw_read(rawfpB,&rawB);
    if ((sA==-1) || (sB==-1)) break;
  
    fprintf(stdout,
    "%.4d-%.2d-%.2d %.2d:%.2d:%.2d\t%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
	    rawA.PARMS.YEAR,rawA.PARMS.MONTH,rawA.PARMS.DAY,
            rawA.PARMS.HOUR,rawA.PARMS.MINUT,rawA.PARMS.SEC,
            rawB.PARMS.YEAR,rawB.PARMS.MONTH,rawB.PARMS.DAY,
            rawB.PARMS.HOUR,rawB.PARMS.MINUT,rawB.PARMS.SEC);
    
    if (rawA.PARMS.REV.MAJOR !=rawB.PARMS.REV.MAJOR) fprintf(stdout,"raw.PARMS.REV.MAJOR: %d !=%d \n",rawA.PARMS.REV.MAJOR,rawB.PARMS.REV.MAJOR);


    if (rawA.PARMS.REV.MINOR !=rawB.PARMS.REV.MINOR) fprintf(stdout,"raw.PARMS.REV.MINOR: %d !=%d \n",rawA.PARMS.REV.MINOR,rawB.PARMS.REV.MINOR);

    if (rawA.PARMS.NPARM !=rawB.PARMS.NPARM) fprintf(stdout,"raw.PARMS.NPARM: %d !=%d \n",rawA.PARMS.NPARM,rawB.PARMS.NPARM);

   if (rawA.PARMS.ST_ID !=rawB.PARMS.ST_ID) fprintf(stdout,"raw.PARMS.ST_ID: %d !=%d \n",rawA.PARMS.ST_ID,rawB.PARMS.ST_ID);

   if (rawA.PARMS.YEAR !=rawB.PARMS.YEAR) fprintf(stdout,"raw.PARMS.YEAR: %d !=%d \n",rawA.PARMS.YEAR,rawB.PARMS.YEAR);

   if (rawA.PARMS.MONTH !=rawB.PARMS.MONTH) fprintf(stdout,"raw.PARMS.MONTH: %d !=%d \n",rawA.PARMS.MONTH,rawB.PARMS.MONTH);

   if (rawA.PARMS.DAY !=rawB.PARMS.DAY) fprintf(stdout,"raw.PARMS.DAY: %d !=%d \n",rawA.PARMS.DAY,rawB.PARMS.DAY);

   if (rawA.PARMS.HOUR !=rawB.PARMS.HOUR) fprintf(stdout,"raw.PARMS.HOUR: %d !=%d \n",rawA.PARMS.HOUR,rawB.PARMS.HOUR);

   if (rawA.PARMS.MINUT !=rawB.PARMS.MINUT) fprintf(stdout,"raw.PARMS.MINUT: %d !=%d \n",rawA.PARMS.MINUT,rawB.PARMS.MINUT);

   if (rawA.PARMS.SEC !=rawB.PARMS.SEC) fprintf(stdout,"raw.PARMS.SEC: %d !=%d \n",rawA.PARMS.SEC,rawB.PARMS.SEC);

   if (rawA.PARMS.TXPOW !=rawB.PARMS.TXPOW) fprintf(stdout,"raw.PARMS.TXPOW: %d !=%d \n",rawA.PARMS.TXPOW,rawB.PARMS.TXPOW);

   if (rawA.PARMS.NAVE !=rawB.PARMS.NAVE) fprintf(stdout,"raw.PARMS.NAVE: %d !=%d \n",rawA.PARMS.NAVE,rawB.PARMS.NAVE);

   if (rawA.PARMS.ATTEN !=rawB.PARMS.ATTEN) fprintf(stdout,"raw.PARMS.ATTEN: %d !=%d \n",rawA.PARMS.ATTEN,rawB.PARMS.ATTEN);

   if (rawA.PARMS.LAGFR !=rawB.PARMS.LAGFR) fprintf(stdout,"raw.PARMS.LAGFR: %d !=%d \n",rawA.PARMS.LAGFR,rawB.PARMS.LAGFR); 

   if (rawA.PARMS.SMSEP !=rawB.PARMS.SMSEP) fprintf(stdout,"raw.PARMS.SMSEP: %d !=%d \n",rawA.PARMS.SMSEP,rawB.PARMS.SMSEP);


   if (rawA.PARMS.ERCOD !=rawB.PARMS.ERCOD) fprintf(stdout,"raw.PARMS.ERCOD: %d !=%d \n",rawA.PARMS.ERCOD,rawB.PARMS.ERCOD);

   if (rawA.PARMS.AGC_STAT !=rawB.PARMS.AGC_STAT) fprintf(stdout,"raw.PARMS.AGC_STAT: %d !=%d \n",rawA.PARMS.AGC_STAT,rawB.PARMS.AGC_STAT);

   if (rawA.PARMS.LOPWR_STAT !=rawB.PARMS.LOPWR_STAT) fprintf(stdout,"raw.PARMS.LOPWR_STAT: %d !=%d \n",rawA.PARMS.LOPWR_STAT,rawB.PARMS.LOPWR_STAT);

   if (rawA.PARMS.NBAUD !=rawB.PARMS.NBAUD) fprintf(stdout,"raw.PARMS.NBAUD: %d !=%d \n",rawA.PARMS.NBAUD,rawB.PARMS.NBAUD);

   if (rawA.PARMS.NOISE !=rawB.PARMS.NOISE) fprintf(stdout,"raw.PARMS.NOISE: %d !=%d \n",rawA.PARMS.NOISE,rawB.PARMS.NOISE);

   if (rawA.PARMS.NOISE_MEAN !=rawB.PARMS.NOISE_MEAN) fprintf(stdout,"raw.PARMS.NOISE_MEAN: %d !=%d \n",rawA.PARMS.NOISE_MEAN,rawB.PARMS.NOISE_MEAN);

   if (rawA.PARMS.CHANNEL !=rawB.PARMS.CHANNEL) fprintf(stdout,"raw.PARMS.CHANNEL: %d !=%d \n",rawA.PARMS.CHANNEL,rawB.PARMS.CHANNEL);


   if (rawA.PARMS.RXRISE !=rawB.PARMS.RXRISE) fprintf(stdout,"raw.PARMS.RXRISE: %d !=%d \n",rawA.PARMS.RXRISE,rawB.PARMS.RXRISE);


   if (rawA.PARMS.INTT !=rawB.PARMS.INTT) fprintf(stdout,"raw.PARMS.INTT: %d !=%d \n",rawA.PARMS.INTT,rawB.PARMS.INTT);


   if (rawA.PARMS.TXPL !=rawB.PARMS.TXPL) fprintf(stdout,"raw.PARMS.TXPL: %d !=%d \n",rawA.PARMS.TXPL,rawB.PARMS.TXPL);


   if (rawA.PARMS.MPINC !=rawB.PARMS.MPINC) fprintf(stdout,"raw.PARMS.MPINC: %d !=%d \n",rawA.PARMS.MPINC,rawB.PARMS.MPINC);




   if (rawA.PARMS.MPPUL !=rawB.PARMS.MPPUL) fprintf(stdout,"raw.PARMS.MPPUL: %d !=%d \n",rawA.PARMS.MPPUL,rawB.PARMS.MPPUL);


   if (rawA.PARMS.MPLGS !=rawB.PARMS.MPLGS) fprintf(stdout,"raw.PARMS.MPLGS: %d !=%d \n",rawA.PARMS.MPLGS,rawB.PARMS.MPLGS);


   if (rawA.PARMS.NRANG !=rawB.PARMS.NRANG) fprintf(stdout,"raw.PARMS.NRANG: %d !=%d \n",rawA.PARMS.NRANG,rawB.PARMS.NRANG);


   if (rawA.PARMS.FRANG !=rawB.PARMS.FRANG) fprintf(stdout,"raw.PARMS.FRANG: %d !=%d \n",rawA.PARMS.FRANG,rawB.PARMS.FRANG);


   if (rawA.PARMS.RSEP !=rawB.PARMS.RSEP) fprintf(stdout,"raw.PARMS.RSEP: %d !=%d \n",rawA.PARMS.RSEP,rawB.PARMS.RSEP);

   if (rawA.PARMS.BMNUM !=rawB.PARMS.BMNUM) fprintf(stdout,"raw.PARMS.BMNUM: %d !=%d \n",rawA.PARMS.BMNUM,rawB.PARMS.BMNUM);


   if (rawA.PARMS.XCF !=rawB.PARMS.XCF) fprintf(stdout,"raw.PARMS.XCF: %d !=%d \n",rawA.PARMS.XCF,rawB.PARMS.XCF);

 
   if (rawA.PARMS.TFREQ !=rawB.PARMS.TFREQ) fprintf(stdout,"raw.PARMS.TFREQ: %d !=%d \n",rawA.PARMS.TFREQ,rawB.PARMS.TFREQ);

   if (rawA.PARMS.SCAN !=rawB.PARMS.SCAN) fprintf(stdout,"raw.PARMS.SCAN: %d !=%d \n",rawA.PARMS.SCAN,rawB.PARMS.SCAN);

   if (rawA.PARMS.MXPWR !=rawB.PARMS.MXPWR) fprintf(stdout,"raw.PARMS.MXPWR: %d !=%d \n",rawA.PARMS.MXPWR,rawB.PARMS.MXPWR);


   if (rawA.PARMS.LVMAX !=rawB.PARMS.LVMAX) fprintf(stdout,"raw.PARMS.LVMAX: %d !=%d \n",rawA.PARMS.LVMAX,rawB.PARMS.LVMAX);


   if (rawA.PARMS.CP !=rawB.PARMS.CP) fprintf(stdout,"raw.PARMS.CP: %d !=%d \n",rawA.PARMS.CP,rawB.PARMS.CP);

   if (strcmp(rawA.COMBF,rawB.COMBF) !=0)  fprintf(stdout,"raw.combf: %s !=%s \n",rawA.COMBF,rawB.COMBF);

   for (c=0;c<rawA.PARMS.MPPUL;c++) if (rawA.PULSE_PATTERN[c] !=rawB.PULSE_PATTERN[c]) break;
   if (c !=rawA.PARMS.MPPUL) fprintf(stdout,"Pulse pattern does not match\n");

   for (c=0;c<rawA.PARMS.MPLGS;c++) {
     if (rawA.LAG_TABLE[0][c] !=rawB.LAG_TABLE[0][c]) break;
     if (rawA.LAG_TABLE[1][c] !=rawB.LAG_TABLE[1][c]) break;

   }
   if (c !=rawA.PARMS.MPLGS) fprintf(stdout,"Lag table does not match\n");

 
   for (c=0;c<rawA.PARMS.NRANG;c++) {
     if (rawA.pwr0[c] !=rawB.pwr0[c]) {
       fprintf(stdout,"pwr0[%d]: %g != %g\n",c,rawA.pwr0[c],rawB.pwr0[c]);
     }
   }

   if (c !=rawA.PARMS.NRANG) fprintf(stdout,"Lag-zero power does not match\n");

   for (c=0;c<rawA.PARMS.NRANG;c++) {
     for (x=0;x<rawA.PARMS.MPLGS;x++) {
     
       if (rawA.acfd[c][x][0] !=rawB.acfd[c][x][0]) {
         fprintf(stdout,"acfd[%d][%d][0]: %g != %g\n",c,x,rawA.acfd[c][x][0],
	         rawB.acfd[c][x][0]);
       }

       if (rawA.acfd[c][x][1] !=rawB.acfd[c][x][1]) {
         fprintf(stdout,"acfd[%d][%d][1]: %g != %g\n",c,x,rawA.acfd[c][x][1],
	       rawB.acfd[c][x][1]);
       }
     }
   }

   if (rawA.PARMS.XCF) {

     for (c=0;c<rawA.PARMS.NRANG;c++) {
       for (x=0;x<rawA.PARMS.MPLGS;x++) {
     
         if (rawA.acfd[c][x][0] !=rawB.acfd[c][x][0]) {
           fprintf(stdout,"acfd[%d][%d][0]: %g != %g\n",c,x,rawA.acfd[c][x][0],
	         rawB.acfd[c][x][0]);
         }

         if (rawA.acfd[c][x][1] !=rawB.acfd[c][x][1]) {
           fprintf(stdout,"acfd[%d][%d][1]: %g != %g\n",c,x,rawA.acfd[c][x][1],
	         rawB.acfd[c][x][1]);
         }
       }
     }

   }


   if (c !=rawA.PARMS.NRANG) fprintf(stdout,"Data does not match\n");

  }
  return 0;
}
   











