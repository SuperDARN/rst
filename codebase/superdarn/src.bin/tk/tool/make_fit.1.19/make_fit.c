/* make_fit.c
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2010 The Johns Hopkins University/Applied Physics Laboratory & Others 

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
  2020-05-07 Marina Schmidt Added Free functions
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

/*Following coding style: https://www.cs.swarthmore.edu/~newhall/unixhelp/c_codestyle.html*/

/*
 *Function: free_files
 *---------------------
 *Frees file types used in make_fit
 *  rawfp: old RAWACF file pointer 
 *  fp: new RAWACF file pointer
 *  fitfp: FITACF file pointer
 *  inxfp: Index file pointer
 *
 *  returns: nothing
 */
void free_files(struct OldRawFp *rawfp, FILE *fp, FILE *fitfp, FILE *inxfp)
{
    if (rawfp != NULL) OldRawClose(rawfp);
    if (fp != NULL) fclose(fp);
    if (fitfp != NULL) fclose(fitfp);
    if (inxfp != NULL) fclose(inxfp);
}

/*
 *Function: free_fitstructs
 *-----------------------------
 *  fit_prms: fitacf parameters used in FITACF 3.0 algorithm
 *  fit: fitacf data structure
 *  FitBlock: fitacf block used in FITACF 2.5 algorithm
 * 
 *  returns: nothing
 */
void free_fitstructs(FITPRMS *fit_prms, struct FitData *fit, struct FitBlock *fblk)
{
    if (fit != NULL) FitFree(fit);
    if (fit_prms != NULL) FitacfFree(fit_prms);
    if (fblk != NULL) FitACFFree(fblk); 
}

/*
 *Function: free_radarstructs
 *---------------------------
 *  network: Radar network structure
 *  RadarParam: radar parameter structure
 *  RawData: rawacf data structure
 *
 *  return: nothing
 */
void free_radarstructs(struct RadarNetwork *network, struct RadarParm *prm, struct RawData *raw)
{
    if (network != NULL) RadarFree(network);
    if (prm != NULL) RadarParmFree(prm);
    if (raw != NULL) RawFree(raw);
}

