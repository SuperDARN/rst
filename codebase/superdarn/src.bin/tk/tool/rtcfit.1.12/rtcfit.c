/* rtcfit.c
   ========
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
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "radar.h" 
#include "rmath.h"

#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"

#include "fitcfit.h"
#include "cfitread.h"
#include "cfitwrite.h"

#include "connex.h"
#include "fitcnx.h"

#include "loginfo.h"

#include "errstr.h"
#include "hlpstr.h"

int resetflg;


#define PATH "."
#define PIDFILE "cfit.rt.pid"
#define FNAME "rt.cfit"
#define LOGNAME "cfit.rt.log"

struct CFitdata *cfit;

struct OptionData opt;

extern int dotflag;
char logfname[256]={LOGNAME};



void trapsignal(int signal) {
  resetflg=2; 
}

double strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}   

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: rtcfit --help\n");
  return(-1);
}
 
int main(int argc,char *argv[]) {

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
 
  char *logstr=NULL;
  char *fnamestr=NULL;
  char *pathstr=NULL;
  char *pidstr=NULL;

  char *envstr;
  char *stcode=NULL;

  int sock;

  int remote_port=0;
  char dname[256];
  char logbuf[256];

  pid_t pid;
  FILE *fp;
  gzFile gzfp;
  char host[256];
  int flag,status;

  
  struct RadarParm *prm;
  struct FitData *fit;

  sigset_t set;
  struct sigaction act;

  struct timeval tv; 
  int reset=60;
  int cnt=0; 

  struct RadarNetwork *network=NULL;


  char path[256]={PATH};
  char pidfile[256]={PIDFILE};
  char fname[256]={FNAME};
  char tmpname[256];
  char *port_fname=NULL;
  unsigned char port_flag=0;
  double now=-1;
  double dstart=0;



  int yr,mo,dy,hr,mt;
  double sc;
 
  double minpwr=3.0;

  prm=RadarParmMake();
  fit=FitMake();
  cfit=CFitMake();

  pid=getpid();
 
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

  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"name",'t',&stcode);

  OptionAdd(&opt,"mp",'d',&minpwr);

  OptionAdd(&opt,"rpf",'x',&port_flag);
  
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"f",'t',&fnamestr);
  OptionAdd(&opt,"p",'t',&pathstr);
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
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (pathstr !=NULL) strcpy(path,pathstr);
  if (pidstr !=NULL) strcpy(pidfile,pidstr);

 
  strcpy(host,argv[argc-2]);
  if (port_flag==0) remote_port=atoi(argv[argc-1]);
  else port_fname=argv[argc-1];
 
  strcpy(tmpname,fname);
  strcat(tmpname,"~");

  if (port_flag==0) sprintf(logbuf,"Host:%s %d",host,remote_port);
  else sprintf(logbuf,"Host:%s Port File:%s",host,port_fname);
  loginfo(logfname,logbuf);

  sprintf(logbuf,"File path:%s",path);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"pid file:%s",pidfile);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"pid:%d",(int) pid);
  loginfo(logfname,logbuf);
 
  fp=fopen(pidfile,"w");
  fprintf(fp,"%d\n",pid);
  fclose(fp);
  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapsignal;
  sigaction(SIGUSR1,&act,NULL);

  now=0;

  do {
    resetflg=0;
  
    if (port_flag==1) {
      fp=fopen(port_fname,"r");
      if (fp !=NULL) {
        if (fscanf(fp,"%d",&remote_port) !=1) {
          fclose(fp);
          exit(-1);
        }
        fclose(fp);
      } else remote_port=1024;
    }

    sprintf(logbuf,"Connecting to host:%s %d",host,remote_port);

    loginfo(logfname,logbuf);

    sock=ConnexOpen(host,remote_port,NULL); 
    if (sock<0) {
      loginfo(logfname,"Could not connect to host - retrying.");
      sleep(10);
      continue;
    }
   
    resetflg=0;
    do {
     
       tv.tv_sec=reset; 

       status=FitCnxRead(1,&sock,prm,fit,&flag,&tv);

       if ((status==-1) || (flag==-1) || (resetflg !=0))  break;
       if (status==0) resetflg=1;
          
       if ((status==1) && (flag==1)) {
         fprintf(stderr,".");
         fflush(stderr);
         dotflag=1;
          

         FitToCFit(minpwr,cfit,prm,fit); 
	    
         now=cfit->time;

         if (dstart==0) {
           dstart=now-(int) now % (24*3600); /* start of day */
           TimeEpochToYMDHMS(dstart,&yr,&mo,&dy,&hr,&mt,&sc);
           if (stcode==NULL) stcode=RadarGetCode(network,cfit->stid,0);
           sprintf(dname,"%s/%.4d%.2d%.2d.%s.cfit",
		           path,yr,mo,dy,stcode);
	 }


         gzfp=gzopen(dname,"a");
         CFitWrite(gzfp,cfit);
         gzclose(gzfp);

         if ((now-dstart) >= 24*3600) { /* advance to the next day */
           dstart=now-(int) now % (24*3600); /* start of day */
           TimeEpochToYMDHMS(dstart,&yr,&mo,&dy,&hr,&mt,&sc);
           if (stcode==NULL) stcode=RadarGetCode(network,cfit->stid,0);
           sprintf(dname,"%s/%.4d%.2d%.2d.%s.cfit",
		           path,yr,mo,dy,stcode);
	 }
	 
         cnt++;
	    
       } 
  

     
    } while (1);
 
    if (resetflg==0) loginfo(logfname,"Connection failed.");
    if (resetflg==1) loginfo(logfname,"Connection timed out.");
    if (resetflg==2) loginfo(logfname,"Connection reset by signal.");
    ConnexClose(sock);
    sleep(5);
  } while(1);

  return 0;
}

