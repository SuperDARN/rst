/* FitACFEnd.c
   ===========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "fitacf.h"
#include "rawread.h"
#include "fitwrite.h"

struct RadarParm prm;
struct RawData raw;
struct FitData fit;
struct FitBlock fblk;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;


int main(int argc,char *argv[]) {
 
  FILE *fp;
  char *envstr;
  int s;
 
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

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  s=RawFread(fp,&prm,&raw);

  if (s==-1) {
     fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }
    
  radar=RadarGetRadar(network,prm.stid);
  if (radar==NULL) {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
		          prm.time.dy,prm.time.hr,prm.time.mt,
                          prm.time.sc);

  if (site==NULL) {
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }

  FitACFStart(site,prm.time.yr,&fblk); 

  do {
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);

    FitACF(&prm,&raw,&fblk,&fit);
    s=FitFwrite(stdout,&prm,&fit);
    s=RawFread(fp,&prm,&raw);


  } while (s !=-1);
 
  FitACFEnd(&fblk);

  fclose(fp);


  return 0;
}
