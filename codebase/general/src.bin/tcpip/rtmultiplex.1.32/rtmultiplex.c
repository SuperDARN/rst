/* rtmultiplex.c
   =============
   Author: R.J.Barnes
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
#include "rtypes.h"
#include "option.h"
#include "connex.h"

#include "loginfo.h"
#include "errstr.h"
#include "hlpstr.h"
#include "pipe.h"
#include "fork.h"


#define MAXSERVER 64

int num=0;
char host[MAXSERVER][256];
int remoteport[MAXSERVER];
int sock[MAXSERVER];
time_t tick[MAXSERVER];

time_t pipewait=0;

unsigned  char *buffer[MAXSERVER];
int size[MAXSERVER];
int flag[MAXSERVER];

int outpipe=-1;
int resetflg=0;

char pname[256]={"port.id"};
char pidname[256]={"pid.id"};
char logfname[256]={"web.rt.log"};
char timefname[256]={"time.rt"};
unsigned char vb=0,vt=0;

struct OptionData opt;


void trappipe(int signal) {
  close(outpipe);
  outpipe=-1;
}


int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: rtmultiplex --help\n");
  return(-1);
}


int main(int argc,char *argv[]) {
  int arg;
  int c;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  char *logstr=NULL;
  char *timestr=NULL;
  char *pnamestr=NULL;
  char *pidstr=NULL;

  int port=0;
  int timeout=20;
  int resetclk=1;
  int status;
  char logbuf[256];

  int num=0;
  char host[MAXSERVER][256];
  int remoteport[MAXSERVER];
  int sock[MAXSERVER];

  sigset_t set;
  struct sigaction act;

  struct timeval tmout;
  int cnt=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'i',&timeout);
  OptionAdd(&opt,"r",'i',&resetclk);
  OptionAdd(&opt,"lp",'i',&port);
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
  if (timestr !=NULL) {
    strcpy(timefname,timestr);
    vt=1;
  }
  if (pnamestr !=NULL) strcpy(pname,pnamestr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);

  for (c=arg;c<argc;c+=2) {
    strcpy(host[num],argv[c]);
    remoteport[num]=atoi(argv[c+1]);
    sock[num]=-1;
    num++;
  }

  sigemptyset(&set);
  sigaddset(&set,SIGPIPE);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trappipe;
  sigaction(SIGPIPE,&act,NULL);

  signal(SIGCHLD,SIG_IGN);

  outpipe=forkinet(port);

  do {
    for (c=0;c<num;c++) {
      if (time(NULL) > tick[c]) {
        if (sock[c] !=-1) {
          sprintf(logbuf,"Connection to Host %s:%d timed out - connection reset",
                         host[c],remoteport[c]);
          loginfo(logfname,logbuf);
          close(sock[c]);
          sock[c]=-1;
        }
        sprintf(logbuf,"Connecting to Host %s:%d.",host[c],remoteport[c]);
        loginfo(logfname,logbuf);
        sprintf(logbuf,"Timeout: %d seconds.",timeout);
        loginfo(logfname,logbuf);
        sprintf(logbuf,"Reset Delay: %d seconds.",resetclk);
        loginfo(logfname,logbuf);
        tmout.tv_sec=timeout;
        tmout.tv_usec=0;
        sock[c]=ConnexOpen(host[c],remoteport[c],&tmout);
        if (sock[c]==-1) {
          sprintf(logbuf,"Could not connect to Host %s:%d.",
                         host[c],remoteport[c]);
          loginfo(logfname,logbuf);
        } else {
          sprintf(logbuf,"Connection to Host open.");
          loginfo(logfname,logbuf);
        }
        tick[c]=time(NULL)+timeout;
      }
    }

    cnt=0;
    for (c=0;c<num;c++) if (sock[c] !=-1) cnt++;
    if (cnt==0) {
      sleep(1);
      continue;
    }

    if (vb) {
      sprintf(logbuf,"Polling for incoming data. (%d open connections)",cnt);
      loginfo(logfname,logbuf);
    }

    tmout.tv_sec=timeout;
    tmout.tv_usec=0;

    status=ConnexRead(num,sock,buffer,size,flag,&tmout);
    if (status==0) {
      loginfo(logfname,"select() timed out - restarting connections.");
      for (c=0;c<num;c++) {
        close(sock[c]);
        sock[c]=-1;
        flag[c]=0;
        tick[c]=time(NULL)+resetclk;
      }
      continue;
    }

    if (status==-1) {
      loginfo(logfname,"select() failed - restarting connections.");
      for (c=0;c<num;c++) {
        close(sock[c]);
        sock[c]=-1;
        flag[c]=0;
        tick[c]=time(NULL)+resetclk;
      }
      continue;
    }

    for (c=0;c<num;c++) {
      if (flag[c]==0) continue;
      if (flag[c]==-1) {
        if (vb) {
          sprintf(logbuf,"Connection to Host %s:%d failed.",
                         host[c],remoteport[c]);
          loginfo(logfname,logbuf);
        }
        close(sock[c]);
        sock[c]=-1;
        flag[c]=0;
        tick[c]=time(NULL)+resetclk;
        continue;
      }
      if ((outpipe==-1) && (time(NULL) > pipewait)) {
        loginfo(logfname,"Child process died - Restarting.");
        outpipe=forkinet(port);
        if (outpipe==-1) {
          pipewait=time(NULL)+resetclk;
          continue;
        }
      }
      tick[c]=time(NULL)+timeout;
      if (vb) {
        sprintf(logbuf,"Received %d bytes from Host %s:%d.",
                       size[c],host[c],remoteport[c]);
        loginfo(logfname,logbuf);
      }
      if (outpipe==-1) continue;

      status=pipewrite(outpipe,buffer[c],size[c]);
      if (status==-1) {
        loginfo(logfname,"Pipe broken");
        close(outpipe);
        pipewait=time(NULL)+resetclk;
        outpipe=-1;
      }
    }
  } while (1);

  return 0;
}

