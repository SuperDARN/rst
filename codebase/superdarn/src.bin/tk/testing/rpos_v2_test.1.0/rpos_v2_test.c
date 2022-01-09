
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "option.h"
#include "radar.h"
#include "rpos_v2.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: rpos_v2_test --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  FILE *fp;
  char *envstr;

  int yr=2020;
  int mo=1;
  int dy=1;

  int stid=1;
  int bm=15;
  int rn=20;
  double height=300;
  int frang=180;
  int rsep=45;
  int rx=0;
  int model=0;
  int gs=0;
  int rear=0;
  double frho,flat,flon;

  int standard=0;
  int chisham=0;
  int midlat=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  
  OptionAdd(&opt,"stid",'i',&stid);
  OptionAdd(&opt,"bm",'i',&bm);
  OptionAdd(&opt,"rn",'i',&rn);
  OptionAdd(&opt,"fh",'d',&height);

  OptionAdd(&opt,"standard",'x',&standard);
  OptionAdd(&opt,"chisham",'x',&chisham);
  OptionAdd(&opt,"midlat",'x',&midlat);

  OptionAdd(&opt,"gs",'x',&gs);
  OptionAdd(&opt,"rear",'x',&rear);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

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
  radar=RadarGetRadar(network,stid);
  site=RadarYMDHMSGetSite(radar,yr,mo,dy,0,0,0);

  if (standard) model=0;
  else if (chisham) model=1;
  else if (midlat) model=2;

  RPosGeo_v2(1,bm,rn,site,frang,rsep,rx,height,&frho,&flat,&flon,model,gs,rear);

  fprintf(stderr,"flat: %lf, flon: %lf\n",flat,flon);

  exit(0);
}
