/* RadarGetStatus
   ==============
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "radar.h"

struct RadarNetwork *network; 

int main(int argc,char *argv[]) {
  char *envstr;
  FILE *fp;
  char *code=NULL;
  int status=0;
  int st;

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }
  
  code=argv[argc-1];

  st=RadarGetID(network,code);
  status=RadarGetStatus(network,st);

  fprintf(stdout,"RadarGetStatus\n");
  fprintf(stdout,"%s=%d\n",code,st);
  if (status==1) fprintf(stdout,"Operational\n");
  else if (status==0) fprintf(stdout,"Under Construction\n");
  else if (status==-1) fprintf(stdout,"Obsolete\n");

  return 0;
}
