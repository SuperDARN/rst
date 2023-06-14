/* make_snd.c
   ==========
   Author: E.G.Thomas

Copyright (C) <year>  <name of author>

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

    if ((cpid !=-1) && (abs(prm->cp) !=abs(cpid))) continue;
    if ((channel !=-1) && (prm->channel !=channel)) continue;

    snd->origin.code=1;
    ctime = time((time_t) 0);
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    SndSetOriginTime(snd,tmstr);
    SndSetOriginCommand(snd,command);
    SndSetCombf(snd,prm->combf);

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

