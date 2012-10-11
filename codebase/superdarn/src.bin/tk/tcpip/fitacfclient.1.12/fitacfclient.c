/* fitacfclient.c
   ==============
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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

int main(int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;


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

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
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
      int i;
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n", 
              prm->time.yr,prm->time.mo,prm->time.dy,
	      prm->time.hr,prm->time.mt,prm->time.sc);
      fprintf(stderr,"bmnum = %d\tbmazm = %f\tchannel = %d\tintt = %d\n",
	                        prm->bmnum,prm->bmazm,
                                prm->channel,prm->intt.sc);
      fprintf(stderr,"frang = %d\ttfreq = %d\n",prm->frang,
                                               prm->tfreq);
      fprintf(stderr,"rsep = %d\tnoise.search = %g\n",prm->rsep,
                                              prm->noise.search);
      fprintf(stderr,"noise.mean = %g\tscan = %d\n",
                      prm->noise.mean,prm->scan);
      fprintf(stderr,"cpid = %d\n",
                      prm->cp);

      fprintf(stderr,"origin.code=%d\n",prm->origin.code);

      if (prm->origin.time !=NULL) 
        fprintf(stderr,"origin.time=%s\n",prm->origin.time);
      if (prm->origin.command !=NULL) 
        fprintf(stderr,"origin.command=%s\n",prm->origin.command);
      
    
      if (fit->rng==NULL) continue;
  
      for (i=0;i<75; i++) {
        if (fit->rng[i].qflg !=0) fprintf(stderr,"d");
        else fprintf(stderr,"-");
      }
      fprintf(stderr,"\n");

      for (i=0;i<75; i++) {
        if (fit->rng[i].gsct !=0) fprintf(stderr,"g");
        else fprintf(stderr,"-");
      }
      fprintf(stderr,"\n");

      for (i=0;i<75; i++) {
        if (fit->rng[i].qflg !=0) fprintf(stderr,"%d:%g\t%g\t%g\t%g\n",i,
					 fit->rng[i].v,fit->rng[i].v_err,
                                         fit->rng[i].p_l,fit->rng[i].w_l);
      }
      
   }           
  } while(1);
  fprintf(stderr,"Connection failed.\n");

  return 0;
}
   

 










