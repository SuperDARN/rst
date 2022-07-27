/* fitread.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "fitblk.h"
#include "fitdata.h"




int FitDecode(struct DataMap *ptr,
              struct FitData *fit) {
  int c,x;
  int snum=0;
  int *slist=NULL;
  struct DataMapScalar *s;
  struct DataMapArray *a;



  int qflg=0,xqflg=0;
  int xcf=0,nrang=0;

  if (fit->rng !=NULL) free(fit->rng);
  if (fit->xrng !=NULL) free(fit->xrng);
  if (fit->elv !=NULL) free(fit->elv);

  memset(fit,0,sizeof(struct FitData));
  fit->rng=NULL;
  fit->xrng=NULL;
  fit->elv=NULL;

  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];

    if ((strcmp(s->name,"algorithm")==0) && (s->type=DATASTRING))
      FitSetAlgorithm(fit,*((char **) s->data.vptr));

    if ((strcmp(s->name,"fitacf.revision.major")==0) && (s->type==DATAINT))
      fit->revision.major=*(s->data.iptr);
    if ((strcmp(s->name,"fitacf.revision.minor")==0) && (s->type==DATAINT))
      fit->revision.minor=*(s->data.iptr);

    if ((strcmp(s->name,"noise.sky")==0) && (s->type==DATAFLOAT))
      fit->noise.skynoise=*(s->data.fptr);
    if ((strcmp(s->name,"noise.lag0")==0) && (s->type==DATAFLOAT))
      fit->noise.lag0=*(s->data.fptr);
    if ((strcmp(s->name,"noise.vel")==0) && (s->type==DATAFLOAT))
      fit->noise.vel=*(s->data.fptr);
  }

  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];

    if ((strcmp(a->name,"slist")==0) && (a->type==DATASHORT) &&
        (a->dim==1)) {
      snum=a->rng[0];
      slist=malloc(sizeof(int)*snum);
      if (slist==NULL) break;
      for (x=0;x<snum;x++) slist[x]=a->data.sptr[x];
    }

    if ((strcmp(a->name,"pwr0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) nrang=a->rng[0];
    if ((strcmp(a->name,"x_v")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) xcf=1;
  }

  FitSetRng(fit,nrang);
  if (xcf) {
    FitSetXrng(fit,nrang);
    FitSetElv(fit,nrang);
  }

  for (c=0;c<ptr->anum;c++) {

    a=ptr->arr[c];


    if ((strcmp(a->name,"pwr0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[x].p_0=a->data.fptr[x];
    }


    if ((strcmp(a->name,"nlag")==0) && (a->type==DATASHORT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].nump=a->data.sptr[x];
    }

    if ((strcmp(a->name,"qflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      qflg=1;
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].qflg=a->data.cptr[x];
    }

    if ((strcmp(a->name,"gflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].gsct=a->data.cptr[x];
    }

    if ((strcmp(a->name,"p_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].p_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"p_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->rng[slist[x]].p_l_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"p_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].p_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"p_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->rng[slist[x]].p_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"v")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].v=a->data.fptr[x];
    }
    if ((strcmp(a->name,"v_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].v_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"w_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].w_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"w_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->rng[slist[x]].w_l_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"w_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].w_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"w_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->rng[slist[x]].w_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"sd_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->rng[slist[x]].sdev_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"sd_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->rng[slist[x]].sdev_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"sd_phi")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
	fit->rng[slist[x]].sdev_phi=a->data.fptr[x];
    }

    if (!xcf) continue;

    if ((strcmp(a->name,"x_qflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      xqflg=1;
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].qflg=a->data.cptr[x];
    }

    if ((strcmp(a->name,"x_gflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].gsct=a->data.cptr[x];
    }

    if ((strcmp(a->name,"x_p_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].p_l=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_p_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->xrng[slist[x]].p_l_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_p_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].p_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_p_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
         fit->xrng[slist[x]].p_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_v")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].v=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_v_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].v_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_w_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].w_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_w_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->xrng[slist[x]].w_l_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_w_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].w_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_w_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->xrng[slist[x]].w_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"phi0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].phi0=a->data.fptr[x];
    }

    if ((strcmp(a->name,"phi0_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->xrng[slist[x]].phi0_err=a->data.fptr[x];
    }


    if ((strcmp(a->name,"elv")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->elv[slist[x]].normal=a->data.fptr[x];
    }

    if ((strcmp(a->name,"elv_low")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->elv[slist[x]].low=a->data.fptr[x];
    }

    if ((strcmp(a->name,"elv_high")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->elv[slist[x]].high=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_sd_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->xrng[slist[x]].sdev_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_sd_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->xrng[slist[x]].sdev_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_sd_phi")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++)
        fit->xrng[slist[x]].sdev_phi=a->data.fptr[x];
    }

  }



  if (qflg==0) for (x=0;x<snum;x++) fit->rng[slist[x]].qflg=1;
  if ((xcf) && (xqflg==0)) for (x=0;x<snum;x++) fit->xrng[slist[x]].qflg=1;
  if (slist !=NULL) free(slist);

  return 0;

}


int FitRead(int fid,struct RadarParm *prm,
              struct FitData *fit) {

  int s;
  struct DataMap *ptr;

  ptr=DataMapRead(fid);
  if (ptr==NULL) return -1;
  s=RadarParmDecode(ptr,prm);
  if (s !=0) {
    DataMapFree(ptr);
    return s;
  }
  s=FitDecode(ptr,fit);
  DataMapFree(ptr);
  return s;

}


int FitFread(FILE *fp,struct RadarParm *prm,
              struct FitData *fit) {
  return FitRead(fileno(fp),prm,fit);
}

