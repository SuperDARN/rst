/* OldFitWrite.c
   ==============
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
#include "fitdata.h"
#include "oldfitread.h"
#include "oldfitwrite.h"

int main(int argc,char *argv[]) {
  
  int drec=2,dnum;

  struct RadarParm prm;
  struct FitData fit;

  struct OldFitFp *fp;
  int fd=0;

  fp=OldFitOpen(argv[1],NULL);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  fd=fileno(stdout);
  OldFitHeaderWrite(fd,"Demonstration Code","fitacf","4.00");

  while(OldFitRead(fp,&prm,&fit) !=-1) {
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);

    prm.cp=1000;
    dnum=OldFitWrite(fd,&prm,&fit,NULL);
    drec+=dnum;
  }
 
  OldFitClose(fp);

  return 0;
}
