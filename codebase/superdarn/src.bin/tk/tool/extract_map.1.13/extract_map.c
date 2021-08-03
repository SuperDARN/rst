/* extract_map.c
   ============= 
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "hlpstr.h"



struct CnvMapData *map;
struct GridData *grd;

#define GRID 0x01
#define SCATTER 0x02
#define POTENTIAL 0x04
#define LATMIN 0x08

char *model_dir[]={
    "Bz+","Bz+/By+","By+","Bz-/By+","Bz-",
    "Bz-/By-","By-","Bz+/By-",0};

char *model_mag[]={"0<BT<4","4<BT<6","6<BT<12",0};

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: extract_map --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  
  FILE *fp;   
  char *fname=NULL;
  int tme;
  int yr,mo,dy,eyr,emo,edy,hr,mt;
  double sc;
  int i,j;

  
  int flg=GRID;
  unsigned char rflg=0;
  unsigned char sflg=0;
  unsigned char pflg=0;
  unsigned char lflg=0;

  grd=GridMake();
  map=CnvMapMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"mid",'x',&rflg);
  OptionAdd(&opt,"s",'x',&sflg);
  OptionAdd(&opt,"p",'x',&pflg);
  OptionAdd(&opt,"l",'x',&lflg);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  rflg=!rflg;
  if (arg !=argc) fname=argv[arg];
  if (sflg) flg=SCATTER;
  if (pflg) flg=POTENTIAL;
  if (lflg) flg=LATMIN;

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (old) {
    while (OldCnvMapFread(fp,map,grd)!=-1) {
    
      if (flg==GRID) {

        if (vb==1) {
         TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d-%d-%d %d:%d:%d-",
                 yr,mo,dy,hr,mt,(int) sc); 
         TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d:%d:%d pnts=%d (%d)\n",hr,mt,(int) sc,
		 grd->vcnum,grd->stnum); 
	}
        OldGridFwrite(stdout,grd);
      } else {
        if (rflg==0) {
         tme=(map->st_time+map->ed_time)/2.0;
         TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
        } else {
          TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
          TimeEpochToYMDHMS(map->ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
          if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
          fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
        }
        if (flg==SCATTER) {
          fprintf(stdout,"%d %d ",grd->vcnum,grd->stnum);
          for (i=0;i<grd->stnum;i++) fprintf(stdout,"%d ",
					      grd->sdata[i].npnt);
          for (i=0;i<grd->stnum;i++) fprintf(stdout,"%d ",
					      grd->sdata[i].st_id);
          fprintf(stdout,"\n");
        } else if (flg==POTENTIAL) {
          for (i=0;i<8;i++) if (strcmp(map->imf_model[1],model_dir[i])==0) 
                                break;
          for (j=0;j<3;j++) if (strcmp(map->imf_model[0],model_mag[j])==0)
                                 break;
          fprintf(stdout,"%g %g %g %g %d %d %d %d %g %g %g\n",
	    map->pot_drop/1000,map->Bx,map->By,map->Bz,
	    i,j,grd->vcnum,grd->stnum,map->chi_sqr,
            map->chi_sqr_dat,map->rms_err);
        } else if (flg==LATMIN) {
          fprintf(stdout,"%g\n",map->latmin);
        }
      }
    }


  } else {


     while (CnvMapFread(fp,map,grd)!=-1) {
    
      if (flg==GRID) {

        if (vb==1) {
         TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d-%d-%d %d:%d:%d-",
                 yr,mo,dy,hr,mt,(int) sc); 
         TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d:%d:%d pnts=%d (%d)\n",hr,mt,(int) sc,
		 grd->vcnum,grd->stnum); 
	}
        GridFwrite(stdout,grd);
      } else {
        if (rflg==0) {
         tme=(map->st_time+map->ed_time)/2.0;
         TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
        } else {
          TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
          TimeEpochToYMDHMS(map->ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
          if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
          fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
        }
        if (flg==SCATTER) {
          fprintf(stdout,"%d %d ",grd->vcnum,grd->stnum);
          for (i=0;i<grd->stnum;i++) fprintf(stdout,"%d ",
					      grd->sdata[i].npnt);
          for (i=0;i<grd->stnum;i++) fprintf(stdout,"%d ",
					      grd->sdata[i].st_id);
          fprintf(stdout,"\n");
        } else if (flg==POTENTIAL) {
          for (i=0;i<8;i++) if (strcmp(map->imf_model[1],model_dir[i])==0) 
                                break;
          for (j=0;j<3;j++) if (strcmp(map->imf_model[0],model_mag[j])==0)
                                 break;
          fprintf(stdout,"%g %g %g %g %d %d %d %d %g %g %g\n",
	    map->pot_drop/1000,map->Bx,map->By,map->Bz,
	    i,j,grd->vcnum,grd->stnum,map->chi_sqr,
            map->chi_sqr_dat,map->rms_err);
        } else if (flg==LATMIN) {
          fprintf(stdout,"%g\n",map->latmin);
        }
      }
    }
  }

  fclose(fp);
  return 0;
}






















