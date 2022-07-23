/* rpos_v2_test.c
   ==============
   Author: E.G.Thomas

   Copyright (C) 2022  Evan G. Thomas

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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "rpos_v2.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;
struct OptionData opt;


double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}


int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: rpos_v2_test --help\n");
  return(-1);
}


int main (int argc,char *argv[]) {

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  FILE *fp;
  char *envstr;

  char *dtetxt=NULL;
  double dval=0;
  int yr,mo,dy,hr,mt,isc,usc;
  double sc;

  int stid=1;
  int bm=7;
  int rn=20;
  double height=300.0;
  int frang=180;
  int rsep=45;
  int rx=0;
  int model=0;
  int gs=0;
  int rear=0;
  double frho,flat,flon;
  double hop=0.5;

  int standard=0;
  int chisham=0;
  int cv=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"d",'t',&dtetxt);

  OptionAdd(&opt,"stid",'i',&stid);
  OptionAdd(&opt,"bm",'i',&bm);
  OptionAdd(&opt,"rn",'i',&rn);
  OptionAdd(&opt,"fh",'d',&height);
  OptionAdd(&opt,"hop",'d',&hop);

  OptionAdd(&opt,"standard",'x',&standard);
  OptionAdd(&opt,"chisham",'x',&chisham);
  OptionAdd(&opt,"cv",'x',&cv);

  OptionAdd(&opt,"gs",'x',&gs);
  OptionAdd(&opt,"rear",'x',&rear);

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

  if (dtetxt !=NULL) dval=strdate(dtetxt);

  if (dval !=0) {
    TimeEpochToYMDHMS(dval,&yr,&mo,&dy,&hr,&mt,&sc);
    isc=sc;
  } else TimeReadClock(&yr,&mo,&dy,&hr,&mt,&isc,&usc);

  RadarLoadHardware(envstr,network);
  radar=RadarGetRadar(network,stid);
  site=RadarYMDHMSGetSite(radar,yr,mo,dy,0,0,0);

  if (site==NULL) {
    fprintf(stderr,"Radar station ID (%d) and date (%4d%02d%02d) combination not valid.\n",stid,yr,mo,dy);
    exit(-1);
  }

  if (standard) model=0;
  else if (chisham) model=1;
  else if (cv) model=2;

  RPosGeo_v2(1,bm,rn,site,frang,rsep,rx,height,&frho,&flat,&flon,hop,model,gs,rear);

  fprintf(stderr,"flat: %lf, flon: %lf\n",flat,flon);

  exit(0);
}
