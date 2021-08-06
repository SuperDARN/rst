/* cfitopen.c
   =========
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "cfitdata.h"
#include "cfitread.h"





struct CFitfp *CFitOpen(char *name) {
  struct CFitfp *fptr=NULL;

  fptr=malloc(sizeof(struct CFitfp));
  fptr->fp=gzdopen(open(name,O_RDONLY),"r");
  if (fptr->fp==NULL) {
    free(fptr);
    return NULL;
  }
  /* 
  fstat(fptr->fp,&cstat);
  fptr->fsze=cstat.st_size;
  */

  fptr->ctime=-1;
  fptr->blen=0;
  fptr->fptr=0;
  fptr->fsze=0;
  fptr->fbuf=NULL;

  /*  
  fptr->fbuf=malloc(fptr->fsze);
  
  if (fptr->fbuf !=NULL) {
    if (gzread(fptr->fp,fptr->fbuf,fptr->fsze) !=fptr->fsze) {
      gzclose(fptr->fp);  
      free(fptr->fbuf);
      free(fptr);
      return NULL;
    }
    gzclose(fptr->fp);
    fptr->fp=-1;
  }
  */

  return fptr;
} 



