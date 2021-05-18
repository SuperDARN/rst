/* map_addimf.c
   ===========
   Author: R.J.Barnes and others


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

/*
 * SGS: the wind, ace, etc. functions should be in the respective .c files,
 *      not in this file. I suspect they are located here to access global
 *      variables defined in this file.
 *
 * SGS: this function should be renamed to reflect the fact that it adds IMF,
 *      solar wind, tilt and (in the future) activity.
 *
 * SGS: add better directory specification
 *
 * SGS: add omni data files and functions to read them
 *
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
struct CnvMapData *map;

char dpath[256]={"/data"};

double st_time;
double ed_time;

struct file_list *fptr;
struct swdata sw;

struct RCDFData data[10];

struct OptionData opt;

#define DELAYSTEP 50
#define IMFSTEP 50

struct delaytab {
  int num;
  double *time;
  float *delay;
};

/*
 * function prototypes
 */
/*int findvalue(int inx,int cnt,double *time,float *data,double tval,float *val);*/
int findvalue(struct swdata *ptr, double tme, float *val);
int load_text(FILE *fp, struct swdata *ptr);
struct delaytab *load_delay(FILE *fp);
double strtime(char *text);
int load_omni();
int load_wind();
int load_ace();

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: map_addimf --help\n");
  return(-1);
}

