/* RfileLoadIndex.c
   ================
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"

struct blk1 {
  double st_time,ed_time;
  int npnt;
  double lat[256];
  double lon[256];
  double hgt[256];
  int flg[256];
   
};

struct blk2 {
  double st_time,ed_time;
  int npnt;
  int a[256],b[256];
};

struct data {
  struct blk1 blk1;
  struct blk2 blk2;
} data;


int decode_blk1(char *name,char *unit,char *type,
                double st_time,double ed_time,
                int npnt,int nprm,int pnt,
                struct RfileData *data,void *ptr) {

  char *block="lat lon hgt flg";
  struct blk1 *dp;
  dp=(struct blk1 *) ptr;
  if (strcmp(name,block) !=0) return 0;


  dp->st_time=st_time;
  dp->ed_time=ed_time;
  dp->npnt=npnt;
 
  dp->lat[pnt]=data[0].data.fval;
  dp->lon[pnt]=data[1].data.fval;
  dp->hgt[pnt]=data[2].data.fval;
  dp->flg[pnt]=data[3].data.ival;
  return 1;
}

int decode_blk2(char *name,char *unit,char *type,
                double st_time,double ed_time,
                int npnt,int nprm,int pnt,
                struct RfileData *data,void *ptr) {

  char *block="a b";
  struct blk2 *dp;
  dp=(struct blk2 *)ptr;

  if (strcmp(name,block) !=0) return 0;

  dp->st_time=st_time;
  dp->ed_time=ed_time;
  dp->npnt=npnt;
 
  dp->a[pnt]=data[0].data.ival;
  dp->b[pnt]=data[1].data.ival;
  return 1;
}




int main(int argc,char *argv[]) {

  FILE *fp;
  struct RfileIndex *inx=NULL;
  int status;
  RfileDecoder decode[2];
  void *dptr[2];
  int i;
  int yr,mo,dy,hr,mt; 
  double sc;
  double aval;

  if (argc>2) {
    fp=fopen(argv[2],"r");
    if (fp !=NULL) {
      inx=RfileLoadIndex(fp);
      fclose(fp);
    }
  } 

  if (argc>1) {
    fp=fopen(argv[1],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
   

  } else fp=stdin;
  
  decode[0]=decode_blk1;
  decode[1]=decode_blk2;
  dptr[0]=&data.blk1;
  dptr[1]=&data.blk2;
 
  status=RfileRead(fp,2,decode,dptr);

  if (status!=-1) {
    TimeEpochToYMDHMS(data.blk1.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    mt+=1;
    RfileSeek(fp,yr,mo,dy,hr,mt,(int) sc,inx,&aval);
  }

  while (RfileRead(fp,2,decode,dptr) !=-1) {
    TimeEpochToYMDHMS(data.blk1.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%d-%d-%d %d:%d:%d to ",yr,mo,dy,hr,mt,(int) sc);
    TimeEpochToYMDHMS(data.blk1.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
 
    fprintf(stdout,"Block 1\n");
  
    fprintf(stdout,"pnt\tlat\tlon\thgt\tflg\n");
    for (i=0;i<data.blk1.npnt;i++) {
      fprintf(stdout,"%d\t%g\t%g\t%g\t%d\n",i,
             data.blk1.lat[i],data.blk1.lon[i],
             data.blk1.hgt[i],data.blk1.flg[i]);
    
    }
    fprintf(stdout,"Block 2\n");
    fprintf(stdout,"pnt\ta\tb\n");
    for (i=0;i<data.blk2.npnt;i++) {
      fprintf(stdout,"%d\t%d\t%d\n",i,
             data.blk2.a[i],data.blk2.b[i]);
    
    }
    fprintf(stdout,"\n"); 


  }
  if (fp !=stdin) fclose(fp);
  return 0;
}
  





