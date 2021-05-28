/* extract_grid.c
   ==============
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"

#include "hlpstr.h"

struct GridData *rcd;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: extract_grid --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  
  int old=0;

  int arg;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char rflg=0;


  FILE *fp;   
  char *fname=NULL;
  double tme;
  int yr,mo,dy,hr,mt,eyr,emo,edy;
  double sc;
  int i;

  rcd=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old); 
  OptionAdd(&opt,"mid",'x',&rflg);

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

  if (arg<argc) fname=argv[arg];
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (old) {
    while (OldGridFread(fp,rcd) !=-1) {
      if (rflg==0) {
        tme=(rcd->st_time+rcd->ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
		       yr,mo,dy,hr,mt,(int) sc);
      } else {
        TimeEpochToYMDHMS(rcd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	        yr,mo,dy,hr,mt,(int) sc);
        TimeEpochToYMDHMS(rcd->ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
        if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
        fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
      }
    
      fprintf(stdout,"%d %d ",rcd->vcnum,rcd->stnum);
    
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].npnt);
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].st_id);
      fprintf(stdout,"\n");
    

    }
    if (fp !=stdout) fclose(fp);
  } else {
    while (GridFread(fp,rcd) !=-1) {
      if (rflg==0) {
        tme=(rcd->st_time+rcd->ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
		       yr,mo,dy,hr,mt,(int) sc);
      } else {
        TimeEpochToYMDHMS(rcd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	        yr,mo,dy,hr,mt,(int) sc);
        TimeEpochToYMDHMS(rcd->ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
        if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
        fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
      }
    
      fprintf(stdout,"%d %d ",rcd->vcnum,rcd->stnum);
    
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].npnt);
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].st_id);
      fprintf(stdout,"\n");
    

    }
    if (fp !=stdout) fclose(fp);
  }

  return 0;
}






















