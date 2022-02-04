/* GridCopy.c
   ==========
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
#include "griddata.h"
#include "gridread.h"


struct GridData grd;
struct GridData ogrd;

int main(int argc,char *argv[]) {
 
  FILE *fp;

  int num=0;

  int yr,mo,dy,hr,mt;
  double sc;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(GridFread(fp,&grd) !=-1) {

    if (num>0) {
      TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
            yr,mo,dy,hr,mt,(int) sc);
      fprintf(stdout,"Difference in points:%d\n",grd.vcnum-ogrd.vcnum);

   }   
   GridCopy(&ogrd,&grd);
   num++;  

 
  }
 
  fclose(fp);


  return 0;
}
