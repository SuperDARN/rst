/* OldFitCnxRead
   =============
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
#include "connex.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitcnx.h"


struct RadarParm prm;
struct FitData fit;

int main(int argc,char *argv[]) {
  int sock;
  int port=0;
  char *host;
  int flag,status;

  host=argv[argc-2];
  port=atoi(argv[argc-1]);

  sock=ConnexOpen(host,port);
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
  do {
    status=OldFitCnxRead(1,&sock,&prm,&fit,&flag,NULL);
    if (status==-1) break;
    if (flag !=-1) fprintf(stdout,"%d-%d-%d %d:%d:%d %d %d\n",
              prm.time.yr,prm.time.mo,prm.time.dy,
	      prm.time.hr,prm.time.mt,prm.time.sc,prm.bmnum,prm.scan);

             

  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;
}
