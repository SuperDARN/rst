/* map_fit.c 
   ========= 
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

#include "hlpstr.h"
#include "version.h"




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

  FILE *fp;   
  char *fname=NULL;

  int yr,mo,dy,hr,mt;
  double sc;
  char *source=NULL;

  char *ewstr=NULL;
  char *mwstr=NULL;

  int error_wt=-1;
  int model_wt=-1;
  int major=-1;
  int minor=-1;

  grd=GridMake();
  map=CnvMapMake();



  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"ew",'t',&ewstr);
  OptionAdd(&opt,"mw",'t',&mwstr);
 
  OptionAdd(&opt,"s",'t',&source);
  OptionAdd(&opt,"major",'i',&major);
  OptionAdd(&opt,"minor",'i',&minor);

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

  if (old) {
    while (OldCnvMapFread(fp,map,grd) !=-1) {
          
      TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (error_wt !=-1) map->error_wt=error_wt;
      if (model_wt !=-1) map->model_wt=model_wt;

      if (source !=NULL) strcpy(map->source,source);
      else strcpy(map->source,"map_fit");
      if (major !=-1) map->major_rev=major;
      else map->major_rev=atoi(MAJOR_VERSION);
      if (minor !=-1) map->minor_rev=minor;
      else map->minor_rev=atoi(MINOR_VERSION);

      CnvMapFitMap(map,grd);
      OldCnvMapFwrite(stdout,map,grd);
      if (vb==1) 
        fprintf(stderr,
              "%d-%d-%d %d:%d:%d dp=%g error=%g chi_sqr=%g rms_err=%g\n",
	      yr,mo,dy,hr,mt,(int) sc,
	      map->pot_drop/1000,
    	      map->pot_drop_err/1000,map->chi_sqr,map->rms_err);
    }
  } else {
     while (CnvMapFread(fp,map,grd) !=-1) {
          
      TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (error_wt !=-1) map->error_wt=error_wt;
      if (model_wt !=-1) map->model_wt=model_wt;

      if (source !=NULL) strcpy(map->source,source);
      else strcpy(map->source,"map_fit");
      if (major !=-1) map->major_rev=major;
      else map->major_rev=atoi(MAJOR_VERSION);
      if (minor !=-1) map->minor_rev=minor;
      else map->minor_rev=atoi(MINOR_VERSION);

      CnvMapFitMap(map,grd);
      CnvMapFwrite(stdout,map,grd);
      if (vb==1) 
        fprintf(stderr,
              "%d-%d-%d %d:%d:%d dp=%g error=%g chi_sqr=%g rms_err=%g\n",
	      yr,mo,dy,hr,mt,(int) sc,
	      map->pot_drop/1000,
    	      map->pot_drop_err/1000,map->chi_sqr,map->rms_err);
    }
  }
   
  return 0;
}






















