/* maptocnvmap.c
   =============
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"

#include "oldcnvmapread.h"
#include "cnvmapwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;
struct GridData *grd;
struct CnvMapData *map;

int main (int argc,char *argv[]) {

  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  FILE *fp;
  
  int yr,mo,dy,hr,mt;
  double sc; 
  int s;

  grd=GridMake();
  map=CnvMapMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

   if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
    
  while (OldCnvMapFread(fp,map,grd) !=-1) {
   if (vb) {
      TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
              yr,mo,dy,hr,mt,(int) sc);
    }
    s=CnvMapFwrite(stdout,map,grd);
    if (s <=0) {
        fprintf(stderr,"CnvMapFwrite failed.\n");
        if (fp !=stdin) fclose(fp);
        exit(-1);
    }
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 

