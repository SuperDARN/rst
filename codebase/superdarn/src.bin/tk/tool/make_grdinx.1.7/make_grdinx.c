/* make_grdinx.c
   =============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "rtime.h"
#include "griddata.h"

#include "gridread.h"
#include "gridwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;
struct GridData *grd;


int main (int argc,char *argv[]) {


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
  
  int sze=0;
  int yr,mo,dy,hr,mt;
  double sc; 

  grd=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

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



  if (old) {
    fprintf(stderr,
    "Index files for old format grid files cannot be created.\n");
    exit(-1);
  }

  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
    
  while (GridFread(fp,grd) !=-1) {
    if (vb) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d\n",
              yr,mo,dy,hr,mt,(int) sc,grd->vcnum);
    }
    ConvertFwriteDouble(stdout,grd->st_time);
    ConvertFwriteInt(stdout,sze);
    sze+=GridWrite(-1,grd);
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 






















