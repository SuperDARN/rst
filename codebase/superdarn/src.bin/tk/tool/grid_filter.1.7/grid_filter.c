/* grid_filter.c
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
#include <math.h>
#include <sys/types.h>
#include "option.h"
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "hlpstr.h"





struct OptionData opt;
struct GridData *grd;
  
int skip=10*60; /* skip time */

int main(int argc,char *argv[]) {

 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */

  double tval=0,dval=0;
  int c=0;
 
  FILE *fp;
   
  int old=0;
  int new=0;


  int arg=0;
  unsigned char help=0;
  unsigned char option=0;

  grd=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);

 
  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (arg !=argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(1);
    }
  } else fp=stdin;
  
  if (old) {
    while (OldGridFread(fp,grd) !=-1)  {
    
      if (c==0) dval=grd->st_time-((int) grd->st_time % (24*3600));
      if ((grd->st_time-dval)>=tval) {
        OldGridFwrite(stdout,grd);
        tval+=skip;
      }
    }
    if (fp !=stdin) fclose(fp); 
  } else {
    while (GridFread(fp,grd) !=-1)  {
    
      if (c==0) dval=grd->st_time-((int) grd->st_time % (24*3600));
      if ((grd->st_time-dval)>=tval) {
        GridFwrite(stdout,grd);
        tval+=skip;
      }
    }
    if (fp !=stdin) fclose(fp); 
  }
  return 0;
}







