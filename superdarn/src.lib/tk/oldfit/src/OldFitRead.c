/* OldFitRead.c
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
#include "fitdata.h"
#include "oldfitread.h"

int main(int argc,char *argv[]) {
 
 

  struct RadarParm prm;
  struct FitData fit;

  struct OldFitFp *fp;

  if (argc==2)
    fp=OldFitOpen(argv[1],NULL);
  else
    fp=OldFitOpen(argv[1],argv[2]);


  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(OldFitRead(fp,&prm,&fit) !=-1) {
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
  }
 
  OldFitClose(fp);

  return 0;
}