int main(int argc,char *argv[]) {

  unsigned char old=0;

  char *envstr;
  int status;
  int arg;
  int return_value = 0; 
  int elv_version = 2;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  unsigned char old_elev=0;

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
  int fitacf_version = 25;
  FITPRMS *fit_prms = NULL;

  /*feenableexcept(FE_INVALID | FE_OVERFLOW);*/

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"old_elev",'x',&old_elev);   /* set to use old elev ang alg */

  OptionAdd(&opt,"old",'x',&old);

  OptionAdd(&opt,"fitacf-version",'t',&fitacf_version_s);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
      fprintf(stderr,"Error processing options\n");
      OptionFree(&opt);
      exit(-1);
  }

  if (option==1) {
      OptionDump(stdout,&opt);
      OptionFree(&opt);
      exit(0);
  }

  if (version==1) {
      OptionVersion(stdout);
      OptionFree(&opt);
      exit(0);
  }

  if ((old) && (argc-arg<2)) {
      OptionPrintInfo(stdout,hlpstr);
      OptionFree(&opt);  
      exit(-1);
  }
  
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    OptionFree(&opt);
    exit(0);
  }
  
  if (old_elev == 1)
  {
      elv_version = 1;
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
      OptionFree(&opt);
      exit(-1);
    }
  }

  if (vb) {
    fprintf(stderr, "Using fitacf version: %0.1f\n", (float)fitacf_version/10);
  }

 
  OptionFree(&opt);
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
    RadarFree(network);
    exit(-1);
  }

  return_value = RadarLoadHardware(envstr,network);
  if (return_value == -1)
  {
      fprintf(stderr,"Could not load hardware file\n");
      RadarFree(network);
      free(envstr);
      exit(-1);
  }


  prm=RadarParmMake();
  if (prm == NULL)
  {
      RadarFree(network);
      fprintf(stderr,"Error: cannot create Radar parameter block\n");
      exit(-1);
  }
  raw=RawMake();
  if (raw == NULL)
  {
      fprintf(stderr,"Error: cannot read Rawacf structure\n");
      free_radarstructs(network, prm, raw);
      exit(-1);
  }

  if (old) {
     rawfp=OldRawOpen(argv[arg],NULL);
     if (rawfp==NULL) {
       fprintf(stderr,"File not found.\n");
       free_radarstructs(network, prm, raw);
       exit(-1);
     } else if (rawfp->error == -2) {
       /* Error case where num_bytes is less than 0 */
       OldRawClose(rawfp);
       free_radarstructs(network, prm, raw);
       exit(-1);
     }
     status=OldRawRead(rawfp,prm,raw);
  } 
  else {
    if (arg==argc) 
        fp=stdin;
    else 
        fp=fopen(argv[arg],"r");
    if (fp==NULL) {
        fprintf(stderr,"File not found.\n");
        free_radarstructs(network, prm, raw);
        exit(-1);
    }
    status=RawFread(fp,prm,raw);
  }

  radar=RadarGetRadar(network,prm->stid);
  if (radar==NULL) {
    fprintf(stderr,"Failed to get radar information.\n");
    free_radarstructs(network, prm, raw);
    free_files(rawfp, fp, fitfp, inxfp);
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,
		          prm->time.dy,prm->time.hr,prm->time.mt,
                          prm->time.sc);

  if (site==NULL) {
    fprintf(stderr,"Failed to get site information.\n");
    free_radarstructs(network, prm, raw);
    free_files(rawfp, fp, fitfp, inxfp);
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

  fit=FitMake();
  if (fit == NULL)
  {
    fprintf(stderr, "Error: cannot allocate memory for fitdata structure\n");
    free_radarstructs(network, prm, raw);
    free_files(rawfp, fp, fitfp, inxfp);
    exit(-1);
  }

  if (fitacf_version == 30){
      /* Allocate the memory for the FIT parameter structure */
      /* and initialise the values to zero.                  */
      fit_prms = malloc(sizeof(*fit_prms));
      if (fit_prms == NULL)
      {
          fprintf(stderr,"Error: Could not allocate memory for fitacf parameter structure \n");
          free_radarstructs(network, prm, raw);
          free_files(rawfp, fp, fitfp, inxfp);
          free_fitstructs(fit_prms, fit, fblk);
          exit(-1);
      }      
      
      memset(fit_prms, 0, sizeof(*fit_prms));
      if (fit_prms == NULL)
      {
          fprintf(stderr,"Error: Could not allocate memory for fitacf parameter structure \n");
          free_radarstructs(network, prm, raw);
          free_files(rawfp, fp, fitfp, inxfp);
          free_fitstructs(fit_prms, fit, fblk);
          exit(-1);
      }
            
      if (Allocate_Fit_Prm(prm, fit_prms) == -1) { 
          fprintf(stderr,"Error: Could not allocate space for FITACF 3.0 Parameter structure\n");
          free_radarstructs(network, prm, raw);
          free_files(rawfp, fp, fitfp, inxfp);
          free_fitstructs(fit_prms, fit, fblk);
          exit(-1);
      }

      /* If the allocation was successful, copy the parameters and */
      /* load the data into the FitACF structure.                  */
      if(fit_prms != NULL) {
    	  if (prm->stid == 1 && elv_version == 1)
          {
              elv_version = 0;
          }
          Copy_Fitting_Prms(site,prm,raw,fit_prms);
    	  Fitacf(fit_prms,fit, elv_version);
        /*FitacfFree(fit_prms);*/
    	}
      else {
          fprintf(stderr, "Unable to allocate fit_prms!\n");
          free_radarstructs(network, prm, raw);
          free_files(rawfp, fp, fitfp, inxfp);
          free_fitstructs(fit_prms, fit, fblk);
          exit(-1);
      }
  }
  else if (fitacf_version == 25) {
    fblk = FitACFMake(site,prm->time.yr);
    fblk->prm.old_elev = old_elev;        /* passing in old_elev flag */
    FitACF(prm,raw,fblk,fit);
  }

  if (old) {
    char vstr[256];
    fitfp=fopen(argv[arg+1],"w");
    if (fitfp==NULL) {
        fprintf(stderr,"Could not create fit file.\n");
        free_radarstructs(network, prm, raw);
        free_files(rawfp, fp, fitfp, inxfp);
        free_fitstructs(fit_prms, fit, fblk);
        exit(-1);
    }
    if (argc-arg>2) {
      inxfp=fopen(argv[arg+2],"w");
      if (inxfp==NULL) {
        fprintf(stderr,"Could not create inx file.\n");
        free_radarstructs(network, prm, raw);
        free_files(rawfp, fp, fitfp, inxfp);
        free_fitstructs(fit_prms, fit, fblk);
        exit(-1);
      }
    }
    sprintf(vstr,"%d.%d",fit->revision.major,fit->revision.minor);
    OldFitHeaderFwrite(fitfp,"make_fit","fitacf",vstr);
    if (inxfp !=NULL) OldFitInxHeaderFwrite(inxfp,prm);
  }

  do {


    //set origin code = 1 which  means produced not at a radar site
    prm->origin.code = 1;
    ctime = time((time_t) 0);
    if (RadarParmSetOriginCommand(prm,command) == -1) 
    {
        fprintf(stderr,"Error: cannot set Origin Command\n");
        free_radarstructs(network, prm, raw);
        free_files(rawfp, fp, NULL, inxfp);
        free_fitstructs(fit_prms, fit, fblk);
        exit(-1);
    }
    
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    if (RadarParmSetOriginTime(prm,tmstr) == -1)
    {
        fprintf(stderr,"Error: cannot set Origin Time\n");
        free_radarstructs(network, prm, raw);
        free_files(rawfp, fp, NULL, inxfp);
        free_fitstructs(fit_prms, fit, fblk);
        exit(-1);
    }

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
            fprintf(stderr,"Error: cannot allocate space for fitacf record\n");
            free_radarstructs(network, prm, raw);
            free_files(rawfp, fp, NULL, inxfp);
            free_fitstructs(fit_prms, fit, fblk);            
            exit(-1);
        }

        /* If the allocation was successful, copy the parameters and */
        /* load the data into the FitACF structure.                  */
        if(fit_prms != NULL) {
          Copy_Fitting_Prms(site,prm,raw,fit_prms);
          Fitacf(fit_prms,fit, elv_version);
          /*FitacfFree(fit_prms);*/
        }
        else {
            fprintf(stderr, "Unable to allocate fit_prms!\n");
            free_radarstructs(network, prm, raw);
            free_files(rawfp, fp, NULL, inxfp);
            free_fitstructs(fit_prms, fit, fblk);
            exit(-1);
        }
      }
      else if (fitacf_version == 25) {
        FitACF(prm,raw,fblk,fit);
      }
      else {
            fprintf(stderr, "The requested fitacf version does not exist\n");
            free_radarstructs(network, prm, raw);
            free_files(rawfp, fp, NULL, inxfp);
            free_fitstructs(fit_prms, fit, fblk);
            exit(-1);
      }
    }


  } while (status==0);

  free_radarstructs(network, prm, raw);
  //free_files(rawfp, fp, NULL, inxfp);
  free_fitstructs(fit_prms, fit, fblk);

  return 0;
}
