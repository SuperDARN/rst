/* dattorawacf.c
   =============
   Author: R.J.Barnes
*/


/*
   See license.txt
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
#include "rawdata.h"
#include "rawwrite.h"
#include "radar.h"



#include "oldrawread.h"

#include "errstr.h"
#include "hlpstr.h"

struct OptionData opt;
struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;
  float thr=-1;
  int arg=0;
  int s;

  struct OldRawFp *rawfp=NULL;
  struct RadarParm *rprm=NULL;
  struct RawData *rawacf=NULL;


  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];

  char *envstr=NULL;
  FILE *fp;

  float offset;
  int cnt=0;

  rprm=RadarParmMake();
  rawacf=RawMake();

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
  OptionAdd(&opt,"t",'f',&thr);
  OptionAdd(&opt,"vb",'x',&vb);

  arg=OptionProcess(1,argc,argv,&opt,NULL);


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (arg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  rawfp=OldRawOpen(argv[arg],NULL);

  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }


  while ((s=OldRawRead(rawfp,rprm,rawacf)) !=-1) {

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
    if (site==NULL) {
        fprintf(stderr,"ERROR! Something went wrong with getting the time from the site.\n");
        if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",rprm->time.yr,
                        rprm->time.mo,rprm->time.dy,rprm->time.hr,rprm->time.mt,
                        rprm->time.sc);
        continue;
    }

    /* calculate beam azimuth */
    offset=site->maxbeam/2.0-0.5;
    rprm->bmazm=site->boresite+site->bmsep*(rprm->bmnum-offset);
    if (thr !=-1) rawacf->thr=thr;
    s=RawFwrite(stdout,rprm,rawacf);
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



