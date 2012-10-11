/* GridTableMap.c
   ==============
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "radar.h"
#include "fitread.h"
#include "scandata.h"
#include "fitscan.h"
#include "bound.h"
#include "gtable.h"
#include "gtablewrite.h"

struct RadarScan scn;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;


struct GridTable grid;
struct RadarParm prm;
struct FitData fit;

int main(int argc,char *argv[]) {
  char *envstr; 
  FILE *fp;

  int s;

  int state=0;

  int yr,mo,dy,hr,mt;
  double sc;

  int iflg=0;
  int avlen=120;
  unsigned char xtd=0;
  char wrtlog[256];

  int pval=0;
  double min[4]={35,3,10,0};
  double max[4]={2000,50,1000,200};
  int tflg=0;

  double alt=300.0;

  grid.st_time=-1;
  grid.status=0;
  grid.pnt=NULL;  
  grid.gsct=0;
  grid.chn=0;


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

  while(FitFreadRadarScan(fp,&state,&scn,&prm,&fit,0,0,0) !=-1) {
    TimeEpochToYMDHMS(scn.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

    FilterBoundType(&scn,tflg);
    FilterBound(pval,&scn,min,max);
 

    if (site==NULL) {
       radar=RadarGetRadar(network,scn.stid);
       if (radar==NULL) {
         fprintf(stderr,"Failed to get radar information.\n");
         exit(-1);
       }

       site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
		          prm.time.dy,prm.time.hr,prm.time.mt,
                          prm.time.sc);
     
    }

    s=GridTableTest(&grid,&scn,avlen);
    
    if (s==1) {
      GridTableFwrite(stdout,&grid,wrtlog,xtd);

    }
    
    if (scn.num>=16) GridTableMap(&grid,&scn,site,avlen,iflg,alt);     
  
  }


  
 
  fclose(fp);


  return 0;
}
