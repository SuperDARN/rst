/* make_info.c
   ===========*/



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
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitdata.h"
#include "cfitdata.h"

#include "cfitread.h"
#include "fitread.h"
#include "oldfitread.h"
#include "rawread.h"
#include "oldrawread.h"
#include "errstr.h"
#include "hlpstr.h"




struct RadarParm *prm;
struct FitData *fit;
struct RawData *raw;
struct CFitdata *cfit;
struct OptionData opt;


int main(int argc,char *argv[]) {

/* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  int old=0;
  int new=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;

  int gsct=0,isct=0,i,c;
  struct OldRawFp *rawfp=NULL;
  struct OldFitFp *fitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  FILE *fp=NULL;

  int yr,mo,dy,hr,mt,isc;
  double dsc;
  int us;

  char combf[128];
  int j,k; 

  unsigned char fitflg=0;
  unsigned char rawflg=0;
  unsigned char cfitflg=0;

  prm=RadarParmMake();
  raw=RawMake();
  fit=FitMake();
  cfit=CFitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 


  OptionAdd(&opt,"raw",'x',&rawflg);
  OptionAdd(&opt,"fit",'x',&fitflg);
  OptionAdd(&opt,"cfit",'x',&cfitflg);


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if ((cfitflg==0) && (rawflg==0)) fitflg=1;


  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  if (fitflg) {
    if (old) {
      for (c=arg;c<argc;c++) {
        fitfp=OldFitOpen(argv[c],NULL); 
        fprintf(stderr,"Opening file %s\n",argv[c]);
        if (fitfp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[c]);
          continue;
        }

        while (OldFitRead(fitfp,prm,fit) !=-1) {
   
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
                 prm->time.yr,prm->time.mo,prm->time.dy,prm->time.hr,
                 prm->time.mt,
	         prm->time.sc,prm->time.us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",prm->stid,
                prm->bmnum,prm->channel,
                 prm->scan); 
        fprintf(stdout,"%8d %3d %3d",prm->cp,prm->intt.sc,prm->intt.us);
        fprintf(stdout,"%5d %5d ",prm->frang,prm->rsep);
        fprintf(stdout,"%6d %6g ",prm->tfreq,prm->noise.search);
        fprintf(stdout,"%2d %4d ",prm->atten,prm->nave);
        fprintf(stdout,"%3d ",prm->nrang);

        k=0;
        for (j=0;prm->combf[j] !=0;j++) 
          if (isprint(prm->combf[j])) {
	    combf[k]=prm->combf[j];
            k++;
	  }
	combf[k]=0;

        gsct=0;
        isct=0;
        for (i=0;i<prm->nrang;i++) {
            if (fit->rng[i].qflg==1) {
              if (fit->rng[i].gsct==1) gsct++;
              else isct++;
            }
          }
          fprintf(stdout,"%3d %3d ",gsct,isct);
          fprintf(stdout,"\"%s\"",combf);
          fprintf(stdout,"\n");

        }
        OldFitClose(fitfp);
      }
    } else {
      if (arg==argc) fp=stdin;
      else {
        fp=fopen(argv[arg],"r");
        if (fp==NULL) {
          fprintf(stderr,"Could not open file.\n");
          exit(-1);
        }
      }
      while (FitFread(fp,prm,fit) !=-1) {
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
                 prm->time.yr,prm->time.mo,prm->time.dy,
                 prm->time.hr,prm->time.mt,prm->time.sc,prm->time.us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",prm->stid,
                prm->bmnum,prm->channel,
                prm->scan); 
        fprintf(stdout,"%8d %3d %3d",prm->cp,prm->intt.sc,prm->intt.us);
        fprintf(stdout,"%5d %5d ",prm->frang,prm->rsep);
        fprintf(stdout,"%6d %6g ",prm->tfreq,prm->noise.search);
        fprintf(stdout,"%2d %4d ",prm->atten,prm->nave);
        fprintf(stdout,"%3d ",prm->nrang);

        k=0;
        for (j=0;prm->combf[j] !=0;j++)
          if (isprint(prm->combf[j])) {
            combf[k]=prm->combf[j];
            k++;
          }
        combf[k]=0;


        gsct=0;
        isct=0;
        for (i=0;i<prm->nrang;i++) {
            if (fit->rng[i].qflg==1) {
              if (fit->rng[i].gsct==1) gsct++;
              else isct++;
            }
          }
          fprintf(stdout,"%3d %3d ",gsct,isct);
          fprintf(stdout,"\"%s\"",combf);
          fprintf(stdout,"\n");
      }
      if (fp !=stdin) fclose(fp);
    }
  } else if (rawflg) {
     if (old) {
      for (c=arg;c<argc;c++) {
        rawfp=OldRawOpen(argv[c],NULL); 
        fprintf(stderr,"Opening file %s\n",argv[c]);
        if (rawfp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[c]);
          continue;
        }

        while (OldRawRead(rawfp,prm,raw) !=-1) {
   
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
                prm->time.yr,prm->time.mo,prm->time.dy,
                prm->time.hr,prm->time.mt,prm->time.sc,prm->time.us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",prm->stid,
                prm->bmnum,prm->channel,
                 prm->scan); 
        fprintf(stdout,"%8d %3d %3d",prm->cp,prm->intt.sc,prm->intt.us);
        fprintf(stdout,"%5d %5d ",prm->frang,prm->rsep);
        fprintf(stdout,"%6d %6g ",prm->tfreq,prm->noise.search);
        fprintf(stdout,"%2d %4d ",prm->atten,prm->nave);
        fprintf(stdout,"%3d ",prm->nrang);

        k=0;
        for (j=0;prm->combf[j] !=0;j++)
          if (isprint(prm->combf[j])) {
            combf[k]=prm->combf[j];
            k++;
          }
        combf[k]=0;


	/*
        gsct=0;
        isct=0;
        for (i=0;i<prm.nrang;i++) {
            if (raw.rng[i].qflg==1) {
              if (raw.rng[i].gsct==1) gsct++;
              else isct++;
            }
          }
        */
          fprintf(stdout,"%3d %3d ",gsct,isct);
          fprintf(stdout,"\"%s\"",combf);
          fprintf(stdout,"\n");

        }
        OldRawClose(rawfp);
      }
    } else {
      if (arg==argc) fp=stdin;
      else {
        fp=fopen(argv[arg],"r");
        if (fp==NULL) {
          fprintf(stderr,"Could not open file.\n");
          exit(-1);
        }
      }
      while (RawFread(fp,prm,raw) !=-1) {
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
                 prm->time.yr,prm->time.mo,prm->time.dy,
                 prm->time.hr,prm->time.mt,prm->time.sc,prm->time.us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",prm->stid,
                prm->bmnum,prm->channel,
                prm->scan); 
        fprintf(stdout,"%8d %3d %3d",prm->cp,prm->intt.sc,prm->intt.us);
        fprintf(stdout,"%5d %5d ",prm->frang,prm->rsep);
        fprintf(stdout,"%6d %6g ",prm->tfreq,prm->noise.search);
        fprintf(stdout,"%2d %4d ",prm->atten,prm->nave);
        fprintf(stdout,"%3d ",prm->nrang);

        k=0;
        for (j=0;prm->combf[j] !=0;j++)
          if (isprint(prm->combf[j])) {
            combf[k]=prm->combf[j];
            k++;
          }
        combf[k]=0;


        gsct=0;
        isct=0;
        /*
        for (i=0;i<prm.nnrang;i++) {
            if (raw.rng[i].qflg==1) {
              if (raw.rng[i].gsct==1) gsct++;
              else isct++;
            }
          }
	*/
          fprintf(stdout,"%3d %3d ",gsct,isct);
          fprintf(stdout,"\"%s\"",combf);
          fprintf(stdout,"\n");
      }
      if (fp !=stdin) fclose(fp);
    }



   
  } else {
    for (c=arg;c<argc;c++) {
      cfitfp=CFitOpen(argv[c]);
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (cfitfp==NULL) {
	fprintf(stderr,"file %s not found\n",argv[c]);
	continue;
      }

      while (CFitRead(cfitfp,cfit) !=-1) {
        TimeEpochToYMDHMS(cfit->time,&yr,&mo,&dy,&hr,&mt,&dsc);
        isc=(int) dsc;
        us=1.0e6*(dsc-isc);
	fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
		yr,mo,dy,hr,mt,isc,us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",cfit->stid,
                cfit->bmnum,cfit->channel,
                cfit->scan);
        fprintf(stdout,"%8d %3d %3d",cfit->cp,cfit->intt.sc,cfit->intt.us);
        fprintf(stdout,"%5d %5d ",cfit->frang,cfit->rsep);
        fprintf(stdout,"%6d %6g ",cfit->tfreq,cfit->noise);
        fprintf(stdout,"%2d %4d ",cfit->atten,cfit->nave);
        fprintf(stdout,"%3d ",cfit->nrang);

        gsct=0;
        isct=0;
        for (i=0;i<cfit->num;i++) {
	  if (cfit->data[i].gsct==1) gsct++;
	  else isct++;
	  
	}
	fprintf(stdout,"%3d %3d ",gsct,isct);
        fprintf(stdout,"\"\""); 
	fprintf(stdout,"\n");
      }
      CFitClose(cfitfp);
    }
  }


  return 0;
}
   









