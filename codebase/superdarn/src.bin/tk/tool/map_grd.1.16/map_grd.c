 /* map_grd.c
   ========== 
   Author: R.J.Barnes and others
 */

/*
 * LICENSE AND DISCLAIMER
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rmath.h"

#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "oldcnvmapwrite.h"
#include "cnvmapwrite.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"
#include "mlt.h"
#include "mlt_v2.h"

#include "radar.h" 

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

#include "hlpstr.h"

struct CnvMapData *map;
struct GridData *grd;

struct OptionData opt;

int main(int argc,char *argv[]) {

  int old=0;
  int old_aacgm=0;

  int i, arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  char *envstr;
  FILE *fp;

  char *fname=NULL;
  int tme;
  int yrsec;
  int cnt=0;
  float latmin=60;
  float latshft=0;
  unsigned char sh=0;

  int yr,mo,dy,hr,mt;
  double sc;

  /* function pointers for file reading/writing (old and new) and MLT */
  int (*Grid_Read)(FILE *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);
  double (*MLTCnv)(int, int, double);

  grd=GridMake();
  map=CnvMapMake();

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

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"sh",'x',&sh);
  OptionAdd(&opt,"l",'f',&latmin);
  OptionAdd(&opt,"s",'f',&latshft);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);
  
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }  

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  } 
  
  if (arg !=argc) fname=argv[arg];
 
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"Grid file not found.\n");
    exit(-1);
  }

  memset(map,0,sizeof(struct CnvMapData));  
  map->fit_order=4;
  map->doping_level=1;
  map->model_wt=1;
  map->error_wt=1;
  map->hemisphere=1;
  map->lat_shft=latshft;

  /* set function pointer to read/write old or new */
  if (old) {
    Grid_Read = &OldGridFread;
    Map_Write = &OldCnvMapFwrite;
  } else {
    Grid_Read = &GridFread;
    Map_Write = &CnvMapFwrite;
  }

  /* set function pointer to compute MLT or MLT_v2 */
  if (old_aacgm) MLTCnv = &MLTConvertYrsec;
  else           MLTCnv = &MLTConvertYrsec_v2;

  if (sh==1) map->hemisphere=-1;

  while (Grid_Read(fp,grd) !=-1) {
    TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    if (cnt==0) {
      for (i=0;i<grd->stnum;i++) {
        radar=RadarGetRadar(network,grd->sdata[i].st_id);
        if (radar!=NULL) 
          site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
        if ((site !=NULL) && (site->geolat<0)) {
          map->hemisphere=-1;
          break;
        }
      }
    } 

    yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
    map->mlt.start=MLTCnv(yr,yrsec,0.0);

    TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
    map->mlt.end=MLTCnv(yr,yrsec,0.0);

    tme=(grd->st_time+grd->ed_time)/2.0;
    TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
    map->mlt.av=MLTCnv(yr,yrsec,0.0);
 
    if (latshft !=0) {
      map->lon_shft=(map->mlt.av-12)*15.0;
      map->latmin-=latshft;
    }

    if (map->hemisphere==1) map->latmin=latmin;  
    else map->latmin=-latmin;

    map->st_time=grd->st_time;
    map->ed_time=grd->ed_time;

    Map_Write(stdout,map,grd);
    TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (vb==1) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy, hr,mt,(int) sc);  

    cnt++;
  }

  return 0;
}

