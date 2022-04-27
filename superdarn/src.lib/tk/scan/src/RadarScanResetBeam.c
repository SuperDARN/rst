/* RadarScanResetBeam.c
   ====================
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
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "scandata.h"
#include "fitscan.h"

struct RadarScan scn;


int main(int argc,char *argv[]) {
 
  FILE *fp=NULL;

  int bmnum=2;
  int bmtab[2]={5,8};

  struct RadarParm prm;
  struct FitData fit;
  int n;
  
  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  while(FitFread(fp,&prm,&fit) !=-1) {

    FitToRadarScan(&scn,&prm,&fit);
    if (prm.scan !=1) continue;
   
    fprintf(stdout,"Input:\n");
    for (n=0;n<scn.num;n++) fprintf(stdout,"%d\n",scn.bm[n].bm);

    RadarScanResetBeam(&scn,bmnum,bmtab);
 
    fprintf(stdout,"Output:\n");
    for (n=0;n<scn.num;n++) fprintf(stdout,"%d\n",scn.bm[n].bm);

   
    RadarScanReset(&scn);

  }

  return 0;
}
