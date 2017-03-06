/* map_fit.c 
   ========= 
   Author: R.J.Barnes and others
*/

/*
 LICENSE AND DISCLAIMER
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

  int old=0;
 
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

  /* function pointers for file reading/writing (old and new) and MLT */
  int (*Map_Read)(FILE *, struct CnvMapData *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);

  grd=GridMake();
  map=CnvMapMake();


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"ew",'t',&ewstr);  /* error weight */
  OptionAdd(&opt,"mw",'t',&mwstr);  /* model weight */
 
  OptionAdd(&opt,"s",'t',&source);
  OptionAdd(&opt,"major",'i',&major);
  OptionAdd(&opt,"minor",'i',&minor);

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

  while (Map_Read(fp,map,grd) !=-1) {
          
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
    Map_Write(stdout,map,grd);
    if (vb==1) 
      fprintf(stderr,
              "%d-%d-%d %d:%d:%d dp=%g error=%g chi_sqr=%g rms_err=%g\n",
               yr,mo,dy,hr,mt,(int) sc, map->pot_drop/1000,
               map->pot_drop_err/1000,map->chi_sqr,map->rms_err);
  }

  return 0;
}

