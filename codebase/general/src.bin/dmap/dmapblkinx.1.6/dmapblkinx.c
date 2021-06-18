/* dmapblkinx.c
   ============
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "dmap.h"
#include "hlpstr.h"




struct OptionData opt;
int arg=0;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: dmapblkinx --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  struct DataMap *ptr;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  unsigned char zflg=0;


  FILE *fp=NULL;
  gzFile zfp=0;

  int sze=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"z",'x',&zflg);

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

  if (arg !=argc) {
    if (zflg) {
      zfp=gzopen(argv[arg],"r");
      if (zfp==0) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    } else {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    }  
  } else {
    if (zflg) zfp=gzdopen(fileno(stdin),"r");
    else fp=stdin; 
  }


  sze=0;   
  while (1) {

    if (zflg) ptr=DataMapReadZ(zfp);
    else ptr=DataMapFread(fp);

    if (ptr==NULL) break;

    ConvertFwriteInt(stdout,sze);
    sze+=DataMapSize(ptr);
    DataMapFree(ptr);

  }
  if (zflg) gzclose(zfp);
  else fclose(fp);
  return 0;  


}
