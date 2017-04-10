/* map_filter.c
   ============
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include "option.h"
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "hlpstr.h"



struct OptionData opt;
struct GridData *grd;
struct CnvMapData *map;
 
int skip=10*60; /* skip time */

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

  double tval=0,dval=0;
  int c=0;
 
  FILE *fp;
   
  int arg=0;
  unsigned char help=0;
  unsigned char option=0;

  grd=GridMake();
  map=CnvMapMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);

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



  if (arg !=argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(1);
    }
  } else fp=stdin;
  

  if (old) {  
    while (OldCnvMapFread(fp,map,grd) !=-1)  {
    
      if (c==0) {
        dval=map->st_time-((int) map->st_time % (24*3600));
        c=1;
      }
      if ((map->st_time-dval)>=tval) {
        OldCnvMapFwrite(stdout,map,grd);
        tval+=skip;
      }
    }
  } else {
    while (CnvMapFread(fp,map,grd) !=-1)  {
    
      if (c==0) {
       dval=map->st_time-((int) map->st_time % (24*3600));
       c=1;
      }
      if ((map->st_time-dval)>=tval) {
        CnvMapFwrite(stdout,map,grd);
        tval+=skip;
      }
    }
  }
  if (fp !=stdin) fclose(fp); 
 
  return 0;
}







