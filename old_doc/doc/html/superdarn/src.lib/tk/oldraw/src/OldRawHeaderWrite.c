/* OldRawHeaderWrite.c
   ===================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "oldrawread.h"
#include "oldrawwrite.h"

int main(int argc,char *argv[]) {
 
  int thr=0; 
  int recnum=0;

  struct RadarParm prm;
  struct RawData raw;

  struct OldRawFp *fp;
  int fd=0;

  fp=OldRawOpen(argv[1],NULL);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  fd=fileno(stdout);

  OldRawHeaderWrite(fd,"rawwrite","1.00",thr,"Demonstration code.");

  while(OldRawRead(fp,&prm,&raw) !=-1) {
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);

    prm.cp=1000;
    recnum++;
    OldRawWrite(fd,"rawwrite",&prm,&raw,recnum,NULL);
    
  }
 
  OldRawClose(fp);

  return 0;
}
