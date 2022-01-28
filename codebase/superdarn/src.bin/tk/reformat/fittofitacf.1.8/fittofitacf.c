/* fittofitacf.c
   ============= 
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
  E.G.Thomas 2021-08: added support for bmoff parameter
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitwrite.h"
#include "radar.h" 

#include "oldfitread.h"
#include "hlpstr.h"
#include "errstr.h"





struct OptionData opt;
struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fittofitacf --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  int arg=0;

  int s;
  struct OldFitFp *fitfp;
  struct RadarParm *rprm;
  struct FitData *fitacf;

  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];
 
  char *envstr=NULL;
  FILE *fp;
 
  float offset;
  int cnt=0;

  rprm=RadarParmMake();
  fitacf=FitMake();

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


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vb);
 
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

  fitfp=OldFitOpen(argv[arg],NULL);

  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);

  } 

  while ((s=OldFitRead(fitfp,rprm,fitacf)) !=-1) {

    rprm->origin.code=1;
    ctime = time((time_t) 0);
    RadarParmSetOriginCommand(rprm,command);
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    RadarParmSetOriginTime(rprm,tmstr);

    radar=RadarGetRadar(network,rprm->stid);
    site=RadarYMDHMSGetSite(radar,rprm->time.yr,
                    rprm->time.mo,rprm->time.dy,rprm->time.hr,rprm->time.mt,
			   rprm->time.sc);

    /* calculate beam azimuth */

    offset=site->maxbeam/2.0-0.5;
    rprm->bmazm=site->boresite+site->bmsep*(rprm->bmnum-offset)+site->bmoff;

    s=FitFwrite(stdout,rprm,fitacf); 
    if (s==-1) {
      cnt=-1;
      break;
    }
    if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",rprm->time.yr,
                    rprm->time.mo,rprm->time.dy,rprm->time.hr,rprm->time.mt,
                    rprm->time.sc);
    cnt++;
  
  }

  if (cnt==-1) exit(EXIT_FAILURE);
  exit(EXIT_SUCCESS);
  return 0; 

}



