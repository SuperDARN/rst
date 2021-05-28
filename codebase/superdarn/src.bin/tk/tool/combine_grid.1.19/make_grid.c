/* make_grid.c
   ===========
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"



int make_grid(struct GridData *out,
              struct GridData **in,int fnum,int rflg,int *dflg) {

  int i,j,k,l=0;
  int tpnt=0;
  struct GridGVec *data[64];
  int cnt[64];
  out->stnum=0;
  out->vcnum=0;
  out->xtd=0;

  l=0;
  for (i=0;i<fnum;i++)
    if ( (dflg[i] !=0) &&
         (in[i]->st_time<out->ed_time) &&
         (in[i]->ed_time>out->st_time)) {

    if (in[i]->xtd) out->xtd=1;

    for (j=0;j<in[i]->stnum;j++) {
      for (k=0;(k<out->stnum) && (in[i]->sdata[j].st_id !=
                                   out->sdata[k].st_id) &&
                                 (in[i]->sdata[j].chn !=
                                   out->sdata[k].chn);k++);
      if (k<out->stnum) {
        if (rflg !=0) {
          out->sdata[k].st_id=in[i]->sdata[j].st_id;
          out->sdata[k].chn=in[i]->sdata[j].chn;

          out->sdata[k].npnt=in[i]->sdata[j].npnt;
          out->sdata[k].freq0=in[i]->sdata[j].freq0;
          out->sdata[k].major_revision=in[i]->sdata[j].major_revision;
          out->sdata[k].minor_revision=in[i]->sdata[j].minor_revision;
          out->sdata[k].prog_id=in[i]->sdata[j].prog_id;
          out->sdata[k].noise.mean=in[i]->sdata[j].noise.mean;
          out->sdata[k].noise.sd=in[i]->sdata[j].noise.sd;
          out->sdata[k].gsct=in[i]->sdata[j].gsct;
          out->sdata[k].vel.min=in[i]->sdata[j].vel.min;
          out->sdata[k].vel.max=in[i]->sdata[j].vel.max;
          out->sdata[k].pwr.min=in[i]->sdata[j].pwr.min;
          out->sdata[k].pwr.max=in[i]->sdata[j].pwr.max;
          out->sdata[k].wdt.min=in[i]->sdata[j].wdt.min;
          out->sdata[k].wdt.max=in[i]->sdata[j].wdt.max;
          out->sdata[k].verr.min=in[i]->sdata[j].verr.min;
          out->sdata[k].verr.max=in[i]->sdata[j].verr.max;
          data[k]=in[i]->data;
          cnt[k]=in[i]->vcnum;
        }
        continue;
      }
      if (out->sdata !=NULL) out->sdata=realloc(out->sdata,
                                    sizeof(struct GridSVec)*(l+1));
      else out->sdata=malloc(sizeof(struct GridSVec));

      out->sdata[l].st_id=in[i]->sdata[j].st_id;
      out->sdata[l].chn=in[i]->sdata[j].chn;
      out->sdata[l].npnt=in[i]->sdata[j].npnt;
      out->sdata[l].freq0=in[i]->sdata[j].freq0;
      out->sdata[l].major_revision=in[i]->sdata[j].major_revision;
      out->sdata[l].minor_revision=in[i]->sdata[j].minor_revision;
      out->sdata[l].prog_id=in[i]->sdata[j].prog_id;
      out->sdata[l].noise.mean=in[i]->sdata[j].noise.mean;
      out->sdata[l].noise.sd=in[i]->sdata[j].noise.sd;
      out->sdata[l].gsct=in[i]->sdata[j].gsct;
      out->sdata[l].vel.min=in[i]->sdata[j].vel.min;
      out->sdata[l].vel.max=in[i]->sdata[j].vel.max;
      out->sdata[l].pwr.min=in[i]->sdata[j].pwr.min;
      out->sdata[l].pwr.max=in[i]->sdata[j].pwr.max;
      out->sdata[l].wdt.min=in[i]->sdata[j].wdt.min;
      out->sdata[l].wdt.max=in[i]->sdata[j].wdt.max;
      out->sdata[l].verr.min=in[i]->sdata[j].verr.min;
      out->sdata[l].verr.max=in[i]->sdata[j].verr.max;

      data[l]=in[i]->data;
      cnt[l]=in[i]->vcnum;

      l++;
    }
  }

  tpnt=0;
  for (i=0;i<l;i++) tpnt+=out->sdata[i].npnt;
  out->stnum=l;
  out->vcnum=tpnt;
  if (out->data !=NULL)
    out->data=realloc(out->data,sizeof(struct GridGVec)*tpnt);
  else out->data=malloc(sizeof(struct GridGVec)*tpnt);

  k=0;
  for (i=0;i<l;i++) {
    for (j=0;j<cnt[i];j++) {
      if (data[i][j].st_id !=out->sdata[i].st_id) continue;
      if (data[i][j].chn !=out->sdata[i].chn) continue;
      memcpy(&out->data[k],&data[i][j],sizeof(struct GridGVec));
      k++;
    }
  }

  return l;
}

