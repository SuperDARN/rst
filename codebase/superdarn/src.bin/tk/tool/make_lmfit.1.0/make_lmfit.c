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
  E.G.Thomas 2021-08: added support for multi-channel tdiff values
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <zlib.h>

#include "rtypes.h"
#include "option.h"

#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"

#include "lmfit.h"
#include "fitacf.h"
#include "rawread.h"
#include "fitwrite.h"

#include "oldrawread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"




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
  fprintf(stderr,"Please try: make_lmfit --help\n");
  return(-1);
}

int main(int argc,char *argv[])
{

  unsigned char old=0;

  char *envstr;
  int status;
  int arg;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  FILE *fp=NULL;
  struct OldRawFp *rawfp=NULL;
  FILE *fitfp=NULL;
  int irec=1;
  int drec=2;
  int dnum=0;

  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];

  prm=RadarParmMake();
  raw=RawMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"old",'x',&old);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1)
	{
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1)
	{
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }




  if ((old) && (argc-arg<2))
	{
    OptionPrintInfo(stdout,hlpstr);
    exit(-1);
  }

  envstr=getenv("SD_RADAR");
  if (envstr==NULL)
	{
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL)
	{
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp);
  if (network==NULL)
	{
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL)
	{
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);


  if (old) {
      rawfp=OldRawOpen(argv[arg],NULL);
      if (rawfp==NULL) {
          fprintf(stderr,"File not found.\n");
          exit(-1);
      } else if (rawfp->error==-2) {
          /* Error code for num_bytes less than 0 */
          free(rawfp);
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
  if (radar==NULL)
	{
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,
		          prm->time.dy,prm->time.hr,prm->time.mt,
                          prm->time.sc);

  if (site==NULL)
	{
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }


  command[0]=0;
  n=0;
  for (c=0;c<argc;c++)
	{
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }



  if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);

  fblk=FitACFMake(site,prm->time.yr);

  lmfit(prm,raw,fit,fblk,site,0);

  if (old)
  {
    char vstr[256];
    fitfp=fopen(argv[arg+1],"w");
    if (fitfp==NULL)
    {
      fprintf(stderr,"Could not create fit file.\n");
      exit(-1);
    }
    sprintf(vstr,"%d.%d",fit->revision.major,fit->revision.minor);
    OldFitHeaderFwrite(fitfp,"make_fit","fitacf",vstr);
  }

  do
  {
    //set origin code = 1 which  means produced not at a radar site
    prm->origin.code = 1;
    ctime = time((time_t) 0);
    RadarParmSetOriginCommand(prm,command);
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    RadarParmSetOriginTime(prm,tmstr);
/*
 int i;
		for(i=10;i<prm->nrang;i++)
			if(fit->rng[i].qflg)
				fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
					prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);
*/
    if (old)
    {
       dnum=OldFitFwrite(fitfp,prm,fit,NULL);
       drec+=dnum;
       irec++;
    }
    else status=FitFwrite(stdout,prm,fit);

    if(old)
      status=OldRawRead(rawfp,prm,raw);
    else
      status=RawFread(fp,prm,raw);

     if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);


    if (status==0)
			lmfit(prm,raw,fit,fblk,site,0);

  } while (status==0);

  FitACFFree(fblk);
  if (old) OldRawClose(rawfp);
  return 0;
}











