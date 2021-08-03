/* rtfitacftofit.c
   ===============
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
#include "fitpacket.h"
#include "loginfo.h"

#include "errstr.h"
#include "hlpstr.h"
#include "fork.h"



int outpipe=-1;
int resetflg=0;
char pname[256]={"port.id"};
char pidname[245]={"pid.id"};


extern int loaddata(char *);

char logfname[256]={"log.rt"};
char timefname[256]={"time.rt"};

struct OptionData opt;

void trapsignal(int signal) {
  resetflg=2;
}

void trapalarm(int signal) {
  resetflg=1;
}

void trappipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: rtfitacftofit --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  char *logstr=NULL;
  char *timestr=NULL;
  char *pnamestr=NULL;
  char *pidstr=NULL;

  int sock;
  int port=0;
  int timeout=2*60;
  int flag,status;
  int remote_port=0;
  char logbuf[256];

  char host[256];

  char *port_fname=NULL;
  unsigned char port_flag=0;

  FILE *fp;

  struct RadarParm *prm;
  struct FitData *fit;
  unsigned char *buffer=NULL;
  int buf_len;

  sigset_t set;
  struct sigaction act;


#ifdef _POSIX 
  timer_t timer_id=-1;
  struct itimerspec timer;
  struct sigevent event;
#else
  struct itimerval timer;
#endif

  prm=RadarParmMake();
  fit=FitMake();

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"t",'i',&timeout);
  OptionAdd(&opt,"lp",'i',&port);
  OptionAdd(&opt,"rpf",'x',&port_flag);
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"T",'t',&timestr);
  OptionAdd(&opt,"pf",'t',&pnamestr);
  OptionAdd(&opt,"if",'t',&pidstr);
 
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

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (timestr !=NULL) strcpy(timefname,timestr);

  if (pnamestr !=NULL) strcpy(pname,pnamestr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);
 
  strcpy(host,argv[argc-2]);

  if (port_flag==0) remote_port=atoi(argv[argc-1]);
  else port_fname=argv[argc-1];
 
  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);
  sigaddset(&set,SIGUSR2);
  sigaddset(&set,SIGPIPE);
  sigaddset(&set,SIGALRM);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapsignal;
  sigaction(SIGUSR1,&act,NULL);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trappipe;
  sigaction(SIGPIPE,&act,NULL);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapalarm;
  sigaction(SIGUSR2,&act,NULL);

  signal(SIGCHLD,SIG_IGN); 
 
#ifdef _POSIX
  timer.it_interval.tv_sec=0L;
  timer.it_interval.tv_nsec=0L;
  timer.it_value.tv_sec=timeout;
  timer.it_value.tv_nsec=0;
  event.sigev_signo=SIGUSR2;

  if((timer_id=timer_create(CLOCK_REALTIME,&event))==-1) {
     fprintf(stderr,"Failed to create timer\n");
     exit(-1);
  }
#else
  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapalarm;
  sigaction(SIGALRM,&act,NULL);
  
  timer.it_value.tv_sec=timeout;
  timer.it_value.tv_usec=0;
#endif

  outpipe=forkinet(port);

 

  if (port_flag==0) 
    sprintf(logbuf,"Connecting to Host %s:%d\n",host,remote_port);
  else sprintf(logbuf,"Connecting to Host %s Port File %s\n",host,port_fname);
  loginfo(logfname,logbuf);
 
  do { 
    resetflg=0;

    if (port_flag==1) {
      fp=fopen(port_fname,"r");
      if (fp !=NULL) {
        fscanf(fp,"%d",&remote_port);
        fclose(fp);
      } else remote_port=1024;
    }

    sprintf(logbuf,"Connecting to host:%s %d",host,remote_port);

    loginfo(logfname,logbuf);

    sock=ConnexOpen(host,remote_port,NULL); 
    if (sock==-1) {
      loginfo(logfname,"Could not connect to host.");
      sleep(10); 
      continue;
    }
    do {
#ifdef _POSIX
    timer_settime(timer_id,0,&timer,NULL);
#else
    setitimer(ITIMER_REAL,&timer,NULL);
#endif
      status=FitCnxRead(1,&sock,prm,fit,&flag,NULL);
      if ((flag==-1) || (status==-1) || (resetflg !=0)) break;
      if (flag==0) continue;
      buffer=fitpacket(prm,fit,&buf_len);
      if (outpipe==-1) {
         loginfo(logfname,"Child process died - Restarting.");
         outpipe=forkinet(port);
      }
      if (outpipe !=-1) status=ConnexWriteIP(outpipe,(void *) buffer,buf_len);
      free(buffer);
    } while(1);
    loginfo(logfname,"Connection to host failed.");
    ConnexClose(sock);
    if (resetflg !=0) {
      if (resetflg==1) loginfo(logfname,"Alarm Triggered.");
      else loginfo(logfname,"User signal received.");
    }
  } while (1);
  sleep(10);

  return 0;
}
   

 






















