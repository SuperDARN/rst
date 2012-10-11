/* OldGridTableWrite.c
   ===================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
#include "filter.h"
#include "bound.h"
#include "gtable.h"
#include "oldgtablewrite.h"

struct RadarScan src[3];
struct RadarScan dst;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;


struct GridTable grid;
struct RadarParm prm;
struct FitData fit;

int main(int argc,char *argv[]) {
  char *envstr; 
  FILE *fp;
  int fid;
  int s;

  int state=0;

  int yr,mo,dy,hr,mt;
  double sc;

  int iflg=0;
  int avlen=120;
  unsigned char xtd=0;
  char wrtlog[256];

  int index=0,num=0;
  int mode=0,nbox=3;

  int mask=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

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

  fid=open("test.grdmap",O_WRONLY | O_TRUNC | O_CREAT,mask);
    
  while(FitFreadRadarScan(fp,&state,&src[index],&prm,&fit,avlen,0,0) !=-1) {
    TimeEpochToYMDHMS(src[index].st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

   

    if (num>2) {

      FilterRadarScan(mode,nbox,index,src,&dst,15);     
 
      if (site==NULL) {
         radar=RadarGetRadar(network,dst.stid);
         if (radar==NULL) {
           fprintf(stderr,"Failed to get radar information.\n");
           exit(-1);
         }

         site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
		          prm.time.dy,prm.time.hr,prm.time.mt,
                          prm.time.sc);
     
      }

      s=GridTableTest(&grid,&dst,avlen);
    
      if (s==1) {
      

        OldGridTableWrite(fid,&grid,wrtlog,xtd);
       
      }
      
      if (dst.num>=16) GridTableMap(&grid,&dst,site,avlen,iflg,300.0);  
    }
   
    index++;
    if (index>2) index=0;
    num++;

  
  }

  fclose(fp);
  close(fid);

  return 0;
}
