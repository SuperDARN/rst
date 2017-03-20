/* rawseek.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
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


