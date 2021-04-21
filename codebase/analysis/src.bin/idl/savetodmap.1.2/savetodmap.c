/* savetodmap.c
   ============ 
   Author R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include <stdint.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "dmap.h"
#include "idlsave.h"

#include "hlpstr.h"

struct OptionData opt;
int arg=0;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: savetodmap --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char zflg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  struct DataMap *dmap=NULL;
  struct IDLFile *fp=NULL;

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

  if (arg !=argc) fp=IDLOpen(argv[arg],zflg);
  else fp=IDLFdopen(fileno(stdin),zflg);

  dmap=IDLReadSave(fp);

  if (dmap==NULL) return 0;

  if (zflg) {
    gzFile gz=0;
    gz=gzdopen(fileno(stdout),"w");
    DataMapWriteZ(gz,dmap);
    gzclose(gz);
  } else DataMapFwrite(stdout,dmap);
  
  return 0;


}
