/* CFitSeek.c
   ===========
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

int main(int argc,char *argv[]) {
 
  struct CFitfp *fp=NULL;
  struct CFitdata cfit;

  int yr=2004,mo=11,dy=10,hr=10,mt=0;
  double sc=0,atme;

  int s;


  fp=CFitOpen(argv[1]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  s=CFitSeek(fp,yr,mo,dy,hr,mt,(int) sc,&atme);
  if (s==-1) {
    fprintf(stderr,"file does not contain that interval.\n");
    exit(-1);
  }

  fprintf(stdout,"Requested:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  TimeEpochToYMDHMS(atme,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stdout,"Found:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  while(CFitRead(fp,&cfit) !=-1) {
    
    TimeEpochToYMDHMS(cfit.time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d bmnum=%d pnts=%d\n",
            yr,mo,dy,hr,mt,(int) sc,cfit.bmnum,cfit.num);


   


  }
  CFitClose(fp);

  return 0;
}
