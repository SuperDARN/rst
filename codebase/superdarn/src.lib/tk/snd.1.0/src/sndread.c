/* sndread.c
   =========
   Author: E.G.Thomas


   Copyright (C) <year>  <name of author>

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
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "snddata.h"


int SndDecode(struct DataMap *ptr, struct SndData *snd) {

  int c,x;
  int snum=0;
  int *slist=NULL;
  struct DataMapScalar *s;
  struct DataMapArray *a;

  if (snd->origin.time !=NULL) free(snd->origin.time);
  if (snd->origin.command !=NULL) free(snd->origin.command);
  if (snd->combf !=NULL) free(snd->combf);
  if (snd->rng !=NULL) free(snd->rng);

  memset(snd,0,sizeof(struct SndData));
  snd->origin.time=NULL;
  snd->origin.command=NULL;
  snd->combf=NULL;
  snd->rng=NULL;

  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];

    if ((strcmp(s->name,"radar.revision.major")==0) && (s->type==DATACHAR))
      snd->radar_revision.major=*(s->data.cptr);
    if ((strcmp(s->name,"radar.revision.minor")==0) && (s->type==DATACHAR))
      snd->radar_revision.minor=*(s->data.cptr);

    if ((strcmp(s->name,"origin.code")==0) && (s->type==DATACHAR))
      snd->origin.code=*(s->data.cptr);
    if ((strcmp(s->name,"origin.time")==0) && (s->type==DATASTRING))
      SndSetOriginTime(snd,*((char **) s->data.vptr));
    if ((strcmp(s->name,"origin.command")==0) && (s->type==DATASTRING))
      SndSetOriginCommand(snd,*((char **) s->data.vptr));

    if ((strcmp(s->name,"cp")==0) && (s->type==DATASHORT))
      snd->cp=*(s->data.sptr);
    if ((strcmp(s->name,"stid")==0) && (s->type==DATASHORT))
      snd->stid=*(s->data.sptr);
    if ((strcmp(s->name,"time.yr")==0) && (s->type==DATASHORT))
      snd->time.yr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mo")==0) && (s->type==DATASHORT))
      snd->time.mo=*(s->data.sptr);
    if ((strcmp(s->name,"time.dy")==0) && (s->type==DATASHORT))
      snd->time.dy=*(s->data.sptr);
    if ((strcmp(s->name,"time.hr")==0) && (s->type==DATASHORT))
      snd->time.hr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mt")==0) && (s->type==DATASHORT))
      snd->time.mt=*(s->data.sptr);
    if ((strcmp(s->name,"time.sc")==0) && (s->type==DATASHORT))
      snd->time.sc=*(s->data.sptr);
    if ((strcmp(s->name,"time.us")==0) && (s->type==DATAINT))
      snd->time.us=*(s->data.iptr);
    if ((strcmp(s->name,"nave")==0) && (s->type==DATASHORT))
      snd->nave=*(s->data.sptr);
    if ((strcmp(s->name,"lagfr")==0) && (s->type==DATASHORT))
      snd->lagfr=*(s->data.sptr);
    if ((strcmp(s->name,"smsep")==0) && (s->type==DATASHORT))
      snd->smsep=*(s->data.sptr);
    if ((strcmp(s->name,"noise.search")==0) && (s->type==DATAFLOAT))
      snd->noise.search=*(s->data.fptr);
    if ((strcmp(s->name,"noise.mean")==0) && (s->type==DATAFLOAT))
      snd->noise.mean=*(s->data.fptr);
    if ((strcmp(s->name,"channel")==0) && (s->type==DATASHORT))
      snd->channel=*(s->data.sptr);
    if ((strcmp(s->name,"bmnum")==0) && (s->type==DATASHORT))
      snd->bmnum=*(s->data.sptr);
    if ((strcmp(s->name,"bmazm")==0) && (s->type==DATAFLOAT))
      snd->bmazm=*(s->data.fptr);
    if ((strcmp(s->name,"scan")==0) && (s->type==DATASHORT))
      snd->scan=*(s->data.sptr);
    if ((strcmp(s->name,"rxrise")==0) && (s->type==DATASHORT))
      snd->rxrise=*(s->data.sptr);
    if ((strcmp(s->name,"intt.sc")==0) && (s->type==DATASHORT))
      snd->intt.sc=*(s->data.sptr);
    if ((strcmp(s->name,"intt.us")==0) && (s->type==DATAINT))
      snd->intt.us=*(s->data.iptr);
    if ((strcmp(s->name,"nrang")==0) && (s->type==DATASHORT))
      snd->nrang=*(s->data.sptr);
    if ((strcmp(s->name,"frang")==0) && (s->type==DATASHORT))
      snd->frang=*(s->data.sptr);
    if ((strcmp(s->name,"rsep")==0) && (s->type==DATASHORT))
      snd->rsep=*(s->data.sptr);
    if ((strcmp(s->name,"xcf")==0) && (s->type==DATASHORT))
      snd->xcf=*(s->data.sptr);
    if ((strcmp(s->name,"tfreq")==0) && (s->type==DATASHORT))
      snd->tfreq=*(s->data.sptr);
    if ((strcmp(s->name,"noise.sky")==0) && (s->type==DATAFLOAT))
      snd->sky_noise=*(s->data.fptr);
    if ((strcmp(s->name,"combf")==0) && (s->type==DATASTRING))
      SndSetCombf(snd,*((char **) s->data.vptr));
    if ((strcmp(s->name,"fitacf.revision.major")==0) && (s->type==DATAINT))
      snd->fit_revision.major=*(s->data.iptr);
    if ((strcmp(s->name,"fitacf.revision.minor")==0) && (s->type==DATAINT))
      snd->fit_revision.minor=*(s->data.iptr);
    if ((strcmp(s->name,"snd.revision.major")==0) && (s->type==DATASHORT))
      snd->snd_revision.major=*(s->data.sptr);
    if ((strcmp(s->name,"snd.revision.minor")==0) && (s->type==DATASHORT))
      snd->snd_revision.minor=*(s->data.sptr);
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
  }

  SndSetRng(snd,snd->nrang);

  for (c=0;c<ptr->anum;c++) {

    a=ptr->arr[c];

    if ((strcmp(a->name,"qflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].qflg=a->data.cptr[x];
    }

    if ((strcmp(a->name,"gflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].gsct=a->data.cptr[x];
    }

    if ((strcmp(a->name,"v")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].v=a->data.fptr[x];
    }

    if ((strcmp(a->name,"v_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].v_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"p_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].p_l=a->data.fptr[x];
    }

    if ((strcmp(a->name,"w_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].w_l=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_qflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].x_qflg=a->data.cptr[x];
    }

    if ((strcmp(a->name,"phi0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].phi0=a->data.fptr[x];
    }

    if ((strcmp(a->name,"phi0_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) snd->rng[slist[x]].phi0_err=a->data.fptr[x];
    }

  }

  if (slist !=NULL) free(slist);

  return 0;
}


int SndRead(int fid, struct SndData *snd) {

  int s;
  struct DataMap *ptr;

  ptr = DataMapRead(fid);
  if (ptr==NULL) return -1;

  s = SndDecode(ptr,snd);

  DataMapFree(ptr);

  return s;
}


int SndFread(FILE *fp, struct SndData *snd) {
  return SndRead(fileno(fp),snd);
}

