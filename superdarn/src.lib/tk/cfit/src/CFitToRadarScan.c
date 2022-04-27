/* CFitToRadarScan.c
   =================
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
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"
#include "scandata.h"
#include "cfitscan.h"

struct RadarScan scn;


int main(int argc,char *argv[]) {
 
  struct CFitfp *fp=NULL;
  struct CFitdata cfit;
 
  int yr,mo,dy,hr,mt;
  double sc;

  fp=CFitOpen(argv[1]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(CFitRead(fp,&cfit) !=-1) {
    CFitToRadarScan(&scn,&cfit);
    if (cfit.scan !=1) continue;
    TimeEpochToYMDHMS(scn.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stdout,"%d\n",scn.num);
    RadarScanReset(&scn);

  }

  CFitClose(fp);

  return 0;
}
