/* rtpoll.c
   ========
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



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "option.h"
#include "errstr.h"
#include "hlpstr.h"
#include "log_info.h"

#define OUTFNAME "tick.dat"

time_t tval=-1;
struct flock rflock;
char logfname[256];
char log_buf[256];

char pidname[256]={"pid.id"};


struct OptionData opt;

int main(int argc,char *argv[]) {
  FILE *fp;
  unsigned char help=0;  
  unsigned char option=0;
  
  pid_t pid;

  int arg,j;
  int dtime=5;
  int fildes,rc;
  int status;
  struct stat buf;
  struct flock rflock;
  
  char *logstr=NULL;
  char *outfnamestr=NULL;

  char *pidstr=NULL;


  char command[256];  

  char outfname[64]=OUTFNAME;
  char *infname=NULL;

  rflock.l_type=F_RDLCK;
  rflock.l_whence=SEEK_SET;
  rflock.l_start=0;
  rflock.l_len=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"d",'i',&dtime);
  OptionAdd(&opt,"if",'t',&pidstr);
  OptionAdd(&opt,"f",'t',&outfnamestr);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (argc==arg) {
     OptionPrintInfo(stdout,errstr);
     exit(-1);
  }

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (outfnamestr !=NULL) strcpy(outfname,outfnamestr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);


  infname=argv[arg];
  command[0]=0;
  for (j=arg+1;j<argc;j++) {
      strcat(command,argv[j]);
      strcat(command," ");
  }
  
  if (command[0] !=0) {
    sprintf(log_buf,"Command\t:%s",command);
    log_info(logfname,log_buf);
  }
  sprintf(log_buf,"Input file:%s",infname);

  log_info(logfname,log_buf);

  sprintf(log_buf,"Output file:%s",outfname);

  log_info(logfname,log_buf);
  sprintf(log_buf,"Poll Interval:%ds",dtime);
  log_info(logfname,log_buf);

  fp=fopen(pidname,"w");
  pid=getpid();
  sprintf(log_buf,"Process ID recorded in file %s.",pidname);
  log_info(logfname,log_buf);
  sprintf(log_buf,"PID %d.",(int) pid);
  log_info(logfname,log_buf);
  fprintf(fp,"%d\n",(int) pid);
  fclose(fp);

  while (1) {

     sleep(dtime);
     fildes=open(infname,O_RDONLY); 
     if (fildes==-1) continue;
     rflock.l_type=F_RDLCK;
     status=fcntl(fildes,F_SETLKW,&rflock);
     rc=fstat(fildes,&buf);
     rflock.l_type=F_UNLCK;  
     status=fcntl(fildes,F_SETLKW,&rflock);
     close(fildes);
     if (buf.st_mtime !=tval) {

       tval=buf.st_mtime; 
       log_info(logfname,"File changed.");
       if (command[0] !=0) {
         sprintf(log_buf,"Executing command:%s",command);
         log_info(logfname,log_buf);
         system(command);
       }
       fp=fopen(outfname,"w");
       fprintf(fp,"%ld\n",(long) tval);
       fclose(fp);
     }
  }
  return 0;
}

  




















