/* map_addimf.c
   =========== 
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
#include <string.h>
#include <math.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"

#include "hlpstr.h"

#include "cdf.h"
#include "rcdf.h"
#include "locate.h"

#include "istp.h"
#include "wind.h"
#include "ace.h"

char *fname=NULL;
FILE *fp;

struct GridData *grd;
struct CnvMapData  *map;
 
char dpath[256]={"/data"};

double st_time;
double ed_time;

struct file_list *fptr;
struct imfdata imf;

struct RCDFData data[10];

struct OptionData opt;

#define DELAYSTEP 50
#define IMFSTEP 50

struct delaytab {
  int num;
  double *time;
  float *delay;
};


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




int load_text(FILE *fp,struct imfdata *ptr) {

  int yr,mo,dy,hr,mt;
  float sc;
  float bx,by,bz;
  char line[256];
  int i;
  int cnt=0;

  ptr->time=malloc(sizeof(double)*IMFSTEP);
  ptr->BGSMc=malloc(sizeof(float)*IMFSTEP*3);
  ptr->BGSEc=malloc(sizeof(float)*IMFSTEP*3);
   while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]==' ') || (line[i]=='\t') ||
             (line[i] =='\n'));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;
  
    if (sscanf(line,"%d %d %d %d %d %g %g %g %g",&yr,&mo,&dy,&hr,&mt,&sc,
              &bx,&by,&bz) != 9) continue;
    ptr->time[cnt]=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    ptr->BGSMc[cnt*3]=bx;
    ptr->BGSMc[cnt*3+1]=by;
    ptr->BGSMc[cnt*3+2]=bz;

    ptr->BGSEc[cnt*3]=bx;   /* fudge as we assume the file contains */
    ptr->BGSEc[cnt*3+1]=by; /* only one set of values. */
    ptr->BGSEc[cnt*3+2]=bz;

    cnt++;
    if ((cnt % IMFSTEP)==0) {
        int blk;
        blk=1+cnt/IMFSTEP;
        ptr->time=realloc(ptr->time,sizeof(double)*IMFSTEP*blk);
        ptr->BGSMc=realloc(ptr->BGSMc,sizeof(float)*IMFSTEP*blk*3);
        ptr->BGSEc=realloc(ptr->BGSEc,sizeof(float)*IMFSTEP*blk*3);

    }

  }
  ptr->cnt=cnt;
  ptr->time=realloc(ptr->time,sizeof(double)*cnt);
  ptr->BGSMc=realloc(ptr->BGSMc,sizeof(float)*cnt*3);
  ptr->BGSMc=realloc(ptr->BGSMc,sizeof(float)*cnt*3);
  return 0;
}

