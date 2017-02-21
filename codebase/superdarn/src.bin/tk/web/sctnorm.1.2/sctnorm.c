/* sct_norm.c
   ==========
   Author R.J.Barnes
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
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"


struct OptionData opt;

int max=14;
int table[256]={64,65,16,7,6,5,3,1,8,9,10,33,32,40};

int main(int argc,char *argv[]) {
  int arg=0;
  FILE *fp=NULL;
  char *idname=NULL;
  char *fname=NULL;

  int yr,mo,dy,hr,mt,sc;
  int npnts;
  int nradar;
  int r,i,s;
  int pnts[256];
  int id[256];
   
  unsigned char epoch=0;
  int eval; 


  OptionAdd(&opt,"e",'x',&epoch);
  OptionAdd(&opt,"id",'t',&idname);
  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if ((idname !=NULL) && ((fp=fopen(idname,"r")) !=NULL)) {
    s=fscanf(fp,"%d",&max);
    if (s !=1) exit(1);
    for (i=0;i<max;i++) {
      s=fscanf(fp,"%d",&table[i]);
      if (s !=1) break;
    }
    if (i !=max) exit(1);
    fclose(fp);
  }

  if (arg<argc) fname=argv[arg];
  if (fname !=NULL) fp=fopen(fname,"r");
  else fp=stdin;
  if (fp==NULL) exit(-1);

  while ( ((epoch==0) && 
          (fscanf(fp,"%d %d %d %d %d %d",&yr,&mo,&dy,&hr,&mt,&sc)==6)) ||
          ((epoch==1) && 
          (fscanf(fp,"%d",&eval)==1))) {
      fscanf(fp,"%d %d",&npnts,&nradar);
      if (epoch==0) 
         fprintf(stdout,"%d %d %d %d %d %d %d %d ",yr,mo,dy,hr,mt,sc,
	    npnts,max);
      else fprintf(stdout,"%d %d %d ",eval,npnts,max);
      for (r=0;r<nradar;r++) fscanf(fp,"%d",&pnts[r]);
      for (r=0;r<nradar;r++) fscanf(fp,"%d",&id[r]);
      for (r=0;r<max;r++) {
      for (i=0;(i<nradar) && (id[i] !=table[r]) ;i++);
      if (i<nradar) fprintf(stdout,"%d ",pnts[i]);
      else fprintf(stdout,"0 ");
    } 
    for (r=0;r<max;r++) fprintf(stdout,"%d ",table[r]);
    fprintf(stdout,"\n");
  }
  return 0;

}















