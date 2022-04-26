/* OldFitInxWrite.c
   ================
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


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitread.h"
#include "oldfitwrite.h"

struct RadarParm pbuf[2];
struct FitData fbuf[2];

struct FitData *fptr;
struct RadarParm *pptr;

int fnum=0;

int main(int argc,char *argv[]) {
 
  int irec=1;
  int drec=2,dnum;

  struct OldFitFp *fitfp;
  FILE *inxfp=0;

  int inxfd;


  fitfp=OldFitOpen(argv[1],NULL);

  if (fitfp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  inxfp=fopen(argv[2],"w");
  if (inxfp==NULL) {
    fprintf(stderr,"could not create index file.\n");
    exit(-1);  
  }
  inxfd=fileno(inxfp);
 
  while(OldFitRead(fitfp,&pbuf[fnum],&fbuf[fnum]) !=-1) {
  


    fptr=&fbuf[fnum];
    pptr=&pbuf[fnum];
    fnum=(fnum+1) % 2;
    if (irec==1) OldFitInxHeaderWrite(inxfd,pptr);
    dnum=OldFitFwrite(NULL,pptr,fptr,NULL);
    OldFitInxWrite(inxfd,drec,dnum,pptr);
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            pptr->time.yr,
            pptr->time.mo,pptr->time.dy,pptr->time.hr,pptr->time.mt,
            pptr->time.sc);


    drec+=dnum;
    irec++;
  }
 
  OldFitClose(fitfp);
  fclose(inxfp);
  inxfp=fopen(argv[2],"r+");
  inxfd=fileno(inxfp);
  OldFitInxClose(inxfd,pptr,irec-1);
  fclose(inxfp);

  return 0;
}
