/* aacgm.c
   =======
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
#include <string.h>
#include <math.h> 
#include "option.h"
#include "rtime.h"
#include "errstr.h"
#include "hlpstr.h"
#include "aacgm.h"



struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
 
  double ilon=0.0,ilat=0.0,alt=0.0;
  double olon=0.0,olat=0.0,r;
  char *fmt=NULL;
  char *dfmt="%f %f\n";
  char *fname=NULL;
  FILE *fp;

  unsigned char option=0;
  unsigned char help=0;
  unsigned char flag=0;
  int c;

  char txt[256];
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"i",'x',&flag);
  OptionAdd(&opt,"lon",'d',&ilon);
  OptionAdd(&opt,"lat",'d',&ilat);
  OptionAdd(&opt,"alt",'d',&alt);
  OptionAdd(&opt,"fmt",'t',&fmt);
  OptionAdd(&opt,"f",'t',&fname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (fmt==NULL) fmt=dfmt;

  if (fname==NULL) {
    AACGMConvert(ilat,ilon,alt,&olat,&olon,&r,flag);
    fprintf(stdout,fmt,olat,olon);    
  } else {
    if (strcmp(fname,"-")==0) fp=stdin;
    else fp=fopen(fname,"r");
    if (fp==NULL) exit(1);
    while(fgets(txt,255,fp) !=NULL) {
      for (c=0;(txt[c] !=0) && (c<256);c++) 
      if (txt[c]=='#') break;
      if (txt[c]=='#') continue;
      if (sscanf(txt,"%lf %lf %lf\n",
          &ilat,&ilon,&alt) !=3) continue;
      AACGMConvert(ilat,ilon,alt,&olat,&olon,&r,flag);
      fprintf(stdout,fmt,olat,olon);    
    }
  }
  return 0;
}
