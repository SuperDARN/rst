/* fitacfclientscan.c
   ============--====
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "connex.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "scandata.h"
#include "downloadscan.h"
#include "errstr.h"
#include "hlpstr.h"




struct RadarScan *rec;

struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;


  int sock;
  int remote_port=0;
  char host[256];
  struct RadarParm *prm;
  struct FitData *fit;
  

  prm=RadarParmMake();
  fit=FitMake();
  rec=RadarScanMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (argc-arg<2) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }
   
 
  prm->scan=0;
 
  strcpy(host,argv[argc-2]);
  remote_port=atoi(argv[argc-1]);

  sock=ConnexOpen(host,remote_port,NULL); 
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }

  do {
    downloadscan(sock,prm,fit,rec); 
  } while(1);

  return 0;
}
   

 










