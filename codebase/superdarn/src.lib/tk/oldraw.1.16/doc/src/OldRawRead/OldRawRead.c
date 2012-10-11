/* OldRawRead.c
   ============
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

int main(int argc,char *argv[]) {
 
 

  struct RadarParm prm;
  struct RawData raw;

  struct OldRawFp *fp;

  fp=OldRawOpen(argv[1],NULL);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(OldRawRead(fp,&prm,&raw) !=-1) {
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
  }
 
  OldRawClose(fp);

  return 0;
}
