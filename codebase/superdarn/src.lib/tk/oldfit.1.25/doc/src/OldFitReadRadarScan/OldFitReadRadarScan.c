/* OldFitOpen.c
   ============
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitread.h"
#include "scandata.h"
#include "oldfitscan.h"


struct RadarScan scn;

int main(int argc,char *argv[]) {
 
 

  struct RadarParm prm;
  struct FitData fit;

  struct OldFitFp *fp;

  int state=0;

  int yr,mo,dy,hr,mt;
  double sc;

  if (argc==2)
    fp=OldFitOpen(argv[1],NULL);
  else
    fp=OldFitOpen(argv[1],argv[2]);


  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  while(OldFitReadRadarScan(fp,&state,&scn,&prm,&fit,0,0,0) !=-1) {
    TimeEpochToYMDHMS(scn.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  }

 
 
  OldFitClose(fp);

  return 0;
}
