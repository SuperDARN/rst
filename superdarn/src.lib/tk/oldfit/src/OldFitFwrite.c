/* OldFitFwrite.c
   ==============
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitread.h"
#include "oldfitwrite.h"

int main(int argc,char *argv[]) {
  
  int drec=2,dnum;

  struct RadarParm prm;
  struct FitData fit;

  struct OldFitFp *fp;

  fp=OldFitOpen(argv[1],NULL);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  OldFitHeaderFwrite(stdout,"Demonstration Code","fitacf","4.00");

  while(OldFitRead(fp,&prm,&fit) !=-1) {
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);

    prm.cp=1000;
    dnum=OldFitFwrite(stdout,&prm,&fit,NULL);
    drec+=dnum;
  }
 
  OldFitClose(fp);

  return 0;
}
