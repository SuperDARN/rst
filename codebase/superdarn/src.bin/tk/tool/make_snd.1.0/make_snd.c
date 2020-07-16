/* make_snd.c
   ==========
   Author: E.G.Thomas
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
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

  int cpid=-1;
  int channel=-1;
  char *cpstr=NULL;
  char *chnstr=NULL;

  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];

  int cnt=0;

  prm=RadarParmMake();
  fit=FitMake();
  snd=SndMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"cp",'t',&cpstr);
  OptionAdd(&opt,"cn",'t',&chnstr);

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

  if (cpstr !=NULL) cpid=atoi(cpstr);

  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }

  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }

  while ((s=FitFread(fp,prm,fit)) !=-1) {

    if ((cpid !=-1) && (prm->cp !=cpid)) continue;
    if ((channel !=-1) && (prm->channel !=channel)) continue;

    snd->origin.code=1;
    ctime = time((time_t) 0);
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    SndSetOriginTime(snd,tmstr);
    SndSetOriginCommand(snd,command);

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

