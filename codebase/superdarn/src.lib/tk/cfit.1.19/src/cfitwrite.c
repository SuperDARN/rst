/* cfitwrite.c
   ==========
   Author: R.J.Barnes
 
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
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "cfitdata.h"
#include "cfitread.h"

int CFitWrite(gzFile fp,struct CFitdata *ptr) {
  int i=0;
  unsigned char gsct;
  if (ConvertWriteIntZ(fp,ptr->version.major) !=0) return -1;
  if (ConvertWriteIntZ(fp,ptr->version.minor) !=0) return -1;
  if (ConvertWriteDoubleZ(fp,ptr->time) !=0) return -1; 
  if (ConvertWriteShortZ(fp,ptr->stid) !=0) return -1; 
  if (ConvertWriteShortZ(fp,ptr->scan) !=0) return -1; 
  if (ConvertWriteShortZ(fp,ptr->cp) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->bmnum) !=0) return -1;
  if (ConvertWriteFloatZ(fp,ptr->bmazm) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->channel) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->intt.sc) !=0) return -1;
  if (ConvertWriteIntZ(fp,ptr->intt.us) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->frang) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->rsep) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->rxrise) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->tfreq) !=0) return -1;
  if (ConvertWriteFloatZ(fp,ptr->noise) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->atten) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->nave) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->nrang) !=0) return -1;
  if (ConvertWriteShortZ(fp,ptr->num) !=0) return -1;
  for (i=0;i<ptr->num;i++) {
    if (ConvertWriteShortZ(fp,ptr->rng[i]) !=0) break;
  }
  if (i !=ptr->num) return -1;

  for (i=0;i<ptr->num;i++) {
    gsct=ptr->data[i].gsct;
    if (gzwrite(fp,&gsct,1) !=1) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].p_0) !=0) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].p_0_e) !=0) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].v) !=0) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].p_l) !=0) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].w_l) !=0) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].v_e) !=0) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].p_l_e) !=0) break;
    if (ConvertWriteFloatZ(fp,ptr->data[i].w_l_e) !=0) break;
  }
  if (i !=ptr->num) return -1;
  return 0;
}


