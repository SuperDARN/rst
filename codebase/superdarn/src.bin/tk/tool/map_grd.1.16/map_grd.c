 /* map_grd.c
   ========== 
   Author: R.J.Barnes
 */

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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
#include "mlt.h"

#include "radar.h" 




struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;


#include "hlpstr.h"

struct CnvMapData *map;
struct GridData *grd;

struct OptionData opt;

int main(int argc,char *argv[]) {

  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */

  int old=0;
  int new=0;

  int arg;
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

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"sh",'x',&sh);
  OptionAdd(&opt,"l",'f',&latmin);
  OptionAdd(&opt,"s",'f',&latshft);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);
  
  old=!new;

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
  
  if (sh==1) map->hemisphere=-1;
  if (old) {
    while (OldGridFread(fp,grd) !=-1) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (cnt==0) {
        int i;
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
      map->mlt.start=MLTConvertYrsec(yr,yrsec,0.0);

      TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map->mlt.end=MLTConvertYrsec(yr,yrsec,0.0);

      tme=(grd->st_time+grd->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map->mlt.av=MLTConvertYrsec(yr,yrsec,0.0);
 
      if (latshft !=0) {
        map->lon_shft=(map->mlt.av-12)*15.0;
        map->latmin-=latshft;
      }

      if (map->hemisphere==1) map->latmin=latmin;  
      else map->latmin=-latmin;

      map->st_time=grd->st_time;
      map->ed_time=grd->ed_time;

      OldCnvMapFwrite(stdout,map,grd);
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,
	      hr,mt,(int) sc);  
      cnt++;
    }
  } else {
    while (GridFread(fp,grd) !=-1) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (cnt==0) {
        int i;
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
      map->mlt.start=MLTConvertYrsec(yr,yrsec,0.0);

      TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map->mlt.end=MLTConvertYrsec(yr,yrsec,0.0);

      tme=(grd->st_time+grd->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map->mlt.av=MLTConvertYrsec(yr,yrsec,0.0);
 
      if (latshft !=0) {
        map->lon_shft=(map->mlt.av-12)*15.0;
        map->latmin-=latshft;
      }

      if (map->hemisphere==1) map->latmin=latmin;  
      else map->latmin=-latmin;

      map->st_time=grd->st_time;
      map->ed_time=grd->ed_time;

      CnvMapFwrite(stdout,map,grd);
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,
	      hr,mt,(int) sc);  
      cnt++;
    }
  }


  return 0;
}






















