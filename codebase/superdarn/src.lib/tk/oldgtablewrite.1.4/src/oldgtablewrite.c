/* oldgtablewrite.h
   ================
   Author: R.J.Barnes
*/


/*
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include "rtypes.h"
#include "rtime.h"
#include "radar.h"
#include "scandata.h"
#include "gtable.h"




#define NBLOCKS 2
#define NPARM1 18
#define NPARM2 8
#define NPARM3 12

int OldGridTableWrite(int fid,struct GridTable *ptr,char *logbuf,int xtd) {
  int i,s;
 
  int syr,smo,sdy,shr,smt,ssc;
  int eyr,emo,edy,ehr,emt,esc;
  char buf[256];
  double sec;

  char *name_blk1[]={"st_id","chn","nvec","freq0","major_rev","minor_rev",
                     "prog_id","noise_mean","noise_sd","gsct",
                     "v_min","v_max","p_min","p_max","w_min","w_max",
                     "ve_min","ve_max",0};
  char *unit_blk1[]={"count","count","count","KHz",
                     "count","count","count","count",
                     "count","count","m/sec","m/sec","dB","dB","m/sec",
                     "m/sec","m/sec","m/sec",0};
  char *type_blk1[]={"int","int","int","float","int",
                     "int","int","float","float",
                     "int","float","float","float","float","float","float",
                      "float","float",0};


  char *name_blk2[]={"gmlong","gmlat","kvect","st_id","chn","grid_index",
                   "vlos","vlos_sd",0};
  char *unit_blk2[]={"degrees","degrees","degrees","count","count",
                   "count","m/sec","m/sec",0};
  char *type_blk2[]={"float","float","float","int","int",
                     "int","float","float",0};


  char *name_blk3[]={"gmlong","gmlat","kvect","st_id","chn","grid_index",
                   "vlos","vlos_sd","pwr","pwr_sd","wdt","wdt_sd",0};
  char *unit_blk3[]={"degrees","degrees","degrees","count","count",
                   "count","m/sec","m/sec","dB","dB","m/sec","m/sec",0};
  char *type_blk3[]={"float","float","float","int","int","int","float","float",
                     "float","float","float","float",0};





  TimeEpochToYMDHMS(ptr->ed_time,&eyr,&emo,&edy,&ehr,&emt,&sec);
  esc=sec;
  TimeEpochToYMDHMS(ptr->st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
  ssc=sec;
 
  if (logbuf !=NULL) 
  sprintf(logbuf,"%d-%d-%d %d:%d:%d %d:%d:%d pnts=%d (%d)",
          syr,smo,sdy,shr,smt,ssc,ehr,emt,esc,ptr->npnt,ptr->st_id);

  sprintf(buf,"%d %d %d %d %d %d ", syr,smo,sdy,shr,smt,ssc);
  s=write(fid,buf,strlen(buf));

  sprintf(buf,"%d %d %d %d %d %d\n",eyr,emo,edy,ehr,emt,esc);
  s=write(fid,buf,strlen(buf));
  sprintf(buf,"%d\n",NBLOCKS);
  s=write(fid,buf,strlen(buf));

  sprintf(buf,"%d %d\n",1,NPARM1);
  s=write(fid,buf,strlen(buf));

  for (i=0;name_blk1[i] !=0;i++) {
    sprintf(buf,"%10s ",name_blk1[i]);
    s=write(fid,buf,strlen(buf));
  }
  sprintf(buf,"\n");
  s=write(fid,buf,strlen(buf));

 for (i=0;unit_blk1[i] !=0;i++) {
    sprintf(buf,"%10s ",unit_blk1[i]);
    s=write(fid,buf,strlen(buf));
  }
  sprintf(buf,"\n");
  s=write(fid,buf,strlen(buf));

 for (i=0;type_blk1[i] !=0;i++) {
    sprintf(buf,"%10s ",type_blk1[i]);
    s=write(fid,buf,strlen(buf));
  }
  sprintf(buf,"\n");
  s=write(fid,buf,strlen(buf));

 

  sprintf(buf,"%10d %10d %10d %#10g %10d %10d ",ptr->st_id,ptr->chn,
	  ptr->npnt,(double)
          ptr->freq,
          MAJOR_REVISION,MINOR_REVISION);
  s=write(fid,buf,strlen(buf));
  sprintf(buf,"%10d %#10g %#10g %10d ",ptr->prog_id,ptr->noise.mean,
          ptr->noise.sd,ptr->gsct);
  s=write(fid,buf,strlen(buf));
  sprintf(buf,"%#10g %#10g %#10g %#10g %#10g %#10g %#10g %#10g\n",
          ptr->min[0],ptr->max[0],
          ptr->min[1],ptr->max[1],
          ptr->min[2],ptr->max[2],
          ptr->min[3],ptr->max[3]);
  s=write(fid,buf,strlen(buf));

  if (xtd==0) {

    sprintf(buf,"%d %d\n",ptr->npnt,NPARM2);
    s=write(fid,buf,strlen(buf));
 
    for (i=0;name_blk2[i] !=0;i++) {
      sprintf(buf,"%10s ",name_blk2[i]);
      s=write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    s=write(fid,buf,strlen(buf));

   for (i=0;unit_blk2[i] !=0;i++) {
      sprintf(buf,"%10s ",unit_blk2[i]);
      s=write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    s=write(fid,buf,strlen(buf));

   for (i=0;type_blk2[i] !=0;i++) {
      sprintf(buf,"%10s ",type_blk2[i]);
      s=write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    s=write(fid,buf,strlen(buf));
  } else {


    sprintf(buf,"%d %d\n",ptr->npnt,NPARM3);
    s=write(fid,buf,strlen(buf));
 
    for (i=0;name_blk3[i] !=0;i++) {
      sprintf(buf,"%10s ",name_blk3[i]);
      s=write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    s=write(fid,buf,strlen(buf));

   for (i=0;unit_blk3[i] !=0;i++) {
      sprintf(buf,"%10s ",unit_blk3[i]);
      s=write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    s=write(fid,buf,strlen(buf));

   for (i=0;type_blk3[i] !=0;i++) {
      sprintf(buf,"%10s ",type_blk3[i]);
      s=write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    s=write(fid,buf,strlen(buf));
  }


 
  for (i=0;i<ptr->pnum;i++) {
    if (ptr->pnt[i].cnt==0) continue;
    if (xtd==0) sprintf(buf,"%#10g %#10g %#10g %10d %10d %10d %#10g %#10g\n",
            ptr->pnt[i].mlon,ptr->pnt[i].mlat,
            ptr->pnt[i].azm,ptr->st_id,ptr->chn,ptr->pnt[i].ref,
            ptr->pnt[i].vel.median,ptr->pnt[i].vel.sd);
    else sprintf(buf,
     "%#10g %#10g %#10g %10d %10d %10d %#10g %#10g %#10g %#10g %#10g %#10g\n",
            ptr->pnt[i].mlon,ptr->pnt[i].mlat,
            ptr->pnt[i].azm,ptr->st_id,ptr->chn,ptr->pnt[i].ref,
            ptr->pnt[i].vel.median,ptr->pnt[i].vel.sd,
            ptr->pnt[i].pwr.median,ptr->pnt[i].pwr.sd,
            ptr->pnt[i].wdt.median,ptr->pnt[i].wdt.sd);

    s=write(fid,buf,strlen(buf));
 
  }

  if (s == -1) {
    fprintf(stderr,"write failed.\n");
    return -1;
  }

  return 0;
} 

int OldGridTableFwrite(FILE *fp,struct GridTable *ptr,char *logbuf,int xtd) {
  return OldGridTableWrite(fileno(fp),ptr,logbuf,xtd);
}