struct delaytab *load_delay(FILE *fp) {
  struct delaytab *ptr;
  int yr,mo,dy,hr,mt;
  float sc;
  int dhr,dmt;
  char line[256];
  int i;
  int cnt=0;
  ptr=malloc(sizeof(struct delaytab));
  ptr->time=malloc(sizeof(double)*DELAYSTEP);
  ptr->delay=malloc(sizeof(float)*DELAYSTEP);
 
  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]==' ') || (line[i]=='\t') ||
             (line[i] =='\n'));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;
  
    if (sscanf(line,"%d %d %d %d %d %g %d %d",&yr,&mo,&dy,&hr,&mt,&sc,
              &dhr,&dmt) != 8) continue;

    ptr->time[cnt]=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    ptr->delay[cnt]=dhr*3600+dmt*60;
    cnt++;
    if ((cnt % DELAYSTEP)==0) {
        int blk;
        blk=1+cnt/DELAYSTEP;
        ptr->time=realloc(ptr->time,sizeof(double)*DELAYSTEP*blk);
        ptr->delay=realloc(ptr->delay,sizeof(float)*DELAYSTEP*blk);
    }

  }

  ptr->num=cnt;
  ptr->time=realloc(ptr->time,sizeof(double)*cnt);
  ptr->delay=realloc(ptr->delay,sizeof(float)*cnt);
 
  return ptr;
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

  fprintf(stderr,"%s\n",path);

  fptr=locate_files(path,"mfi",st_time,ed_time);

  for (i=0;i<fptr->cnt;i++) {
    fprintf(stderr,"%s\n",fptr->fname[i]);

     status=CDFopen(fptr->fname[i],&id);
    if (status !=CDF_OK) {
      fprintf(stderr,"Could not open cdf file.\n");
      continue;
    }
  
    status=windmfi_imf(id,&imf,st_time,ed_time);
    
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
  fprintf(stderr,"%s\n",path);

  /* first check to see if we have the h0 files */

  fptr=locate_files(path,"h0_mfi",st_time,ed_time);

  if (fptr->cnt !=0) {
    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);
      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
      status=acemfi_imf(id,&imf,st_time,ed_time,0);
    
      CDFclose(id);
    }
    free_locate(fptr);
  } else {
    free_locate(fptr);    
    fptr=locate_files(path,"k1_mfi",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

       status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
    
      status=acemfi_imf(id,&imf,st_time,ed_time,1);
    
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}



int main(int argc,char *argv[]) {

 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */

  int old=0;
  int new=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  char *envstr;
 
  char *dname=NULL;
  struct delaytab *dtable=NULL;

  char *iname=NULL;
 

  unsigned char aflg=0,wflg=0;

  int yr,mo,dy,hr,mt;
  double sc;
  double tme;

  int s;
  float extent=24*3600;
  float delay=1800;
  float dBx=0;
  float dBy=0;
  float dBz=0;

  char *pstr=NULL;
  char *dstr=NULL;
  char *estr=NULL;

  float tmp[3];

  int j,k;

  grd=GridMake();
  map=CnvMapMake(); 
 
  envstr=getenv("ISTP_PATH");
  if (envstr !=NULL) strcpy(dpath,envstr);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"ace",'x',&aflg);
  OptionAdd(&opt,"wind",'x',&wflg);
  OptionAdd(&opt,"if",'t',&iname);
  OptionAdd(&opt,"df",'t',&dname);

  OptionAdd(&opt,"p",'t',&pstr);
  OptionAdd(&opt,"d",'t',&dstr);

  OptionAdd(&opt,"bx",'f',&dBx);
  OptionAdd(&opt,"by",'f',&dBy);
  OptionAdd(&opt,"bz",'f',&dBz);

  OptionAdd(&opt,"ex",'t',&estr);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (pstr !=NULL) strcpy(dpath,pstr);
  if (dstr !=NULL) delay=strtime(dstr);  
  if (estr !=NULL) extent=strtime(estr);
  
  if (arg !=argc) fname=argv[arg];


  if (dname !=NULL) {
    fp=fopen(dname,"r");
    if (fp !=NULL) {
     dtable=load_delay(fp);
     fclose(fp);
    }
  }  

  if (iname !=NULL) {
    fp=fopen(iname,"r");
    if (fp !=NULL) {
     load_text(fp,&imf);
     fclose(fp);
    }
  }


  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;

  if (dtable !=NULL) delay=dtable->delay[0];

  if (old) s=OldCnvMapFread(fp,map,grd);
  else s=CnvMapFread(fp,map,grd);

  st_time=map->st_time-delay;
  ed_time=map->st_time-delay+extent; 

  if (wflg==1) load_wind();
  else if (aflg==1) load_ace();
    

 

  j=0;
  k=0;

  do {  

    if (dtable !=NULL) {
      while ((k<dtable->num) && (dtable->time[k]<=map->st_time)) k++;
      if (k==0) delay=dtable->delay[0];
      else delay=dtable->delay[k-1];
    }  
    
 
    tme=map->st_time-delay;
    map->Bx=dBx;
    map->By=dBy;
    map->Bz=dBz;
    map->imf_flag=9;

    if (imf.cnt !=0) {
      findvalue(0,imf.cnt,imf.time,imf.BGSMc,tme,tmp);
      map->Bx=tmp[0];
      map->By=tmp[1];
      map->Bz=tmp[2];
    }
    map->imf_delay=delay/60;
    if (old) OldCnvMapFwrite(stdout,map,grd);
    else CnvMapFwrite(stdout,map,grd);

    if (vb==1) {
       TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
       fprintf(stderr,
               "%d-%d-%d %d:%d:%d delay=%d:%d Bx=%g By=%g Bz=%g\n",
               yr,mo,dy,hr,mt,(int) sc,(int) (delay/3600),
               ( (int) delay % 3600)/60,
               map->Bx,map->By,map->Bz);
    }  

    if (old) s=OldCnvMapFread(fp,map,grd);
    else s=CnvMapFread(fp,map,grd);

  } while (s!=-1);


  fclose(fp); 
  return 0; 
}







