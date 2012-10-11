/* rawseek.c
   =========
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "oldrawread.h"



int OldRawSeek(struct OldRawFp *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  int status=0;
  double tval=0;
  int rjmp=0;
  struct RadarParm *prm=NULL;
  struct RawData *raw=NULL;

  prm=RadarParmMake();
  raw=RawMake();

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
 
  if (tval<fp->ctime) {
    lseek(fp->rawfp,fp->frec,SEEK_SET);
    fp->ptr=fp->frec;
    status=OldRawRead(fp,prm,raw);
  }
 
  do {
    rjmp=fp->rlen;
    status=OldRawRead(fp,prm,raw);
  } while ((tval>=fp->ctime) && (status==0));
   

  lseek(fp->rawfp,-(fp->rlen+rjmp),SEEK_CUR);
  fp->ptr-=fp->rlen+rjmp;
  
  status=OldRawRead(fp,prm,raw);
  
  lseek(fp->rawfp,-fp->rlen,SEEK_CUR);
  fp->ptr-=fp->rlen;
   
  RadarParmFree(prm);
  RawFree(raw);

  if (atme !=NULL) *atme=fp->ctime;
  return 4;
}


