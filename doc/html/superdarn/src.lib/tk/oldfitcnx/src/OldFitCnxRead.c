/* OldFitCnxRead
   =============
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "connex.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitcnx.h"


struct RadarParm prm;
struct FitData fit;

int main(int argc,char *argv[]) {
  int sock;
  int port=0;
  char *host;
  int flag,status;

  host=argv[argc-2];
  port=atoi(argv[argc-1]);

  sock=ConnexOpen(host,port);
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
  do {
    status=OldFitCnxRead(1,&sock,&prm,&fit,&flag,NULL);
    if (status==-1) break;
    if (flag !=-1) fprintf(stdout,"%d-%d-%d %d:%d:%d %d %d\n",
              prm.time.yr,prm.time.mo,prm.time.dy,
	      prm.time.hr,prm.time.mt,prm.time.sc,prm.bmnum,prm.scan);

             

  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;
}
