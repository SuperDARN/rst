/* inxdump.c
   =========
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
#include "hlpstr.h"



struct OptionData opt;

int main (int argc,char *argv[]) {

  FILE *fp;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char startstop=0;
  int st=0;
  double tme[2];
  int32 inx;
  int yr,mo,dy,hr,mt;
  double sc; 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"xtd",'x',&startstop);

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


  do {
    st=ConvertFreadDouble(fp,&tme[0]);
    if (st !=0) break;
    if (startstop) {
      st=ConvertFreadDouble(fp,&tme[1]);
      if (st !=0) break;
    }
    st=ConvertFreadInt(fp,&inx);
    if (st !=0) break;
    TimeEpochToYMDHMS(tme[0],&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %g ",yr,mo,dy,hr,mt,sc);
    if (startstop) {
      TimeEpochToYMDHMS(tme[1],&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %g ",yr,mo,dy,hr,mt,sc);
    }
    fprintf(stdout,"%d\n",inx);
  } while (1);
  return 0;
}
