/* fitacfclient.c
   ==============
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "fitcnx.h"

#include "errstr.h"
#include "hlpstr.h"



struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fitacfclient --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int i,arg;
  int nrng=75;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;


  int sock;
  int remote_port=0;
  char host[256];
  int flag,status;
  struct RadarParm *prm;
  struct FitData *fit;
 
  prm=RadarParmMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"nrange",'i',&nrng);

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
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

  strcpy(host,argv[argc-2]);
  remote_port=atoi(argv[argc-1]);

  sock=ConnexOpen(host,remote_port,NULL); 

  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
 
  do {

   status=FitCnxRead(1,&sock,prm,fit,&flag,NULL);

   if (status==-1) break;
   if (flag !=-1) {
      fprintf(stderr,"%04d-%02d-%02d %02d:%02d:%02d\n",
                      prm->time.yr,prm->time.mo,prm->time.dy,
                      prm->time.hr,prm->time.mt,prm->time.sc);
      fprintf(stderr,"stid  = %3d\n", prm->stid);
      fprintf(stderr,"bmnum = %3d  bmazm = %.2f  channel = %2d  intt = %3.1f\n",
                      prm->bmnum,prm->bmazm,prm->channel,prm->intt.sc+prm->intt.us/1.0e6);
      fprintf(stderr,"frang = %3d  nave = %d  tfreq = %d\n", prm->frang,prm->nave,prm->tfreq);
      fprintf(stderr,"rsep  = %3d  noise.search = %g\n", prm->rsep,prm->noise.search);
      fprintf(stderr,"scan  = %3d  noise.mean   = %g\n", prm->scan,prm->noise.mean);
      fprintf(stderr,"cpid  = %d\n", prm->cp);
      fprintf(stderr,"origin.code = %d\n", prm->origin.code);

      if (prm->origin.time != NULL)
        fprintf(stderr,"origin.time = %s\n",prm->origin.time);
      if (prm->origin.command !=NULL) 
        fprintf(stderr,"origin.command = %s\n",prm->origin.command);

      fprintf(stderr,"fitacf.revision = %d.%d\n",fit->revision.major,fit->revision.minor);

      if (fit->rng==NULL) continue;

      fprintf(stderr,"\n");
      for (i=0;i<nrng; i++) {
        if (fit->rng[i].qflg !=0)
          if (fit->rng[i].gsct != 0) fprintf(stderr,"g");
          else                       fprintf(stderr,"i");
        else                         fprintf(stderr,"-");
      }
      fprintf(stderr,"\n\n");

      fprintf(stderr, "rng    vel      v_e     pow    wid\n");
      fprintf(stderr, "gte   (m/s)    (m/s)   (dB)   (m/s)\n");
      for (i=0;i<nrng; i++) {
        if (fit->rng[i].qflg != 0)
          fprintf(stderr,"%3d:%7.1f %7.1f %7.1f %7.1f\n", i+1, fit->rng[i].v,
                          fit->rng[i].v_err, fit->rng[i].p_l, fit->rng[i].w_l);
      }
      fprintf(stderr,"\n");
   }           
  } while(1);

  fprintf(stderr,"Connection failed.\n");

  return 0;
}

