/* radar.c
   =======
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
#include "rtime.h"
#include "radar.h"




struct RadarSite *RadarEpochGetSite(struct Radar *ptr,double tval) {

  int s;

  if ((ptr->st_time !=-1) && (tval<ptr->st_time)) return NULL;
  if ((ptr->ed_time !=-1) && (tval>ptr->ed_time)) return NULL;
  for (s=0;(s<ptr->snum) && (ptr->site[s].tval !=-1) && 
      (ptr->site[s].tval<tval);s++);
  if (s==ptr->snum) return NULL;
  return &(ptr->site[s]);
 
}

struct RadarSite *RadarYMDHMSGetSite(struct Radar *ptr,int yr,
                               int mo,int dy,int hr,int mt,int sc) {
 
  double tval;
  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  return RadarEpochGetSite(ptr,tval);
}


struct Radar *RadarGetRadar(struct RadarNetwork *ptr,int stid) {
  int r=0;
  for (r=0;(r<ptr->rnum) && (ptr->radar[r].id!=stid);r++);
  if (r==ptr->rnum) return NULL;
  return &ptr->radar[r];
}

int RadarGetID(struct RadarNetwork *ptr,char *code) {
  int r=0,c=0;
  if (code==NULL) return -1;
  if (ptr==NULL) return -1;
  
  for (r=0;(r<ptr->rnum);r++) {
    for (c=0;(c<ptr->radar[r].cnum) && 
        (strcmp(ptr->radar[r].code[c],code) !=0);c++);
    if (c<ptr->radar[r].cnum) break;   
  }
  
  if (r==ptr->rnum) return -1;
  return ptr->radar[r].id;
}


int RadarGetCodeNum(struct RadarNetwork *ptr,int stid) {
  int r=0;
  if (ptr==NULL) return -1;
  for (r=0;(r<ptr->rnum) && (ptr->radar[r].id!=stid);r++);
  if (r==ptr->rnum) return -1;
  return ptr->radar[r].cnum;
}

char *RadarGetCode(struct RadarNetwork *ptr,int stid,int cnum) {
  int r=0;
  if (ptr==NULL) return NULL;
  for (r=0;(r<ptr->rnum) && (ptr->radar[r].id!=stid);r++);
  if (r==ptr->rnum) return NULL;
  if (ptr->radar[r].cnum<=cnum) cnum=ptr->radar[r].cnum-1;
  return ptr->radar[r].code[cnum];
}

char *RadarGetName(struct RadarNetwork *ptr,int stid) {
  int r=0;
  if (ptr==NULL) return NULL;
  for (r=0;(r<ptr->rnum) && (ptr->radar[r].id!=stid);r++);
  if (r==ptr->rnum) return NULL;
  return ptr->radar[r].name;
}

char *RadarGetOperator(struct RadarNetwork *ptr,int stid) {
  int r=0;
  if (ptr==NULL) return NULL;
  for (r=0;(r<ptr->rnum) && (ptr->radar[r].id!=stid);r++);
  if (r==ptr->rnum) return NULL;
  return ptr->radar[r].operator;
}

int RadarGetStatus(struct RadarNetwork *ptr,int stid) {
  int r=0;
  for (r=0;(r<ptr->rnum) && (ptr->radar[r].id!=stid);r++);
  if (r==ptr->rnum) return -1;
  return ptr->radar[r].status;
}



void RadarFree(struct RadarNetwork *ptr) {
  int r,c;
  if (ptr==NULL) return;
  for (r=0;r<ptr->rnum;r++) {
    for (c=0;c<ptr->radar[r].cnum;c++) 
    if (ptr->radar[r].code[c] !=NULL) free(ptr->radar[r].code[c]);
    if (ptr->radar[r].code !=NULL) free(ptr->radar[r].code);
    if (ptr->radar[r].name !=NULL) free(ptr->radar[r].name);
    if (ptr->radar[r].operator !=NULL) free(ptr->radar[r].operator);
    if (ptr->radar[r].hdwfname !=NULL) free(ptr->radar[r].hdwfname);
    if (ptr->radar[r].site !=NULL) free(ptr->radar[r].site);
  }
  free(ptr->radar);
  free(ptr);
}

int RadarLoadHardware(char *hdwpath,struct RadarNetwork *ptr) {
  int i,n;
  FILE *fp;
  char fname[256];
  char line[256];
  int snum;
  int stid,yr,mo,dy,hr,mt,sc,yrsec;
  double tval;
  double geolat, geolon, alt; 
  double boresite, bmsep; 
  double vdir,atten,tdiff,phidiff;
  double interfer[3]; 
  double recrise;
  int maxatten,maxrange,maxbeam;
  int status;
  if (ptr==NULL) return -1;
  if (hdwpath==NULL) return -1;
  for (n=0;n<ptr->rnum;n++) {
    sprintf(fname,"%s/%s",hdwpath,ptr->radar[n].hdwfname);
    fp=fopen(fname,"r");
    if (fp==NULL) continue;
    snum=0;
    while(fgets(line,256,fp) !=NULL) {
      for (i=0;(line[i] !=0) && ((line[i]=='\n') || (line[i]==' '));i++);
      if (line[i]==0) continue;
      if (line[i]=='#') continue;
      status=sscanf(line+i, 
                  "%d%d%d%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%d%d%d",
	          &stid, &yr, &yrsec, &geolat, &geolon,
	          &alt, &boresite, &bmsep,
	          &vdir, &atten, &tdiff, &phidiff,
	          &interfer[0], &interfer[1], 
	          &interfer[2],
		  &recrise,&maxatten,&maxrange,&maxbeam);
      
      if (status<16) continue;
      if (stid !=ptr->radar[n].id) continue;
      
      if (ptr->radar[n].site==NULL) 
          ptr->radar[n].site=malloc(sizeof(struct RadarSite));
      else ptr->radar[n].site=realloc(ptr->radar[n].site,
                                      sizeof(struct RadarSite)*(snum+1));
      if (ptr->radar[n].site==NULL) break;

      
      if (yr==2999) tval=-1;
      else {
        TimeYrsecToYMDHMS(yrsec,yr,&mo,&dy,&hr,&mt,&sc);  
        tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
      } 

      ptr->radar[n].site[snum].tval=tval;
      ptr->radar[n].site[snum].geolat=geolat;
      ptr->radar[n].site[snum].geolon=geolon;
      ptr->radar[n].site[snum].alt=alt;
      ptr->radar[n].site[snum].boresite=boresite;
      ptr->radar[n].site[snum].bmsep=bmsep;
      ptr->radar[n].site[snum].vdir=vdir;
      ptr->radar[n].site[snum].atten=atten;
      ptr->radar[n].site[snum].tdiff=tdiff;
      ptr->radar[n].site[snum].phidiff=phidiff;
      ptr->radar[n].site[snum].interfer[0]=interfer[0];
      ptr->radar[n].site[snum].interfer[1]=interfer[1];
      ptr->radar[n].site[snum].interfer[2]=interfer[2];
      ptr->radar[n].site[snum].recrise=recrise;
      if (status>=17) ptr->radar[n].site[snum].maxatten=maxatten;
      else ptr->radar[n].site[snum].maxatten=3;
      if (status>=18) ptr->radar[n].site[snum].maxrange=maxrange;
      else ptr->radar[n].site[snum].maxrange=75;
      if (status>=19) ptr->radar[n].site[snum].maxbeam=maxbeam;
      else ptr->radar[n].site[snum].maxbeam=16;
      snum++;
      ptr->radar[n].snum=snum;
    }
    fclose(fp);
  }
  return 0;
}

struct RadarNetwork *RadarLoad(FILE *fp) {

  struct RadarNetwork *ptr=NULL;
  int i,j,k,c;
  char line[1024];
  int yr,mo,dy;  
  int num=0;
  
  if (fp==NULL) return NULL;
  ptr=malloc(sizeof(struct RadarNetwork));
  if (ptr==NULL) return NULL;
  ptr->rnum=0;
  ptr->radar=NULL;
  
  while(fgets(line,1023,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]=='\n') || (line[i]==' '));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;

    if (ptr->radar==NULL) ptr->radar=malloc(sizeof(struct Radar));
    else ptr->radar=realloc(ptr->radar,sizeof(struct Radar)*(num+1));
    if (ptr->radar==NULL) break;

    ptr->radar[num].snum=0;
    ptr->radar[num].site=NULL;  
    ptr->radar[num].cnum=0;
    ptr->radar[num].code=NULL;
    ptr->radar[num].operator=NULL;
    ptr->radar[num].name=NULL;
    ptr->radar[num].hdwfname=NULL;
    ptr->radar[num].st_time=-1;
    ptr->radar[num].ed_time=-1;


    for (j=i;(line[j] !=' ') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    line[j]=0;
    ptr->radar[num].id=atoi(line+i);
    i=j+1;
    for (j=i;(line[j]==' ') && (line[j] !=0);j++);
    i=j;
    for (j=i;(line[j]!=' ') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    line[j]=0;
    ptr->radar[num].status=atoi(line+i);
    i=j+1;

    for (j=i;(line[j]!=' ') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    line[j]=0;
    if (line[j-1]=='-') ptr->radar[num].st_time=-1;
    else {
      dy=atoi(line+j-2);
      line[j-2]=0;
      mo=atoi(line+j-4);
      line[j-4]=0;
      yr=atoi(line+j-8);
      ptr->radar[num].st_time=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0.0);
    }
    i=j+1;

    for (j=i;(line[j]!=' ') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    line[j]=0;
    if (line[j-1]=='-') ptr->radar[num].ed_time=-1;
    else {
      dy=atoi(line+j-2);
      line[j-2]=0;
      mo=atoi(line+j-4);
      line[j-4]=0;
      yr=atoi(line+j-8);
      ptr->radar[num].ed_time=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0.0);
    }
    i=j+1;
    for (j=i;(line[j] !='"') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    i=j+1;
    for (k=0;(line[i+k] !='"') && (line[i+k] !=0);k++) 
    if (line[i+k]==0) continue;
    ptr->radar[num].name=malloc(k+1);
    if (ptr->radar[num].name==NULL) continue;
    memcpy(ptr->radar[num].name,line+i,k);
    ptr->radar[num].name[k]=0;
    i=i+k+1;

    for (j=i;(line[j] !='"') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    i=j+1;
    for (k=0;(line[i+k] !='"') && (line[i+k] !=0);k++) 
    if (line[i+k]==0) continue;
    ptr->radar[num].operator=malloc(k+1);
    if (ptr->radar[num].operator==NULL) continue;
    memcpy(ptr->radar[num].operator,line+i,k);
    ptr->radar[num].operator[k]=0;
    i=i+k+1;


    for (j=i;(line[j] !='"') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    i=j+1;
    for (k=0;(line[i+k] !='"') && (line[i+k] !=0);k++) 
    if (line[i+k]==0) continue;
    ptr->radar[num].hdwfname=malloc(k+1);
    if (ptr->radar[num].hdwfname==NULL) continue;
    memcpy(ptr->radar[num].hdwfname,line+i,k);
    ptr->radar[num].hdwfname[k]=0;
    i=i+k+1;

    c=0;
    while (line[i] !=0) { 
      for (j=i;(line[j] !='"') && (line[j] !=0);j++);
      if (line[j]==0) break;
      i=j+1;
      for (k=0;(line[i+k] !='"') && (line[i+k] !=0);k++) 
      if (line[i+k]==0) break;
      if (ptr->radar[num].code==NULL) 
        ptr->radar[num].code=malloc(sizeof(char *));   
      else {
        char **tmp;
        tmp=realloc(ptr->radar[num].code,(c+1)*sizeof(char *));
        if (tmp==NULL) break;
        ptr->radar[num].code=tmp;
      } 
      ptr->radar[num].code[c]=malloc(k+1);
      if (ptr->radar[num].code[c]==NULL) break;
      memcpy(ptr->radar[num].code[c],line+i,k);
      ptr->radar[num].code[c][k]=0;
     
      c++;
      i=i+k+1;
    }
    ptr->radar[num].cnum=c;
    num++; 
  }
  ptr->rnum=num;
  return ptr;
};



