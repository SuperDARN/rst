/*
 * Copyright (C) <year>  <name of author>

Copyright (C) <year>  <name of author>
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

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fittoascii --help\n");
  return(-1);
}

int main(int argc,char *argv[])
{
  /*declarations*/
  int arg = 0;
  int i;
  FILE *fp;
  FILE *fitfp = NULL;
  char *envstr;
  unsigned char help = 0;
  unsigned char option = 0;
  unsigned char version=0;
  int s = 0, vb = 0;

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
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"vb",'x',&vb);

  /*process the options*/
  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if(option == 1)
  {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
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
			fprintf(stderr,"%d\n",prm->nrang);
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
