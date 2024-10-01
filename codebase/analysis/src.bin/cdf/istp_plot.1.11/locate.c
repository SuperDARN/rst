/* locate.c
   ========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/



/* This is a simple test program to demonstrate how to read a CDF file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>

#include "rtypes.h"
#include "rtime.h"
#include "cdf.h"
#include "rcdf.h"
#include "locate.h"

#define ONE_MONTH (28*24*3600)

void free_locate(struct file_list *ptr) {
  int i;
  for (i=0;i<ptr->cnt;i++) if (ptr->fname[i] !=NULL) free(ptr->fname[i]);
  free(ptr);

}

int infname(char *body,char *str) { 
  
  int i=0; 
  int j=0;
  while (body[i] !=0) {
    if (body[i]==str[j]) j++;
    else j=0;
    if (str[j]==0) break;
    i++;
  }
  if (body[i]==0) return 0;
  return 1;
}

int dsel(const struct dirent *dp) {
  if (dp->d_name[0]=='.') return 0;
  return 1;
}

int test_file_epoch(char *fname,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double fstime=0,fetime=0;
  int max=0;  
  CDFid id;
  CDFstatus status;
  struct RCDFData *data=NULL;
  double *dptr;
  char *varlist[]={"Epoch",NULL};

  data=RCDFMake(1);
  if (data==NULL) return -1;

  status=CDFopen(fname,&id);
  if (status !=CDF_OK) {
    RCDFFree(data,1);
    return -1;
  }

  max=RCDFMaxRecR(id,varlist[0]);


  if (max>0) {
     status=RCDFReadR(id,0,varlist,data);
     if (status==-1) {
       CDFclose(&id);
       RCDFFree(data,1);
       return -1;
     }
     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fstime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
     status=RCDFReadR(id,max-1,varlist,data);
     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fetime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
  }

  max=RCDFMaxRecZ(id,varlist[0]);

  if (max>0) {
     status=RCDFReadZ(id,0,varlist,data);
     if (status==-1) {
       CDFclose(&id);
       RCDFFree(data,1);
       return -1;
     }

     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fstime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
     status=RCDFReadZ(id,max-1,varlist,data);
     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fetime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
  }
  CDFclose(&id);
  RCDFFree(data,1);
  if (stime>fetime) return 0;
  if (etime<fstime) return 0;
  return 1;
}

struct file_list *locate_files(char *fpath,char *prefix,
                               double stime,double etime) {

  char tmp[256];
  char dir[256];
  char fname[256];
  char ofname[256];

  char *flist[4096];
  int fcnt=0;

  int status;
  int dyval;
  struct dirent **dlist=NULL;
  int yr,mo,dy,hr,mt;
  double sc;
  int m=0;
  int state=0;
  int syr,eyr;
  int sdyval,edyval;
  
  struct file_list *ptr;
  ptr=malloc(sizeof(struct file_list));
  ptr->cnt=0;
  ptr->fname=NULL;
  
  ofname[0]=0;

  /* The yearc check should look one month either side
     of the desired period to make sure that we catch all
     possible files that might contain the requested period, even if
     they span directories.  */

  TimeEpochToYMDHMS(stime-ONE_MONTH,&syr,&mo,&dy,&hr,&mt,&sc);
  TimeEpochToYMDHMS(etime+ONE_MONTH,&eyr,&mo,&dy,&hr,&mt,&sc);

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sdyval=yr*10000+mo*100+dy;
  TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);
  edyval=yr*10000+mo*100+dy;

  
  
  for (yr=syr;yr<=eyr;yr++) {
    
    sprintf(dir,"%s/%.4d",fpath,yr);
   
    status=scandir(dir,&dlist,dsel,alphasort);
    
    if (status<=0) continue;

    /* now get only the most recent version numbers of the files */

    fcnt=0;
    for (m=0;m<status;m++) {      
      if ((prefix !=NULL) && (infname(dlist[m]->d_name,prefix) ==0)) continue;
     
      if (fcnt==0) {
       flist[fcnt]=dlist[m]->d_name;
       fcnt++;
      } else {
        strcpy(tmp,dlist[m]->d_name);
        strcpy(tmp+128,flist[fcnt-1]);
        tmp[18]=0;
        tmp[128+18]=0;
       
        
        if (strcmp(tmp,tmp+128)==0) flist[fcnt-1]=dlist[m]->d_name;
        else {
          flist[fcnt]=dlist[m]->d_name;
          fcnt++;
        }
      
      }
    }

    for (m=0;m<fcnt;m++) {  
        strcpy(tmp,flist[m]);
        sprintf(fname,"%s/%s",dir,flist[m]);
        tmp[18]=0;
        dyval=atoi(tmp+10); 
        if ((dyval>=sdyval) && (state==0)) {
          if (ofname[0] !=0) {
             status=test_file_epoch(ofname,stime,etime);
             if (status==-1) break;
             if (status==1) {
               if (ptr->fname==NULL) ptr->fname=malloc(sizeof(char *));
               else ptr->fname=realloc(ptr->fname,
                                      (ptr->cnt+1)*sizeof(char *));
               ptr->fname[ptr->cnt]=malloc(strlen(ofname)+1);
               strcpy(ptr->fname[ptr->cnt],ofname);
               ptr->cnt++;
	    }
	  }
          state=1;
	}
	      
        if ((state==1) && (dyval<=edyval)) {
          status=test_file_epoch(fname,stime,etime);
          if (status==1) {
             if (ptr->fname==NULL) ptr->fname=malloc(sizeof(char *));
             else ptr->fname=realloc(ptr->fname,
                                    (ptr->cnt+1)*sizeof(char *));
             ptr->fname[ptr->cnt]=malloc(strlen(fname)+1);
             strcpy(ptr->fname[ptr->cnt],fname);
             ptr->cnt++;
	  }
	} else if (state==1) state=2;
        strcpy(ofname,fname);
             
      }
  }
  if (m !=fcnt) {
    for (m=0;m<ptr->cnt;m++) if (ptr->fname[m] !=NULL) free(ptr->fname[m]);
    free(ptr); 
    return NULL;
  }
  return ptr;
}
   




