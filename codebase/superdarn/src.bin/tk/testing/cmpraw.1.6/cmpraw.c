/* cmpraw.c
   ======== 
   Author: R.J.Barnes
*/


/*
   See license.txt
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

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;


  int arg=0;

  struct rawfp *rawfpA=NULL;
  struct rawfp *rawfpB=NULL;
  
  int sA,sB;
  int c,x;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);  

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
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
    
    if (rawA.PARMS.REV.MAJOR !=rawB.PARMS.REV.MAJOR) fprintf(stdout,"raw.PARMS.REV.MAJOR: %d !=%d \n",rawA.PARMS.REV.MA
OR,rawB.PARMS.REV.MAJOR);


    if (rawA.PARMS.REV.MINOR !=rawB.PARMS.REV.MINOR) fprintf(stdout,"raw.PARMS.REV.MINOR: %d !=%d \n",rawA.PARMS.REV.MI
OR,rawB.PARMS.REV.MINOR);

    if (rawA.PARMS.NPARM !=rawB.PARMS.NPARM) fprintf(stdout,"raw.PARMS.NPARM: %d !=%d \n",rawA.PARMS.NPARM,rawB.PARMS.N
ARM);

   if (rawA.PARMS.ST_ID !=rawB.PARMS.ST_ID) fprintf(stdout,"raw.PARMS.ST_ID: %d !=%d \n",rawA.PARMS.ST_ID,rawB.PARMS.ST
ID);

   if (rawA.PARMS.YEAR !=rawB.PARMS.YEAR) fprintf(stdout,"raw.PARMS.YEAR: %d !=%d \n",rawA.PARMS.YEAR,rawB.PARMS.YEAR);

   if (rawA.PARMS.MONTH !=rawB.PARMS.MONTH) fprintf(stdout,"raw.PARMS.MONTH: %d !=%d \n",rawA.PARMS.MONTH,rawB.PARMS.MO
TH);

   if (rawA.PARMS.DAY !=rawB.PARMS.DAY) fprintf(stdout,"raw.PARMS.DAY: %d !=%d \n",rawA.PARMS.DAY,rawB.PARMS.DAY);

   if (rawA.PARMS.HOUR !=rawB.PARMS.HOUR) fprintf(stdout,"raw.PARMS.HOUR: %d !=%d \n",rawA.PARMS.HOUR,rawB.PARMS.HOUR);

   if (rawA.PARMS.MINUT !=rawB.PARMS.MINUT) fprintf(stdout,"raw.PARMS.MINUT: %d !=%d \n",rawA.PARMS.MINUT,rawB.PARMS.MI
UT);

   if (rawA.PARMS.SEC !=rawB.PARMS.SEC) fprintf(stdout,"raw.PARMS.SEC: %d !=%d \n",rawA.PARMS.SEC,rawB.PARMS.SEC);

   if (rawA.PARMS.TXPOW !=rawB.PARMS.TXPOW) fprintf(stdout,"raw.PARMS.TXPOW: %d !=%d \n",rawA.PARMS.TXPOW,rawB.PARMS.TX
OW);

   if (rawA.PARMS.NAVE !=rawB.PARMS.NAVE) fprintf(stdout,"raw.PARMS.NAVE: %d !=%d \n",rawA.PARMS.NAVE,rawB.PARMS.NAVE);

   if (rawA.PARMS.ATTEN !=rawB.PARMS.ATTEN) fprintf(stdout,"raw.PARMS.ATTEN: %d !=%d \n",rawA.PARMS.ATTEN,rawB.PARMS.AT
EN);

   if (rawA.PARMS.LAGFR !=rawB.PARMS.LAGFR) fprintf(stdout,"raw.PARMS.LAGFR: %d !=%d \n",rawA.PARMS.LAGFR,rawB.PARMS.LA
FR); 

   if (rawA.PARMS.SMSEP !=rawB.PARMS.SMSEP) fprintf(stdout,"raw.PARMS.SMSEP: %d !=%d \n",rawA.PARMS.SMSEP,rawB.PARMS.SM
EP);


   if (rawA.PARMS.ERCOD !=rawB.PARMS.ERCOD) fprintf(stdout,"raw.PARMS.ERCOD: %d !=%d \n",rawA.PARMS.ERCOD,rawB.PARMS.ER
OD);

   if (rawA.PARMS.AGC_STAT !=rawB.PARMS.AGC_STAT) fprintf(stdout,"raw.PARMS.AGC_STAT: %d !=%d \n",rawA.PARMS.AGC_STAT,r
wB.PARMS.AGC_STAT);

   if (rawA.PARMS.LOPWR_STAT !=rawB.PARMS.LOPWR_STAT) fprintf(stdout,"raw.PARMS.LOPWR_STAT: %d !=%d \n",rawA.PARMS.LOPW
_STAT,rawB.PARMS.LOPWR_STAT);

   if (rawA.PARMS.NBAUD !=rawB.PARMS.NBAUD) fprintf(stdout,"raw.PARMS.NBAUD: %d !=%d \n",rawA.PARMS.NBAUD,rawB.PARMS.NB
UD);

   if (rawA.PARMS.NOISE !=rawB.PARMS.NOISE) fprintf(stdout,"raw.PARMS.NOISE: %d !=%d \n",rawA.PARMS.NOISE,rawB.PARMS.NO
SE);

   if (rawA.PARMS.NOISE_MEAN !=rawB.PARMS.NOISE_MEAN) fprintf(stdout,"raw.PARMS.NOISE_MEAN: %d !=%d \n",rawA.PARMS.NOIS
_MEAN,rawB.PARMS.NOISE_MEAN);

   if (rawA.PARMS.CHANNEL !=rawB.PARMS.CHANNEL) fprintf(stdout,"raw.PARMS.CHANNEL: %d !=%d \n",rawA.PARMS.CHANNEL,rawB.
ARMS.CHANNEL);


   if (rawA.PARMS.RXRISE !=rawB.PARMS.RXRISE) fprintf(stdout,"raw.PARMS.RXRISE: %d !=%d \n",rawA.PARMS.RXRISE,rawB.PARM
.RXRISE);


   if (rawA.PARMS.INTT !=rawB.PARMS.INTT) fprintf(stdout,"raw.PARMS.INTT: %d !=%d \n",rawA.PARMS.INTT,rawB.PARMS.INTT);


   if (rawA.PARMS.TXPL !=rawB.PARMS.TXPL) fprintf(stdout,"raw.PARMS.TXPL: %d !=%d \n",rawA.PARMS.TXPL,rawB.PARMS.TXPL);


   if (rawA.PARMS.MPINC !=rawB.PARMS.MPINC) fprintf(stdout,"raw.PARMS.MPINC: %d !=%d \n",rawA.PARMS.MPINC,rawB.PARMS.MP
NC);




   if (rawA.PARMS.MPPUL !=rawB.PARMS.MPPUL) fprintf(stdout,"raw.PARMS.MPPUL: %d !=%d \n",rawA.PARMS.MPPUL,rawB.PARMS.MP
UL);


   if (rawA.PARMS.MPLGS !=rawB.PARMS.MPLGS) fprintf(stdout,"raw.PARMS.MPLGS: %d !=%d \n",rawA.PARMS.MPLGS,rawB.PARMS.MP
GS);


   if (rawA.PARMS.NRANG !=rawB.PARMS.NRANG) fprintf(stdout,"raw.PARMS.NRANG: %d !=%d \n",rawA.PARMS.NRANG,rawB.PARMS.NR
NG);


   if (rawA.PARMS.FRANG !=rawB.PARMS.FRANG) fprintf(stdout,"raw.PARMS.FRANG: %d !=%d \n",rawA.PARMS.FRANG,rawB.PARMS.FR
NG);


   if (rawA.PARMS.RSEP !=rawB.PARMS.RSEP) fprintf(stdout,"raw.PARMS.RSEP: %d !=%d \n",rawA.PARMS.RSEP,rawB.PARMS.RSEP);

   if (rawA.PARMS.BMNUM !=rawB.PARMS.BMNUM) fprintf(stdout,"raw.PARMS.BMNUM: %d !=%d \n",rawA.PARMS.BMNUM,rawB.PARMS.BM
UM);


   if (rawA.PARMS.XCF !=rawB.PARMS.XCF) fprintf(stdout,"raw.PARMS.XCF: %d !=%d \n",rawA.PARMS.XCF,rawB.PARMS.XCF);

 
   if (rawA.PARMS.TFREQ !=rawB.PARMS.TFREQ) fprintf(stdout,"raw.PARMS.TFREQ: %d !=%d \n",rawA.PARMS.TFREQ,rawB.PARMS.TF
EQ);

   if (rawA.PARMS.SCAN !=rawB.PARMS.SCAN) fprintf(stdout,"raw.PARMS.SCAN: %d !=%d \n",rawA.PARMS.SCAN,rawB.PARMS.SCAN);

   if (rawA.PARMS.MXPWR !=rawB.PARMS.MXPWR) fprintf(stdout,"raw.PARMS.MXPWR: %d !=%d \n",rawA.PARMS.MXPWR,rawB.PARMS.MX
WR);


   if (rawA.PARMS.LVMAX !=rawB.PARMS.LVMAX) fprintf(stdout,"raw.PARMS.LVMAX: %d !=%d \n",rawA.PARMS.LVMAX,rawB.PARMS.LV
AX);


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
   











