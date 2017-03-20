/* gridtogrdmap.c
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
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"

#include "oldgridread.h"
#include "gridwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;
struct GridData *grd;


int main (int argc,char *argv[]) {

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;

  FILE *fp;
  
  int yr,mo,dy,hr,mt;
  double sc; 
  int s;

  grd=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
    
  while (OldGridFread(fp,grd) !=-1) {
   if (vb) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d\n",
              yr,mo,dy,hr,mt,(int) sc,grd->vcnum);
    }
    s=GridFwrite(stdout,grd);
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 






















