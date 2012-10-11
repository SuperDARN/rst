/* make_efieldinx.c
   ================
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
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;

int main (int argc,char *argv[]) {

  struct DataMap *ptr;
  struct DataMapScalar *s;
  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;
 
  FILE *fp;
  
  int sze=0,c,x;
  int yr,mo,dy,hr,mt;
  double sc,tval; 

  char *sname[]={"start.year","start.month","start.day","start.hour",
                 "start.minute","start.second",NULL};
  int stype[]={DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATADOUBLE};
  struct DataMapScalar *sdata[6];


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

    
  while ((ptr=DataMapFread(fp)) !=NULL) {
    
    for (c=0;sname[c] !=0;c++) sdata[c]=NULL;
    for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    for (x=0;sname[x] !=0;x++) 
      if ((strcmp(s->name,sname[x])==0) && (s->type==stype[x])) {
        sdata[x]=s;
        break;
      }
    }
    for (x=0;sname[x] !=0;x++) if (sdata[x]==NULL) break;
    if (sdata[x]==NULL) break;
   
    yr=*(sdata[0]->data.sptr);
    mo=*(sdata[1]->data.sptr);
    dy=*(sdata[2]->data.sptr);
    hr=*(sdata[3]->data.sptr);
    mt=*(sdata[4]->data.sptr);
    sc=*(sdata[5]->data.dptr);
    tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);


    if (vb) {
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
              yr,mo,dy,hr,mt,(int) sc);
    }
    ConvertFwriteDouble(stdout,tval);
    ConvertFwriteInt(stdout,sze);
    
    sze+=DataMapSize(ptr);
    DataMapFree(ptr);
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 






















