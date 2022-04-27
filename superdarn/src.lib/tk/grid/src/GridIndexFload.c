/* GridIndexFload.c
   ================
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
#include "gridindex.h"
#include "gridseek.h"

struct GridData grd;

int main(int argc,char *argv[]) {
 
  FILE *fp;

  struct GridIndex *inx=NULL;

  int yr=2005,mo=3,dy=31,hr=10,mt=30;
  double sc=0,atme;
  int s;
  

  if (argc>2) {
     
    fp=fopen(argv[2],"r");

    if (fp==NULL) {
      fprintf(stderr,"Index not found.\n");
      exit(-1);
    }

    inx=GridIndexFload(fp);
  
    fclose(fp);
    if (inx==NULL) {
        fprintf(stderr,"Error loading index.\n");
        exit(-1);
    }
  }



  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  s=GridFseek(fp,yr,mo,dy,hr,mt,(int) sc,&atme,inx);
  if (s==-1) {
    fprintf(stderr,"file does not contain that interval.\n");
    exit(-1);
  }

  fprintf(stdout,"Requested:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  TimeEpochToYMDHMS(atme,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stdout,"Found:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);


  while(GridFread(fp,&grd) !=-1) {

    TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d\n",
            yr,mo,dy,hr,mt,(int) sc,grd.vcnum);


    
  }
 
  fclose(fp);


  return 0;
}
