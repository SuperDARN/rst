/* combine_snd.c
   =============
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
#include <math.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "dmap.h"
#include "snddata.h"
#include "sndread.h"
#include "sndwrite.h"

#include "errstr.h"
#include "hlpstr.h"


char *fname[64];
int fnum=4;

FILE *in_fp[64];

struct SndData *in_rcd[64];
double in_time[64];
int read_flg[64];
int dflg[64];

struct SndData *rcd;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: combine_snd --help\n");
  return(-1);
}

int read_set() {
  int c=0;
  int i;
  int status=0;
  
  for (i=0;i<fnum;i++) {
    if ((in_fp[i] !=NULL) && (read_flg[i]==1)) {
      status=SndFread(in_fp[i],in_rcd[i]);
      if (status !=-1) {
        in_time[i]=TimeYMDHMSToEpoch(in_rcd[i]->time.yr,in_rcd[i]->time.mo,
                                     in_rcd[i]->time.dy,in_rcd[i]->time.hr,
                                     in_rcd[i]->time.mt,
                                     in_rcd[i]->time.sc+in_rcd[i]->time.us/1.0e6);
        read_flg[i]=0;
        dflg[i]=1;
      } else {
        dflg[i]=0;
        fclose(in_fp[i]);
        in_fp[i]=NULL;
      }
    }
    if (in_fp[i] !=NULL) c++;
  }
  return c;
}


int main(int argc,char *argv[]) {

  int arg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  unsigned char vb=0;
 
  int record=0;
  int opfp=0;

  double st_time=0;

  int i,c=0,n=0;

  time_t ctime;
  char command[128];
  char tmstr[40];
 
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


  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }
  fnum=argc-arg;
 
  for (i=0;i<fnum;i++) {
    in_rcd[i]=SndMake();
    dflg[i]=0;
    in_fp[i]=fopen(argv[arg+i],"r");
    if (in_fp[i]==NULL) fprintf(stderr,"File %s Not Found.\n",fname[i]);
    else {
      read_flg[i]=1;
      c++;
    }
  }

  if (c==0) {
    fprintf(stderr,"No files to be processed.\n");
    exit(-1);
  }

  if (vb) {
    fprintf(stderr,"Processing %d files\n",c);
  }

  command[0]=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }

  while ((opfp=read_set()) !=0)  {

    /* Find the earliest sounding record */
    st_time=1e31;
    for (i=0;i<fnum;i++) {
      if (dflg[i] !=0) {
        if (in_time[i]<st_time) st_time=in_time[i];
      }
    }

    /* Write the next sounding record */
    for (i=0;i<fnum;i++) {
      if ( (dflg[i] !=0) && (in_time[i] == st_time) ) {

        in_rcd[i]->origin.code=1;
        ctime = time((time_t) 0);
        strcpy(tmstr,asctime(gmtime(&ctime)));
        tmstr[24]=0;
        SndSetOriginTime(in_rcd[i],tmstr);
        SndSetOriginCommand(in_rcd[i],command);

        SndFwrite(stdout,in_rcd[i]);

        if (vb) {
          int syr,smo,sdy,shr,smt,ssc;
          double sec;
          TimeEpochToYMDHMS(st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
          ssc=sec;

          fprintf(stderr,"%d-%02d-%02d %02d:%02d:%02d (Scan: %d, Beam: %02d)\n",
                  syr,smo,sdy,shr,smt,ssc,in_rcd[i]->scan,in_rcd[i]->bmnum);
        }

        /* discard old records */
        read_flg[i]=1;
        record++;
      }
    }

  }

  return 0;
}

