/* make_snd.c
   ==========
   Author: E.G.Thomas
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "snddata.h"
#include "fitsnd.h"
#include "sndwrite.h"

#include "hlpstr.h"


struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_snd --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  int arg=0;
  int s;
  struct RadarParm *prm;
  struct FitData *fit;
  struct SndData *snd;
  FILE *fp; 

  int cnt=0;

  prm=RadarParmMake();
  fit=FitMake();
  snd=SndMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"vb",'x',&vb);

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

  if (arg !=argc) { 
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;


  while ((s=FitFread(fp,prm,fit)) !=-1) {

    s=FitToSnd(snd,prm,fit,prm->scan);
    if (s==-1) {
      cnt=-1;
      break;
    }

    s=SndFwrite(stdout,snd);
    if (s==-1) {
      cnt=-1;
      break;
    }

    if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",prm->time.yr,
                    prm->time.mo,prm->time.dy,prm->time.hr,prm->time.mt,
                    prm->time.sc);

    cnt++;
  }

  SndFree(snd);

  if (cnt==-1) exit(EXIT_FAILURE);
  else exit(EXIT_SUCCESS);

  return 0;
}

