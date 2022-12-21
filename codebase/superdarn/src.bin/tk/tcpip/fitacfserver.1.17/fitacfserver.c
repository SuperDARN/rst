/* fitacfserver.c
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


#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "fitindex.h"
#include "fitseek.h"

#include "radar.h"
#include "connex.h"
#include "loginfo.h"
#include "oldfitread.h"

#include "fitpacket.h"

#include "errstr.h"
#include "hlpstr.h"
#include "fork.h"

int outpipe=-1;
int resetflg=0;
char pname[256]={"port.id"};
char pidname[245]={"pid.id"};

struct OptionData opt;

char logfname[256];

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fitacfserver --help\n");
  return(-1);
}

void trap_pipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

void trap_signal(int signal) {
  resetflg=1;
}


void wait_boundary(float bnd) {
   struct timespec tm;
   int stime=0;
   int yr,mo,dy,hr,mt,sc,us;
   if (bnd==0) return;
   TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
   bnd=bnd*1000;
   stime=(mt*60+sc)*1000;
   stime=(int) bnd - (stime % (int) bnd);
   if (stime<0) return;

  tm.tv_sec=(int) stime/1000;
  tm.tv_nsec=(int) (stime % 1000) * 1e6;
  nanosleep(&tm,NULL);

}   

int main(int argc,char *argv[]) {

  unsigned char old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  char *logstr=NULL;
  char *pnamestr=NULL;
  char *pidstr=NULL;
  char *ststr=NULL;

 
  int port=0;
  

  int status;
  
  struct RadarParm *prm;
  struct FitData *fit;
  struct FitIndex *inx=NULL;

  unsigned char *buffer=NULL;
  int buflen;
  char *envstr;
  FILE *fp;
  FILE *ip;
 
  struct RadarNetwork *network;  
 
  float intt=7;
  float bnd=0;
  int st_id=-1;

  unsigned char aflg=0;
  unsigned char repflg=0;
  unsigned char sync=0;
  

  struct OldFitFp *fitfp=NULL;
  
  int yr,mo,dy,hr,mt,sc,us;

  sigset_t set;
  struct sigaction act;
  struct timespec tm;
 
  prm=RadarParmMake();
  fit=FitMake();


  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"lp",'i',&port);
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"pf",'t',&pnamestr);
  OptionAdd(&opt,"if",'t',&pidstr);
  OptionAdd(&opt,"b",'f',&bnd);
  OptionAdd(&opt,"s",'t',&ststr);
  OptionAdd(&opt,"i",'f',&intt);

  OptionAdd(&opt,"a",'x',&aflg);
  OptionAdd(&opt,"d",'x',&sync);
  OptionAdd(&opt,"r",'x',&repflg);

  OptionAdd(&opt,"old",'x',&old);

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


  if (argc==arg) {
     OptionPrintInfo(stdout,errstr);
     exit(-1);
  }

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (pnamestr !=NULL) strcpy(pname,pnamestr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);

  if (ststr !=NULL) st_id=RadarGetID(network,ststr);

  if (old) {
    if (argc-arg>1)  fitfp=OldFitOpen(argv[arg],argv[arg+1]);
    else fitfp=OldFitOpen(argv[arg],NULL);

    if (fitfp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    if (argc-arg>1) {
      ip=fopen(argv[arg+1],"r");
      if (ip==NULL) {
        fprintf(stderr,"Index not found.\n");
        exit(-1);
      }
      inx=FitIndexFload(ip);
      fclose(ip);
    }

  }

  signal(SIGCHLD,SIG_IGN); 

  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);
  sigaddset(&set,SIGPIPE);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_signal;
  sigaction(SIGUSR1,&act,NULL);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_pipe;
  sigaction(SIGPIPE,&act,NULL);

  outpipe=fork_inet(port);
  
  if (old) status=OldFitRead(fitfp,prm,fit);
  else status=FitFread(fp,prm,fit);

  if ((aflg==1) && (abs(prm->scan)==1)) wait_boundary(bnd);
  if (sync==1) {
    TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
    if (old) {
      status=OldFitSeek(fitfp,prm->time.yr,prm->time.mo,prm->time.dy,
                    hr,mt,sc,NULL);

      if (status==-1) { /* can't seek in the file so re-open */
        OldFitClose(fitfp);
        if ((argc-arg)>1)  fitfp=OldFitOpen(argv[arg],argv[arg+1]);
        else fitfp=OldFitOpen(argv[arg],NULL);  
      }
      status=OldFitRead(fitfp,prm,fit);
    } else {

       status=FitFseek(fp,prm->time.yr,prm->time.mo,prm->time.dy,
                      hr,mt,sc,NULL,inx);

       if (status==-1) { /* can't seek in the file so re-open */
	 fclose(fp);
         fp=fopen(argv[arg],"r");
       }
       status=FitFread(fp,prm,fit);
    }
  }



  if (aflg==1) {
    TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
    prm->time.yr=yr;
    prm->time.mo=mo;
    prm->time.dy=dy;
    prm->time.hr=hr;
    prm->time.mt=mt;
    prm->time.sc=sc;
    prm->time.us=us;
    prm->intt.sc=intt;
    prm->intt.us=0;
  }  
  if (st_id !=-1) prm->stid=st_id;
  do {

    /* read the fit file here */
    buffer=fitpacket(prm,fit,&buflen);
    if (buffer==NULL) {
      loginfo(logfname,"Failed to compress data - Stopping.");
      exit(-1);
    }

    if (outpipe==-1) {
      loginfo(logfname,"Child process died - Restarting.");
      outpipe=fork_inet(port);
    }

    if (outpipe !=-1) status=ConnexWriteIP(outpipe,buffer,buflen);
    free(buffer);

    tm.tv_sec=(int) intt;
    tm.tv_nsec=(int) ((intt-(int) intt) * 1e9);
    nanosleep(&tm,NULL);
 
    if (old) status=OldFitRead(fitfp,prm,fit);
    else status=FitFread(fp,prm,fit);

    if ((status==0) && (aflg==1)) {
      if (abs(prm->scan)==1) wait_boundary(bnd);
      TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
      prm->time.yr=yr;
      prm->time.mo=mo;
      prm->time.dy=dy;
      prm->time.hr=hr;
      prm->time.mt=mt;
      prm->time.sc=sc;
      prm->time.us=us;
      prm->intt.sc=intt;
      prm->intt.us=0;
    }  
    if (st_id !=-1) prm->stid=st_id;
  

    if (((resetflg==1) || (status !=0)) && (repflg==1)) {
      resetflg=0;
      if (old) {
        OldFitClose(fitfp);
        if ((argc-arg)>1)  fitfp=OldFitOpen(argv[arg],argv[arg+1]);
        else fitfp=OldFitOpen(argv[arg],NULL);
        status=OldFitRead(fitfp,prm,fit);
        
        if ((aflg==1) && (abs(prm->scan)==1)) wait_boundary(bnd);
        if (sync==1) {
          TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
          status=OldFitSeek(fitfp,prm->time.yr,prm->time.mo,prm->time.dy,
                          hr,mt,sc,NULL);
          if (status==-1) { /* can't seek in the file so re-open */
             OldFitClose(fitfp);
             if ((argc-arg)>1)  fitfp=OldFitOpen(argv[arg],argv[arg+1]);
             else fitfp=OldFitOpen(argv[arg],NULL);  
	  }
          status=FitFread(fp,prm,fit);
	}
      } else {
        fclose(fp);
        fp=fopen(argv[arg],"r");
        if ((aflg==1) && (abs(prm->scan)==1)) wait_boundary(bnd);
        if (sync==1) {
          TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
          status=FitFseek(fp,prm->time.yr,prm->time.mo,prm->time.dy,
                          hr,mt,sc,NULL,inx);
          if (status==-1) { /* can't seek in the file so re-open */
             fclose(fp);
             fp=fopen(argv[arg],"r"); 
	  }
          status=FitFread(fp,prm,fit);
	}
      }
      if (aflg==1) {
        TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
        prm->time.yr=yr;
        prm->time.mo=mo;
        prm->time.dy=dy;
        prm->time.hr=hr;
        prm->time.mt=mt;
        prm->time.sc=sc;
        prm->time.us=us;
        prm->intt.sc=intt;
        prm->intt.us=0;
      }
      status=0;
    }
 
  } while(status==0);
  return 0;
}

