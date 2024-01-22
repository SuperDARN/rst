/* map_fit.c 
   ========= 
   Author: R.J.Barnes and others

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
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "fitmap.h"
#include "aacgmlib_v2.h"
#include "igrflib.h"

#include "hlpstr.h"
#include "version.h"

struct CnvMapData *map;
struct GridData *grd;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: map_fit --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;
  int old_aacgm=0;
  int ecdip=0;
 
  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  FILE *fp;   
  char *fname=NULL;

  int yr,mo,dy,hr,mt;
  double sc;
  char *source=NULL;
  int tme;
  int yrsec;
  int first;
  int noigrf=0;
  float decyear;

  char *ewstr=NULL;
  char *mwstr=NULL;

  int error_wt=-1;
  int model_wt=-1;
  int major=-1;
  int minor=-1;
  int order=0;

  int magflg=0;

  /* function pointers for file reading/writing (old and new) */
  int (*Map_Read)(FILE *, struct CnvMapData *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);

  grd=GridMake();
  map=CnvMapMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"ecdip",'x',&ecdip);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"ew",'t',&ewstr);  /* error weight */
  OptionAdd(&opt,"mw",'t',&mwstr);  /* model weight */
  OptionAdd(&opt,"o",'i',&order);

  OptionAdd(&opt,"s",'t',&source);
  OptionAdd(&opt,"major",'i',&major);
  OptionAdd(&opt,"minor",'i',&minor);

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

  if (arg !=argc) fname=argv[arg];

  if (ecdip) magflg = 2;
  else if (old_aacgm) magflg = 1;
  else magflg = 0;

  if (ewstr !=NULL) {
     if (tolower(ewstr[0])=='y') error_wt=1;
     else error_wt=0;
  }
   
  if (mwstr !=NULL) {
     if (tolower(mwstr[0])=='n') model_wt=1;
     else model_wt=0;
  }
   
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  /* set function pointer to read/write old or new */
  if (old) {
    Map_Read  = &OldCnvMapFread;
    Map_Write = &OldCnvMapFwrite;
  } else {
    Map_Read  = &CnvMapFread;
    Map_Write = &CnvMapFwrite;
  }

  first = 1;
  while ((*Map_Read)(fp,map,grd) !=-1) {

    tme = (map->st_time + map->ed_time)/2.0;
    TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int)sc);
    decyear = yr + (float)yrsec/TimeYMDHMSToYrsec(yr,12,31,23,59,59);

    noigrf = map->noigrf;

    if (first) {
      if (!noigrf || ecdip) IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      first = 0;
    }

    if (error_wt !=-1) map->error_wt=error_wt;
    if (model_wt !=-1) map->model_wt=model_wt;

    if (source !=NULL) strcpy(map->source,source);
    else strcpy(map->source,"map_fit");
    if (major !=-1) map->major_rev=major;
    else map->major_rev=atoi(MAJOR_VERSION);
    if (minor !=-1) map->minor_rev=minor;
    else map->minor_rev=atoi(MINOR_VERSION);

    if (order !=0) map->fit_order=order;

    CnvMapFitMap(map,grd,decyear,magflg);
    (*Map_Write)(stdout,map,grd);

    if (vb==1) {
      TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%d-%d-%d %d:%d:%d dp=%g error=%g chi_sqr=%g rms_err=%g\n",
                     yr,mo,dy,hr,mt,(int) sc, map->pot_drop/1000,
                     map->pot_drop_err/1000,map->chi_sqr,map->rms_err);
    }

  }

  return 0;
}

