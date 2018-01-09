/* make_fit.c
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.



*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

#include "rtypes.h"
#include "option.h"

#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"

#include "fitacf.h"
#include "rawread.h"
#include "fitwrite.h"

#include "oldrawread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

#include "fitacftoplevel.h"
#include "fit_structures.h"
#include <fenv.h>

struct RadarParm *prm;
struct RawData *raw;
struct FitData *fit;
struct FitBlock *fblk;

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_fit --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char old=0;

  char *envstr;
  int status;
  int arg;

  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  FILE *fp=NULL;
  struct OldRawFp *rawfp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;
  int irec=1;
  int drec=2;
  int dnum=0;

  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];

  char* fitacf_version_s = NULL;
  int fitacf_version;
  FITPRMS *fit_prms = NULL;

  prm=RadarParmMake();
  raw=RawMake();
  fit=FitMake();

  /*feenableexcept(FE_INVALID | FE_OVERFLOW);*/

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"old",'x',&old);

  OptionAdd(&opt,"fitacf-version",'t',&fitacf_version_s);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (fitacf_version_s != NULL) {
    if (strcmp(fitacf_version_s, "3.0") == 0){
      fitacf_version = 30;
    }
    else if (strcmp(fitacf_version_s, "2.5") == 0) {
      fitacf_version = 25;
    }
    else {
      fprintf(stderr, "The requested fitacf version does not exist\n");
      exit(-1);
    }
  }
  else {
    fitacf_version = 25;
  }

  if (vb) {
    fprintf(stderr, "Using fitacf version: %0.1f\n", (float)fitacf_version/10);
  }



  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }




  if ((old) && (argc-arg<2)) {
    OptionPrintInfo(stdout,hlpstr);
    exit(-1);
  }

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp);
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  if (old) {
     rawfp=OldRawOpen(argv[arg],NULL);
     if (rawfp==NULL) {
       fprintf(stderr,"File not found.\n");
       exit(-1);
     }
     status=OldRawRead(rawfp,prm,raw);
  } else {
    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");

    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    status=RawFread(fp,prm,raw);
  }

  radar=RadarGetRadar(network,prm->stid);
  if (radar==NULL) {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,
		          prm->time.dy,prm->time.hr,prm->time.mt,
                          prm->time.sc);

  if (site==NULL) {
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }


  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }



  if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);

  if (fitacf_version == 30){
      /* Allocate the memory for the FIT parameter structure */
      /* and initialise the values to zero.                  */
      fit_prms = malloc(sizeof(*fit_prms));
      memset(fit_prms, 0, sizeof(*fit_prms));
      if (Allocate_Fit_Prm(prm, fit_prms) == -1) {
        exit(-1);
      }

      /* If the allocation was successful, copy the parameters and */
      /* load the data into the FitACF structure.                  */
      if(fit_prms != NULL) {
    	  Copy_Fitting_Prms(site,prm,raw,fit_prms);
    	  Fitacf(fit_prms,fit);
        /*FitacfFree(fit_prms);*/
    	}
      else {
        fprintf(stderr, "Unable to allocate fit_prms!\n");
        exit(-1);
      }
  }
  else if (fitacf_version == 25) {
    fblk=FitACFMake(site,prm->time.yr);
    FitACF(prm,raw,fblk,fit);
  }
  else {
    fprintf(stderr, "The requested fitacf version does not exist\n");
    exit(-1);
  }

  if (old) {
    char vstr[256];
    fitfp=fopen(argv[arg+1],"w");
    if (fitfp==NULL) {
      fprintf(stderr,"Could not create fit file.\n");
      exit(-1);
    }
    if (argc-arg>2) {
      inxfp=fopen(argv[arg+2],"w");
      if (inxfp==NULL) {
        fprintf(stderr,"Could not create inx file.\n");
        exit(-1);
      }
    }
    sprintf(vstr,"%d.%d",fit->revision.major,fit->revision.minor);
    OldFitHeaderFwrite(fitfp,"make_fit","fitacf",vstr);
    if (inxfp !=NULL) OldFitInxHeaderFwrite(inxfp,prm);
  }



  do {


    ctime = time((time_t) 0);
    RadarParmSetOriginCommand(prm,command);
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    RadarParmSetOriginTime(prm,tmstr);

    if (old) {
       dnum=OldFitFwrite(fitfp,prm,fit,NULL);
       if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,prm);
       drec+=dnum;
       irec++;
    } else status=FitFwrite(stdout,prm,fit);

    if (old) status=OldRawRead(rawfp,prm,raw);
    else status=RawFread(fp,prm,raw);

     if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);


    if (status==0){
      if (fitacf_version == 30) {

        if (Allocate_Fit_Prm(prm, fit_prms) == -1) {
          exit(-1);
        }

        /* If the allocation was successful, copy the parameters and */
        /* load the data into the FitACF structure.                  */
        if(fit_prms != NULL) {
          Copy_Fitting_Prms(site,prm,raw,fit_prms);
          Fitacf(fit_prms,fit);
          /*FitacfFree(fit_prms);*/
        }
        else {
          fprintf(stderr, "Unable to allocate fit_prms!\n");
          exit(-1);
        }
      }
      else if (fitacf_version == 25) {
        FitACF(prm,raw,fblk,fit);
      }
      else {
        fprintf(stderr, "The requested fitacf version does not exist\n");
        exit(-1);
      }
    }


  } while (status==0);
  FitFree(fit);

  if (fitacf_version == 30) {
    FitacfFree(fit_prms);
  }
  else if (fitacf_version == 25) {
    FitACFFree(fblk);
  }
  else {

  }

  if (old) OldRawClose(rawfp);
  return 0;
}













