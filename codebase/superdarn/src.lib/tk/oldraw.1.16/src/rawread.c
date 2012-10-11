/* rawread.c
   =========
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
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "lmt.h"
#include "raw.h"
#include "oldrawread.h"



int OldRawRead(struct OldRawFp *fp,struct RadarParm *prm,
               struct RawData *raw) { 
  if (fp==NULL) return -1;
  raw->thr=fp->thr;
  return (fp->rawread)(fp,prm,raw);
}
 
int OldRawReadDataraw(struct OldRawFp *fp,struct RadarParm *prm,
                      struct RawData *raw) {

 /* reads only the power and ACFs without altering the 
    radar parameters */

  struct RadarParm tprm;
  raw->thr=fp->thr;
  if ( (fp->rawread)(fp,&tprm,raw) !=0) return -1;
  prm->noise.mean=tprm.noise.mean;
  prm->tfreq=tprm.tfreq;
 
  return 0;
}




