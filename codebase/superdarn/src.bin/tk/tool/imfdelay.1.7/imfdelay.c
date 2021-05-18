/* imfdelay.c
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
#include <dirent.h>
#include <math.h>

#include "rtypes.h"
#include "rtime.h"
#include "option.h"

#include "cdf.h"
#include "rcdf.h"

#include "locate.h"

#include "istp.h"
#include "wind.h"
#include "ace.h"

#include "errstr.h"
#include "hlpstr.h"
  
struct imfdata imf;
struct posdata pos;
struct plasmadata plasma;

char dpath[256]={"/data"};
 
double stime=-1;
double etime=-1;

double fdelay=54*60;

struct file_list *fptr;

struct OptionData opt;
struct OptionFile *optf=NULL;

struct swdata {
  double time;
  float px,py,pz;
  float bx,by,bz;
  float vx,vy,vz;
};

int swcnt=0;
struct swdata *swdata;

double *tvalue=NULL;
float *bfield=NULL;

int swcompar(const void *a,const void *b) {
  struct swdata *x;
  struct swdata *y;

  x=(struct swdata *) a;
  y=(struct swdata *) b;

  if (x->time<y->time) return -1;
  if (x->time>y->time) return 1;
  return 0;
}


int findvalue(int inx,int cnt,double *time,float *data,double tval,float *val) {
  int i;
  double stime,etime,v;
  int sinx,einx;

  val[0]=FILL_VALUE;
  val[1]=FILL_VALUE;
  val[2]=FILL_VALUE;

  for (i=0;(i<cnt) && (time[i]<=tval);i++);

  einx=i;
  while ((einx<cnt) && (fabs(data[3*einx])>fabs(FILL_VALUE/2))) einx++;
  sinx=i-1;
  while ((sinx>=0) && (fabs(data[3*sinx])> fabs(FILL_VALUE/2))) sinx--;
 
  if (sinx<0) sinx=0;
  if (einx>=cnt) einx=cnt-1;

  etime=time[einx];
  stime=time[sinx];
  if (tval<stime) return -1;
  if (tval>etime) return -1;

  if (einx !=sinx) {  
    v=(tval-stime)/(etime-stime);
  } else v=0;

  if ((fabs(data[3*sinx]) < fabs(FILL_VALUE/2)) &&
      (fabs(data[3*einx]) < fabs(FILL_VALUE/2))) {
    val[0]=data[3*sinx]*(1-v)+data[3*einx]*v;
    val[1]=data[3*sinx+1]*(1-v)+data[3*einx+1]*v;
    val[2]=data[3*sinx+2]*(1-v)+data[3*einx+2]*v;
  } else if (fabs(data[3*sinx]) < fabs(FILL_VALUE/2)) {
    val[0]=data[3*sinx];
    val[1]=data[3*sinx+1];
    val[2]=data[3*sinx+2];
  }  else if (fabs(data[3*einx]) < fabs(FILL_VALUE/2)) {
    val[0]=data[3*einx];
    val[1]=data[3*einx+1];
    val[2]=data[3*einx+2];
  }
  return sinx;
}



double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
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


int load_wind() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;

  sprintf(path,"%s/%s",dpath,"wind");

  fptr=locate_files(path,"swe",stime,etime);

  for (i=0;i<fptr->cnt;i++) {
    fprintf(stderr,"%s\n",fptr->fname[i]);

    status=CDFopen(fptr->fname[i],&id);
    if (status !=CDF_OK) {
      fprintf(stderr,"Could not open cdf file.\n");
      continue;
    }
   
    status=windswe_pos(id,&pos,stime,etime); 
    status=windswe_plasma(id,&plasma,stime,etime);
    CDFclose(id);
  }
  free_locate(fptr);

  fptr=locate_files(path,"mfi",stime,etime);

  for (i=0;i<fptr->cnt;i++) {
    fprintf(stderr,"%s\n",fptr->fname[i]);

    status=CDFopen(fptr->fname[i],&id);
    if (status !=CDF_OK) {
      fprintf(stderr,"Could not open cdf file.\n");
      continue;
    }    
    status=windmfi_imf(id,&imf,stime,etime);
    CDFclose(id);
  }
  free_locate(fptr);
 
  return 0;
}
 
int load_ace() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;

  sprintf(path,"%s/%s",dpath,"ace"); 
  fptr=locate_files(path,"h0_swe",stime,etime);

  for (i=0;i<fptr->cnt;i++) {
    fprintf(stderr,"%s\n",fptr->fname[i]);

    status=CDFopen(fptr->fname[i],&id);
    if (status !=CDF_OK) {
       fprintf(stderr,"Could not open cdf file.\n");
       continue;
    }
    status=aceswe_pos(id,&pos,stime,etime); 
    status=aceswe_plasma(id,&plasma,stime,etime);
    CDFclose(id);
  }
  free_locate(fptr);
 
  fptr=locate_files(path,"h0_mfi",stime,etime);
  if (fptr->cnt !=0) {
    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
    
      status=acemfi_imf(id,&imf,stime,etime,0);
      CDFclose(id);
    }
    free_locate(fptr);
  } else {
    free_locate(fptr);    
    fptr=locate_files(path,"k1_mfi",stime,etime);
    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
    
      status=acemfi_imf(id,&imf,stime,etime,1);
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: imfdelay --help\n");
  return(-1);
}
 
int main(int argc,char *argv[]) {
  int arg;
  char *envstr;

  char *cfname=NULL;
  FILE *fp;

  unsigned char wind=0;
  unsigned char ace=0;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  char *fdelay_txt=NULL;

  char *stime_txt=NULL;
  char *extime_txt=NULL;
  char *etime_txt=NULL;
  char *sdate_txt=NULL;
  char *edate_txt=NULL;
  char *dpath_txt=NULL;

  double extime=-1;
  double sdate=-1;
  double edate=-1;

  float tlen=60;
  
  int yr,mo,dy,hr,mt;
  double sc;
 
  float tmp[3];
  int n;

  int delta=6*3600;
  double tval;
  double Re=6378.14;
  double distance=0;

  distance=10.0*Re;

  envstr=getenv("ISTP_PATH");
  if (envstr !=NULL) strcpy(dpath,envstr);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"st",'t',&stime_txt);
  OptionAdd(&opt,"et",'t',&etime_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);
  OptionAdd(&opt,"tl",'f',&tlen);


  OptionAdd(&opt,"path",'t',&dpath_txt);

  OptionAdd(&opt,"ace",'x',&ace);
  OptionAdd(&opt,"wind",'x',&wind);

  OptionAdd(&opt,"fixdelay",'t',&fdelay_txt);
 
  OptionAdd(&opt,"cf",'t',&cfname);

  if (argc>1) { 
    arg=OptionProcess(1,argc,argv,&opt,rst_opterr);
    if (arg==-1) {
      exit(-1);
    }
    if (cfname !=NULL) { /* load the configuration file */
      do {
        fp=fopen(cfname,"r");
        if (fp==NULL) break;
        free(cfname);
        cfname=NULL;
        optf=OptionProcessFile(fp);
        if (optf !=NULL) {
          arg=OptionProcess(0,optf->argc,optf->argv,&opt,rst_opterr);
          if (arg==-1) {
            fclose(fp);
            OptionFreeFile(optf);
            exit(-1);
          }
          OptionFreeFile(optf);
	}   
        fclose(fp);
      } while (cfname !=NULL);
    }
  } else {
    OptionPrintInfo(stderr,errstr);
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

  if ((ace+wind)==0) ace=1;

  if (fdelay_txt !=NULL) fdelay=strtime(fdelay_txt);

  if (dpath_txt !=NULL) strcpy(dpath,dpath_txt);

  if (stime_txt !=NULL) stime=strtime(stime_txt);
  if (etime_txt !=NULL) etime=strtime(etime_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL) sdate=strdate(sdate_txt);
  if (edate_txt !=NULL) edate=strdate(edate_txt);

  

  if (sdate==-1) sdate=TimeYMDHMSToEpoch(1997,1,1,0,0,0);
  if (stime==-1) stime=0;

  stime+=sdate;

  if (edate==-1) edate=sdate; 
  if (etime==-1) etime=0;  

  etime+=edate;
  if (extime !=-1) etime=stime+extime;


  if (etime<=stime) etime=stime+(24*3600); /* plot a day */
    
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stderr,"Start time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
  TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stderr,"End time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
  fprintf(stderr,"Time step:%f secs.\n",tlen);


  stime=stime-delta;
  etime=etime+delta;
 
  if (ace==1) load_ace();
  else if (wind==1)load_wind();

  stime=stime+delta;
  etime=etime-delta;


  if (pos.cnt !=0) {  
    swcnt=pos.cnt;
    swdata=malloc(sizeof(struct swdata)*swcnt);
   
    for (n=0;n<swcnt;n++) {
      swdata[n].time=pos.time[n];

      findvalue(0,pos.cnt,pos.time,pos.PGSM,swdata[n].time,tmp);
      swdata[n].px=tmp[0];
      swdata[n].py=tmp[1];
      swdata[n].pz=tmp[2];

      findvalue(0,plasma.cnt,plasma.time,plasma.VGSM,swdata[n].time,tmp);
      swdata[n].vx=tmp[0];
      swdata[n].vy=tmp[1];
      swdata[n].vz=tmp[2];
    
      findvalue(0,imf.cnt,imf.time,imf.BGSMc,swdata[n].time,tmp);
      swdata[n].bx=tmp[0];
      swdata[n].by=tmp[1];
      swdata[n].bz=tmp[2];
    }

    for (n=0;n<swcnt;n++) {
      delta=(swdata[n].px-distance)/fabs(swdata[n].vx);
      swdata[n].time=swdata[n].time+delta;
    }
 
    qsort(swdata,swcnt,sizeof(struct swdata),swcompar);
  } else {
    swcnt=imf.cnt;
    swdata=malloc(sizeof(struct swdata)*swcnt);
    for (n=0;n<swcnt;n++) {
      swdata[n].time=imf.time[n]+fdelay;
      findvalue(0,imf.cnt,imf.time,imf.BGSMc,swdata[n].time,tmp);
      swdata[n].bx=tmp[0];
      swdata[n].by=tmp[1];
      swdata[n].bz=tmp[2];
    }
  }

  tvalue=malloc(sizeof(double)*swcnt);
  bfield=malloc(sizeof(float)*3*swcnt);
  for (n=0;n<swcnt;n++) {
    tvalue[n]=swdata[n].time;
    bfield[3*n]=swdata[n].bx;
    bfield[3*n+1]=swdata[n].by;
    bfield[3*n+2]=swdata[n].bz;
  }

  tval=stime;
  while (tval<etime) {
    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

    findvalue(0,swcnt,tvalue,bfield,tval,tmp);
    fprintf(stdout,"%d %d %d %d %d %f %#12.4f %#12.4f %#12.4f\n",
            yr,mo,dy,hr,mt,sc,tmp[0],tmp[1],tmp[2]);
    tval+=tlen;
  }

  return 0;
}
   


















