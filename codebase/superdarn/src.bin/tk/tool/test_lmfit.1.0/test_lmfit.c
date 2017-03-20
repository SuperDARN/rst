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



struct RadarParm *prm;
struct RawData *raw;
struct FitData *fit;
struct FitBlock *fblk;

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

struct OptionData opt;

int main(int argc,char *argv[])
{

  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  unsigned char old=0;
  unsigned char new=1;

  char *envstr;
  int status;
  int arg;

  unsigned char help=0;
  unsigned char option=0;

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
	int tgthr=0,tgtmin=0,tgtbeam=7,tgtsec=0;
	int done=0;
 
  prm=RadarParmMake();
  raw=RawMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"new",'x',&new);
	OptionAdd(&opt,"hr",'i',&tgthr);
  OptionAdd(&opt,"min",'i',&tgtmin);
  OptionAdd(&opt,"sec",'i',&tgtsec);
	OptionAdd(&opt,"beam",'i',&tgtbeam);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;

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


  if (old)
	{
		rawfp=OldRawOpen(argv[arg],NULL);
    if (rawfp==NULL)
		{
			fprintf(stderr,"File not found.\n");
			exit(-1);
     }
     status=OldRawRead(rawfp,prm,raw);  
  }
  else
	{
		if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");

    if (fp==NULL)
		{
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


	if(prm->time.hr >= tgthr && prm->time.mt >= tgtmin && prm->time.sc >= tgtsec && prm->bmnum == tgtbeam && prm->channel 
= 2)
	{
		lmfit(prm,raw,fit,fblk,1);
		done=1;
	}

  do
  {
    ctime = time((time_t) 0); 
    RadarParmSetOriginCommand(prm,command);
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    RadarParmSetOriginTime(prm,tmstr);


    if(old)
      status=OldRawRead(rawfp,prm,raw);
    else
      status=RawFread(fp,prm,raw);

     if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);

		if(prm->time.hr >= tgthr && prm->time.mt >= tgtmin  && prm->time.sc >= tgtsec && prm->bmnum == tgtbeam
				&& status == 0 && !done && prm->channel != 2)
		{
			lmfit(prm,raw,fit,fblk,1);
			done = 1;
		}

  } while (status==0 && !done);

  FitACFFree(fblk);
  if (old) OldRawClose(rawfp);
  return 0;
}













