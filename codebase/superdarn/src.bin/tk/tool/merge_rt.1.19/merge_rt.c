/* merge_rt.c
   ==========
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "rtypes.h"
#include "option.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "errstr.h"
#include "hlpstr.h"
#include "log_info.h"

#define MAXFILE 80


int fnum=0;
char *inname[MAXFILE];

char *outname=NULL;
char *doutname="rt.grdmap";


char pidname[256]={"pid.id"};

int dflg[MAXFILE];
struct GridData *in_rcd[MAXFILE]; 
struct GridData *rcd;

int tin=0;
double st_tme[MAXFILE];
double ed_tme[MAXFILE];
int tcnt[MAXFILE];
char logname[256];
char logbuf[256];

struct OptionData opt;

int make_grid(struct GridData *out,
              struct GridData **in,int fnum,int *dflg) {

  int i,j,k=0,l=0;
  int tpnt=0;
  struct GridGVec *data[MAXFILE];

  out->stnum=0;
  out->vcnum=0;
  out->xtd=0;

  l=0;
  for (i=0;i<fnum;i++) 
    if ( (dflg[i] !=0) &&
         (in[i]->st_time<out->ed_time) &&
         (in[i]->ed_time>out->st_time)) {

    if (in[i]->xtd) out->xtd=1;
    for (j=0;j<in[i]->stnum;j++) {

      if (out->sdata !=NULL) out->sdata=realloc(out->sdata,
                                    sizeof(struct GridSVec)*(l+1));
      else out->sdata=malloc(sizeof(struct GridSVec));

      out->sdata[l].st_id=in[i]->sdata[j].st_id;
      out->sdata[l].chn=in[i]->sdata[j].chn;
      out->sdata[l].npnt=in[i]->sdata[j].npnt;
      out->sdata[l].freq0=in[i]->sdata[j].freq0;
      out->sdata[l].major_revision=in[i]->sdata[j].major_revision;
      out->sdata[l].minor_revision=in[i]->sdata[j].minor_revision;
      out->sdata[l].prog_id=in[i]->sdata[j].prog_id;
      out->sdata[l].noise.mean=in[i]->sdata[j].noise.mean;
      out->sdata[l].noise.sd=in[i]->sdata[j].noise.sd;
      out->sdata[l].gsct=in[i]->sdata[j].gsct;
      out->sdata[l].vel.min=in[i]->sdata[j].vel.min;
      out->sdata[l].vel.max=in[i]->sdata[j].vel.max;
      out->sdata[l].pwr.min=in[i]->sdata[j].pwr.min;
      out->sdata[l].pwr.max=in[i]->sdata[j].pwr.max;
      out->sdata[l].wdt.min=in[i]->sdata[j].wdt.min;
      out->sdata[l].wdt.max=in[i]->sdata[j].wdt.max;
      out->sdata[l].verr.min=in[i]->sdata[j].verr.min;
      out->sdata[l].verr.max=in[i]->sdata[j].verr.max;

      data[l]=in[i]->data;
      l++;
    }
  }

  tpnt=0;
  for (i=0;i<l;i++) tpnt+=out->sdata[i].npnt;
  out->stnum=l;
  out->vcnum=tpnt;
  if (out->data !=NULL)
    out->data=realloc(out->data,sizeof(struct GridGVec)*tpnt);
  else out->data=malloc(sizeof(struct GridGVec)*tpnt);

  k=0;
  for (i=0;i<l;i++) {
    memcpy(&out->data[k],data[i],sizeof(struct GridGVec)*out->sdata[i].npnt);
    k+=out->sdata[i].npnt;
  }

  return l;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: merge_rt --help\n");
  return(-1);
}
    
int main(int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  pid_t pid; 

  char *logstr=NULL;
  char *pidstr=NULL;
  char *flist=NULL;
 

  int i,k,t;
  time_t stme;
  /*struct tm *gtme;*/
  time_t utc;
  int tmax,tval;
  double st_max;
  int offset=5;
  int repeat=30;

  char line[256];

  FILE *fp;
  int fd; 
  int status=0;
  int mxflg=0;
  
  struct flock rflock;
  struct flock wflock;

  rcd=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"i",'i',&repeat);
  OptionAdd(&opt,"o",'i',&offset);
  OptionAdd(&opt,"r",'x',&mxflg);
  OptionAdd(&opt,"f",'t',&outname);
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"if",'t',&pidstr);
  OptionAdd(&opt,"fl",'t',&flist);

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

  if (flist==NULL) {

    if (argc-arg<2) {
      OptionPrintInfo(stdout,errstr);
      exit(-1);
    }

    for (i=arg;i<argc;i++) {
      inname[fnum]=argv[i];
      in_rcd[fnum]=GridMake();
      fnum++;
    }
  } else {
    fp=fopen(flist,"r");
    if (fp==NULL) {
      fprintf(stderr,"Could not open file list.\n");
      exit(1);
    }
    while(fgets(line,255,fp) !=NULL) {
      for (i=0;(isspace(line[i])) && (line[i] !=0);i++);
      if (line[i]==0) continue;
      if (line[i]=='#') continue;
      for (k=i;line[k] !='\n';k++);
      line[k]=0;
      inname[fnum]=malloc(k-i+1);
      strcpy(inname[fnum],line+i);
      in_rcd[fnum]=GridMake();
      fnum++;
    }
    fclose(fp);
  }


  mxflg=!mxflg;
  if (logstr !=NULL) strcpy(logname,logstr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);


   if (outname==NULL) outname=doutname;


  rflock.l_type=F_RDLCK;
  rflock.l_whence=SEEK_SET;
  rflock.l_start=0;
  rflock.l_len=0;

  wflock.l_type=F_WRLCK;
  wflock.l_whence=SEEK_SET;
  wflock.l_start=0;
  wflock.l_len=0;

  if (fnum==0) {
    fprintf(stderr,"No files specified.\n");
    exit(-1);
  }

  sprintf(logbuf,"Merging files:");
  log_info(logname,logbuf);
  for (i=0;i<fnum;i++) {
    sprintf(logbuf,"%s",inname[i]);
    log_info(logname,logbuf);
  }
  sprintf(logbuf,"Output file:%s",outname);
  log_info(logname,logbuf);
  sprintf(logbuf,"Interval length:%d",repeat);
  log_info(logname,logbuf);
  sprintf(logbuf,"Interval offset:%d",offset); 
  log_info(logname,logbuf);

  fp=fopen(pidname,"w");
  pid=getpid();
  sprintf(logbuf,"Process ID recorded in file %s.",pidname);
  log_info(logname,logbuf);
  sprintf(logbuf,"PID %d.",(int) pid);
  log_info(logname,logbuf);
  fprintf(fp,"%d\n",(int) pid);
  fclose(fp);

  /* normalize on a minute boundary */

  if (repeat<1) repeat=1;
  if (offset>repeat) offset=0;
  if (offset<0) offset=0;

  stme=time(NULL);
  stme=stme-(stme % repeat)+offset;
 
  while(1) {
    do {
      utc=time(NULL);
      /*gtme=gmtime(&utc);*/
      utc=utc-stme;
      sleep(1);
    } while (utc<repeat);    
    stme=time(NULL);
    tin=0;
    for (i=0;i<fnum;i++) {
      fp=fopen(inname[i],"r");
      if (fp==NULL) continue;
      fd=fileno(fp);
      status=fcntl(fd,F_SETLKW,&rflock);
      if (old) {
        if (status==0) status=OldGridFread(fp,in_rcd[i]);
      } else {
        if (status==0) status=GridFread(fp,in_rcd[i]);
      }
      if (status !=-1) {
        dflg[i]=1;
        for (t=0;(t<tin) && (st_tme[t] !=in_rcd[i]->st_time);t++);
        tcnt[t]++;
        if (t==tin) { 
          st_tme[t]=in_rcd[i]->st_time;
          ed_tme[t]=in_rcd[i]->ed_time;
          tin++;
        }
      } else dflg[i]=0;
      fclose(fp);
    }
    tmax=0;
    tval=0;

   
    if (mxflg==1) {
      for (t=0;t<tin;t++) if (tcnt[t]>tmax) {
        tmax=tcnt[t];
        tval=t;
      }
    }
   
    if ((mxflg==0) || (tmax==1)) {
      /* use the most recent record */
      st_max=-1;
      for (t=0;t<tin;t++) if (st_tme[t]>st_max) {
        st_max=st_tme[t];
        tval=t;
      }
    }
      
   
      
    for (t=0;t<tin;t++) tcnt[t]=0;

    rcd->st_time=st_tme[tval];
    rcd->ed_time=ed_tme[tval];
    
    make_grid(rcd,in_rcd,fnum,dflg);
    sprintf(logbuf,"Number of files %d, Number of points %d.",
	    rcd->stnum,rcd->vcnum);
    log_info(logname,logbuf);
    fp=fopen(outname,"w");
    if (fp !=NULL) {
      fd=fileno(fp);
      status=fcntl(fd,F_SETLKW,&wflock);
      if (old) {
        if (status==0) OldGridFwrite(fp,rcd);
      } else {
        if (status==0) GridFwrite(fp,rcd);
      }
      fclose(fp);
    }
  }
  return 0;
}









