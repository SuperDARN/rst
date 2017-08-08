/* RadarEpochGetSite
   =================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "radar.h"

struct RadarNetwork *network; 
struct RadarSite *site; 

int main(int argc,char *argv[]) {
  char *envstr;
  FILE *fp;
  char *code=NULL;
  double tval;
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

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);
  
  code=argv[argc-1];

  st=RadarGetID(network,code);

  tval=TimeYMDHMSToEpoch(2002,8,30,10,30,0);

  site=RadarEpochGetSite(RadarGetRadar(network,st),tval);

  fprintf(stdout,"RadarEpochGetSite\n");
  fprintf(stdout,"%s=%d\n",code,st);
  fprintf(stdout,"lat=%g lon=%g\n",site->geolat,site->geolon);



  return 0;
}
