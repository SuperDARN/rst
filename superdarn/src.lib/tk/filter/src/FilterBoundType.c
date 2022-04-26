/* FilterBoundType.c
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
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "scandata.h"
#include "cfitdata.h"
#include "cfitwrite.h"
#include "fitscan.h"
#include "filter.h"
#include "bound.h"

struct RadarScan src[3];
struct RadarScan dst;


struct RadarParm prm;
struct FitData fit;
struct CFitdata cfit;

int main(int argc,char *argv[]) {

  FILE *fp;
  int state=0;

  double min[4]={35,3,10,0};
  double max[4]={2000,50,1000,200};

  int yr,mo,dy,hr,mt;
  double sc;
 
  int tflg=1;

  int index=0,num=0;
  int mode=0,nbox=3;
  int n,r;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(FitFreadRadarScan(fp,&state,&src[index],&prm,&fit,0,0,0) !=-1) {
    TimeEpochToYMDHMS(src[index].st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

    FilterBoundType(&src[index],tflg);


    FilterBound(15,&src[index],min,max);


    if (num>2) {

      FilterRadarScan(mode,nbox,index,src,&dst,15);   

      for (n=0;n<dst.num;n++) {
        cfit.stid=dst.stid;
	cfit.time=dst.bm[n].time;
	cfit.bmnum=dst.bm[n].bm;
	cfit.cp=dst.bm[n].cpid;
        cfit.intt.sc=dst.bm[n].intt.sc;
        cfit.intt.us=dst.bm[n].intt.us;
	cfit.nave=dst.bm[n].nave;
	cfit.frang=dst.bm[n].frang;
	cfit.rsep=dst.bm[n].rsep;
	cfit.rxrise=dst.bm[n].rxrise;
	cfit.tfreq=dst.bm[n].freq;
	cfit.noise=dst.bm[n].noise;
	cfit.atten=dst.bm[n].atten;
	cfit.channel=dst.bm[n].channel;
	cfit.nrang=MAX_RANGE;
	if (n==0) cfit.scan=1;
	else cfit.scan=0;
	cfit.num=0;
	for (r=0;r<MAX_RANGE;r++) {
	  if (dst.bm[n].sct[r]==0) continue;
	  cfit.rng[cfit.num]=r;
	  cfit.data[cfit.num].gsct=dst.bm[n].rng[r].gsct;
	  cfit.data[cfit.num].p_0=dst.bm[n].rng[r].p_0;
	  cfit.data[cfit.num].p_0_e=dst.bm[n].rng[r].p_0_e;
	  cfit.data[cfit.num].v=dst.bm[n].rng[r].v;
	  cfit.data[cfit.num].v_e=dst.bm[n].rng[r].v_e;
	  cfit.data[cfit.num].p_l=dst.bm[n].rng[r].p_l;
	  cfit.data[cfit.num].p_l_e=dst.bm[n].rng[r].p_l_e;
	  cfit.data[cfit.num].w_l=dst.bm[n].rng[r].w_l;
	  cfit.data[cfit.num].w_l_e=dst.bm[n].rng[r].w_l_e;
	  cfit.num++;
	}
      }
      CFitFwrite(stdout,&cfit);      
    }
   
    index++;
    if (index>2) index=0;
    num++;

  
  }

  fclose(fp);


  return 0;
}
