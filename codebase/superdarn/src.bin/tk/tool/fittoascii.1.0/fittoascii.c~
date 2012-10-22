#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "rprm.h"
#include "invmag.h"
#include "rpos.h"
#include "fitdata.h"
#include "fitread.h"

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;
struct OptionData opt;


int main(int argc,char *argv[])
{
  /*declarations*/
  int arg = 0;
  int i, r;
  FILE *fp;
  FILE *fitfp = NULL;
	FILE *corrfp = NULL;
  char *envstr;
  unsigned char help = 0;
  unsigned char option = 0;
  int s = 0, vb = 0;
	double epoch;

  struct FitData *fit = FitMake();
  struct RadarParm *prm = RadarParmMake();


  envstr=getenv("SD_RADAR");
  if(envstr==NULL)
  {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    return -1;
  }
  fp=fopen(envstr,"r");

  if(fp==NULL)
  {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp);
  if(network==NULL)
  {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if(envstr==NULL)
  {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  /*add options to control performance*/
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vb);

  /*process the options*/
  arg=OptionProcess(1,argc,argv,&opt,NULL);
  if(option == 1)
  {
    OptionDump(stdout,&opt);
    exit(0);
  }

  /*read the radar's file*/
  fitfp=fopen(argv[arg],"r");
  fprintf(stderr,"%s\n",argv[arg]);
  if(fitfp==NULL)
  {
    fprintf(stderr,"File %s not found.\n",argv[arg]);
    exit(-1);
  }

	s=FitFread(fitfp,prm,fit);

  radar=RadarGetRadar(network,prm->stid);
  if (radar==NULL)
  {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }
  site=RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc);

	do
	{
		if((prm->channel == 0 || prm->channel == 1)  && prm->bmnum == 10)
		{
			fprintf(stdout,"%d  %d  %d  %d  %d  %d\n",prm->time.yr,prm->time.mo,prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc);
			fprintf(stdout,"%d  %d  %d  %lf  %d  %d  %d  %d  %d  %d\n",prm->cp,prm->tfreq,prm->bmnum,prm->noise.search,prm->nave,prm->lagfr,prm->mpinc,prm->smsep,prm->rsep,prm->nrang);
			for(i=0;i<prm->nrang;i++)
			{
				fprintf(stdout,"%d  %d  %lf  %lf  %lf\n",i,fit->rng[i].qflg,fit->rng[i].v,fit->rng[i].p_l,fit->rng[i].w_l);
			}
		}
		s=FitFread(fitfp,prm,fit);
	} while(s != -1);


	fclose(fitfp);

  return 0;
}
