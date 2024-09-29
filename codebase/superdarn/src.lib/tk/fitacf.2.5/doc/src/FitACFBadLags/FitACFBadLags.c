/* FitACFBadlags.c
   ===============
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
#include "rmath.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "fitblk.h"
#include "fitacf.h"
#include "badsmp.h"
#include "badlags.h"
#include "rawread.h"


struct RadarParm prm;
struct RawData raw;
struct FitBlock fblk;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;


struct FitACFBadSample bsamp;

int main(int argc,char *argv[]) {
 
  FILE *fp;
  char *envstr;
  int s,i,j,n;
 
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

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  s=RawFread(fp,&prm,&raw);

  if (s==-1) {
     fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }
    
  radar=RadarGetRadar(network,prm.stid);
  if (radar==NULL) {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
		          prm.time.dy,prm.time.hr,prm.time.mt,
                          prm.time.sc);

  if (site==NULL) {
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }

  FitACFStart(site,prm.time.yr,&fblk); 

  do {
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);

    
    fblk.prm.xcf=prm.xcf;
    fblk.prm.tfreq=prm.tfreq;
    fblk.prm.noise=prm.noise.search;
    fblk.prm.nrang=prm.nrang;
    fblk.prm.smsep=prm.smsep;
    fblk.prm.nave=prm.nave;
    fblk.prm.mplgs=prm.mplgs;
    fblk.prm.mpinc=prm.mpinc;
    fblk.prm.txpl=prm.txpl;
    fblk.prm.lagfr=prm.lagfr;
    fblk.prm.mppul=prm.mppul;
    fblk.prm.bmnum=prm.bmnum;
    fblk.prm.cp=prm.cp;
    fblk.prm.channel=prm.channel;
    fblk.prm.offset=prm.offset; /* stereo offset */

    for (i=0;i<fblk.prm.mppul;i++) fblk.prm.pulse[i]=prm.pulse[i];
    for (i=0;i<fblk.prm.mplgs;i++) {
      fblk.prm.lag[0][i]=prm.lag[i][0];
      fblk.prm.lag[1][i]=prm.lag[i][1];
    }

    for (i=0;i<fblk.prm.nrang;i++) {
      fblk.prm.pwr0[i]=raw.pwr0[i];
   
      for (j=0;j<fblk.prm.mplgs;j++) {
        fblk.acfd[i][j].x=raw.acfd[i][j][0];
        fblk.acfd[i][j].y=raw.acfd[i][j][1];
      }
      if (fblk.prm.xcf) {
        for (j=0;j<fblk.prm.mplgs;j++) {
          fblk.xcfd[i][j] = raw.xcfd[i][j][0] + raw.xcfd[i][j][1] * I;
        }
      } 
    }   

    FitACFBadlags(&fblk.prm,&bsamp);
    fprintf(stdout,"%d:",bsamp.nbad);
    for (n=0;n<bsamp.nbad;n++) fprintf(stdout,"%d ",bsamp.badsmp[n]);
    fprintf(stdout,"\n");
    s=RawFread(fp,&prm,&raw);


  } while (s !=-1);
 
  fclose(fp);


  return 0;
}
