/* fit_read_inx.c
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

/* This routine reads in and decodes a record from a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "lmt.h"
#include "raw.h"
#include "fit.h"

#include "fit_read.h"
#include "fit_str.h"



int fit_read_inx(struct fitfp *ptr,int32 *buffer,int recno) {
  int i,status=0;
  if (ptr->inxfp !=-1) {
     if (recno !=-1)  /* jump to this record */
       lseek(ptr->inxfp,(recno+1)*ptr->inx_recl,SEEK_SET);
     for (i=0;(i<4) && (status==0);i++) 
     status=ConvertReadInt(ptr->inxfp,&buffer[i]);
     return status;
  } else if (ptr->ibuf !=NULL) {
     if (recno !=-1)  ptr->iptr=(recno+1)*ptr->inx_recl;
     if ((ptr->istat.st_size-ptr->iptr)<(4*sizeof(int32))) return -1; 
     for (i=0;i<4;i++) 
        ConvertToInt(ptr->ibuf+ptr->iptr+sizeof(int32)*i,&buffer[i]);
     ptr->iptr+=sizeof(int32)*4;
     return 0;
  }
  return -1;
}