int main(int argc,char *argv[])
{
  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  char *envstr;

  char *dname=NULL;
  struct delaytab *dtable=NULL;

  char *iname=NULL;

  unsigned char aflg=0,wflg=0,oflg=0;

  int yr,mo,dy,hr,mt;
  double sc;
  double tme;

  int s;
  float extent=24*3600;
  float delay=1800;
  float dBx=0;
  float dBy=0;
  float dBz=0;

  float dVx=0;
  float dtilt=-99;
  float dKp=0;

  char *pstr=NULL;
  char *dstr=NULL;
  char *estr=NULL;

  float tmp[5];

  int k;

  /* function pointers for file reading/writing (old and new) */
  int (*Map_Read)(FILE *, struct CnvMapData *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);

  grd = GridMake();
  map = CnvMapMake();

  envstr = getenv("ISTP_PATH");
  if (envstr != NULL) strcpy(dpath,envstr);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"ace",'x',&aflg);
  OptionAdd(&opt,"wind",'x',&wflg);
  OptionAdd(&opt,"omni",'x',&oflg);
  OptionAdd(&opt,"if",'t',&iname);
  OptionAdd(&opt,"df",'t',&dname);

  OptionAdd(&opt,"p",'t',&pstr);
  OptionAdd(&opt,"d",'t',&dstr);

  OptionAdd(&opt,"bx",'f',&dBx);
  OptionAdd(&opt,"by",'f',&dBy);
  OptionAdd(&opt,"bz",'f',&dBz);

  OptionAdd(&opt,"vx",'f',&dVx);
  OptionAdd(&opt,"tilt",'f',&dtilt);
  OptionAdd(&opt,"kp",'f',&dKp);

  OptionAdd(&opt,"ex",'t',&estr);

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

  if (iname != NULL) {
    fp = fopen(iname,"r");
    if (fp !=NULL) {
     load_text(fp,&sw);
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

  if (dtable !=NULL) delay = dtable->delay[0];

  /* set function pointer to read/write old or new */
  if (old) {
    Map_Read  = &OldCnvMapFread;
    Map_Write = &OldCnvMapFwrite;
  } else {
    Map_Read  = &CnvMapFread;
    Map_Write = &CnvMapFwrite;
  }

  s = (*Map_Read)(fp,map,grd);

  st_time = map->st_time - delay;
  ed_time = map->st_time - delay + extent;

  if (wflg == 1)      load_wind();
  else if (aflg == 1) load_ace();
  else if (oflg == 1) load_omni();

  k = 0;

  do {

    if (dtable != NULL) {
      while ((k < dtable->num) && (dtable->time[k] <= map->st_time)) k++;
      if (k == 0) delay = dtable->delay[0];
      else        delay = dtable->delay[k-1];
    }

    tme = map->st_time - delay;
    map->Bx = dBx;
    map->By = dBy;
    map->Bz = dBz;
    if (old) map->Bx = dVx;  /* SGS: consider modifying the map structure */
    else     map->Vx = dVx;
    map->Kp = dKp;

    map->imf_flag = 9;

    if (sw.cnt != 0) {
/*      findvalue(0,sw.cnt,sw.time,sw.BGSMc,tme,tmp);*/
      findvalue(&sw,tme,tmp);
      map->Bx = tmp[0];
      map->By = tmp[1];
      map->Bz = tmp[2];
      if (old) map->Bx = tmp[3];
      else     map->Vx = tmp[3];
      map->Kp = tmp[4];
    }
    map->imf_delay = delay/60;

    (*Map_Write)(stdout,map,grd);

    if (vb==1) {
       TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
       if (old)
         fprintf(stderr,
                 "%d-%d-%d %d:%d:%d delay=%d:%d Vx=%g By=%g Bz=%g\n",
                 yr,mo,dy,hr,mt,(int) sc,(int) (delay/3600),
                 ( (int) delay % 3600)/60, map->Bx,map->By,map->Bz);
       else
         fprintf(stderr,
                 "%d-%d-%d %d:%d:%d delay=%d:%d Bx=%g By=%g Bz=%g Vx=%g Kp=%g\n",
                 yr,mo,dy,hr,mt,(int) sc,(int) (delay/3600),
                 ( (int) delay % 3600)/60, map->Bx,map->By,map->Bz,map->Vx,map->Kp);
    }

    s = (*Map_Read)(fp,map,grd);

  } while (s != -1);

  fclose(fp);

  return 0;
}


/*int findvalue(int inx, int cnt, double *time, float *data, double tval,
              float *val)*/
int findvalue(struct swdata *ptr, double tme, float *val)
{
  int i,cnt;
  double stime,etime,v;
  float *imf, *sw, *kp;
  int sinx,einx;

  imf = ptr->BGSMc;
  sw  = ptr->Vx;
  kp  = ptr->Kp;
  cnt = ptr->cnt;

  /* Fill in with default values */
  for (i=0; i<5; i++) val[i] = FILL_VALUE;

  /* Increment i until end of file or until the start time */
  /* of making the map is reached.  This is likely most */
  /* useful for sw data that contains multiple days.  OMNI */
  /* data is produced in monthly and yearly files. */
  for (i=0; (i < cnt) && (ptr->time[i] <= tme); i++);

  /* Skip over where the IMF value is invalid.  Here invalid */
  /* is evaluated by being greater than half of the */
  /* FILL_VALUE value. */
  einx = i;
  while ((einx < cnt) && (fabs(imf[3*einx]) > fabs(FILL_VALUE/2))) einx++;
  sinx = i-1;
  while ((sinx >= 0)  && (fabs(imf[3*sinx]) > fabs(FILL_VALUE/2))) sinx--;

  if (sinx < 0)    sinx = 0;      /* start index */
  if (einx >= cnt) einx = cnt-1;  /* end   index */

  etime = ptr->time[einx];
  stime = ptr->time[sinx];
  /* The tme < stime and tme > etime are believed to be */
  /* error checking in case something has gone wrong. */
  /* Otherwise, the sinx != einx condition exempts when there */
  /* is a gap in IMF data at the beginning or end of an IMF */
  /* file. These returned error codes are left unchecked */
  /* in map_addimf(). */
  if ((tme < stime) && (sinx != einx)) return -1;
  if ((tme > etime) && (sinx != einx)) return -1;

  if (einx != sinx) v = (tme - stime)/(etime - stime);
  else              v = 0;

  if ((fabs(imf[3*sinx]) < fabs(FILL_VALUE/2)) &&
      (fabs(imf[3*einx]) < fabs(FILL_VALUE/2))) {
    val[0] = imf[3*sinx]*(1-v)   + imf[3*einx]*v;
    val[1] = imf[3*sinx+1]*(1-v) + imf[3*einx+1]*v;
    val[2] = imf[3*sinx+2]*(1-v) + imf[3*einx+2]*v;
    val[3] = sw[sinx]*(1-v)      + sw[einx]*v;
    val[4] = kp[sinx]*(1-v)      + kp[einx]*v;
  } else if (fabs(imf[3*sinx]) < fabs(FILL_VALUE/2)) {
    val[0] = imf[3*sinx];
    val[1] = imf[3*sinx+1];
    val[2] = imf[3*sinx+2];
    val[3] = sw[sinx];
    val[4] = kp[sinx];
  }  else if (fabs(imf[3*einx]) < fabs(FILL_VALUE/2)) {
    val[0] = imf[3*einx];
    val[1] = imf[3*einx+1];
    val[2] = imf[3*einx+2];
    val[3] = sw[einx];
    val[4] = kp[einx];
  }

  if (val[3] == FILL_VALUE) val[3] = 0; /* set Vx to zero for default */
  if (val[4] == FILL_VALUE) val[4] = 0; /* set Kp to zero for default */

  return sinx;
}


int load_text(FILE *fp, struct swdata *ptr)
{
  int yr,mo,dy,hr,mt;
  float sc;
  float bx,by,bz,vx,kp;
  char line[256],save[256];
  char *tok;
  int i,blk,ntok;
  int cnt=0;

  ptr->time  = malloc(sizeof(double)*IMFSTEP);
  ptr->BGSMc = malloc(sizeof(float)*IMFSTEP*3);
  ptr->BGSEc = malloc(sizeof(float)*IMFSTEP*3);
  ptr->Vx    = malloc(sizeof(float)*IMFSTEP);
  ptr->Kp    = malloc(sizeof(float)*IMFSTEP);

  while(fgets(line,256,fp) != NULL) {

    for (i=0; (line[i] != 0) && ((line[i] == ' ') || (line[i] == '\t') ||
             (line[i] == '\n')); i++);
    if (line[i] == 0) continue;
    if (line[i] == '#') continue;

    /* count the number of tokens */
    strcpy(save,line);
    ntok = 0;
    tok = strtok(save, " ");
    while (tok != NULL) {
      ntok++;
      tok = strtok(NULL, " ");
    }

    if (ntok == 9)
      sscanf(line,"%d%d%d%d%d%f%f%f%f",&yr,&mo,&dy, &hr,&mt,&sc,
                  &bx,&by,&bz);
    else if (ntok == 10)
      sscanf(line,"%d%d%d%d%d%f%f%f%f%f",&yr,&mo,&dy, &hr,&mt,&sc,
                  &bx,&by,&bz, &vx);
    else if (ntok == 11)
      sscanf(line,"%d%d%d%d%d%f%f%f%f%f%f",&yr,&mo,&dy, &hr,&mt,&sc,
                  &bx,&by,&bz, &vx,&kp);
    else continue;

    ptr->time[cnt]      = TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    ptr->BGSMc[cnt*3]   = bx;
    ptr->BGSMc[cnt*3+1] = by;
    ptr->BGSMc[cnt*3+2] = bz;

    ptr->BGSEc[cnt*3]   = bx;   /* fudge as we assume the file contains */
    ptr->BGSEc[cnt*3+1] = by;   /* only one set of values. */
    ptr->BGSEc[cnt*3+2] = bz;

    if (ntok >= 10) ptr->Vx[cnt] = vx;
    else            ptr->Vx[cnt] = FILL_VALUE;

    if (ntok == 11) ptr->Kp[cnt] = kp;
    else            ptr->Kp[cnt] = FILL_VALUE;

    cnt++;
    if ((cnt % IMFSTEP) == 0) {
        blk = 1 + cnt/IMFSTEP;
        ptr->time  = realloc(ptr->time,sizeof(double)*IMFSTEP*blk);
        ptr->BGSMc = realloc(ptr->BGSMc,sizeof(float)*IMFSTEP*blk*3);
        ptr->BGSEc = realloc(ptr->BGSEc,sizeof(float)*IMFSTEP*blk*3);
        ptr->Vx    = realloc(ptr->Vx,sizeof(float)*IMFSTEP*blk);
        ptr->Kp    = realloc(ptr->Kp,sizeof(float)*IMFSTEP*blk);
    }
  }

  ptr->cnt   = cnt;
  ptr->time  = realloc(ptr->time,sizeof(double)*cnt);
  ptr->BGSMc = realloc(ptr->BGSMc,sizeof(float)*cnt*3);
  ptr->BGSMc = realloc(ptr->BGSMc,sizeof(float)*cnt*3);
  ptr->Vx    = realloc(ptr->Vx,sizeof(double)*cnt);
  ptr->Kp    = realloc(ptr->Kp,sizeof(double)*cnt);

  return 0;
}

struct delaytab *load_delay(FILE *fp)
{
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


double strtime(char *text)
{
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}


int load_omni()
{
  /* SGS: this does nothing right now */
  return (0);
}

int load_wind()
{

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

    status=windmfi_imf(id,&sw,st_time,ed_time);

    CDFclose(id);
  }
  free_locate(fptr);
  return 0;
}

int load_ace()
{

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
      status=acemfi_imf(id,&sw,st_time,ed_time,0);

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

      status=acemfi_imf(id,&sw,st_time,ed_time,1);

      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}
