/* RPosMagGS
   =========
   Author: R.J.Barnes

Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:


*/


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "radar.h"
#include "rpos.h"

struct RadarNetwork *network; 
struct RadarSite *site; 

int main(int argc,char *argv[]) {
  char *envstr;
  FILE *fp;
  char *code=NULL;
  int st;
  int bm,rng;
  int frang=180,rsep=45,rxrise=100.0;
  double hgt=150.0;
  double rho,lat,lon;

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
  
  code=argv[1];
  bm=atoi(argv[2]);
  rng=atoi(argv[3]);

  st=RadarGetID(network,code);

  site=RadarYMDHMSGetSite(RadarGetRadar(network,st),2002,8,30,10,30,0);

  RPosMagGS(1,bm,rng,site,frang,rsep,rxrise,hgt,&rho,&lat,&lon);

  fprintf(stdout,"RPosMagGS\n");
  fprintf(stdout,"%s bm=%d rng=%d\n",code,bm,rng);
  fprintf(stdout,"rho=%g lat=%g lon=%g\n",rho,lat,lon);



  return 0;
}
