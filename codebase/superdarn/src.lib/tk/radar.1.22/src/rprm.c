/* radar.c
   =======
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
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "radar.h"



struct RadarParm *RadarParmMake() {
  struct RadarParm *ptr=NULL;

  ptr=malloc(sizeof(struct RadarParm));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct RadarParm));
  ptr->origin.time=NULL;
  ptr->origin.command=NULL;
  ptr->pulse=NULL;
  ptr->lag[0]=NULL;
  ptr->lag[1]=NULL;
  ptr->combf=NULL;
  return ptr;
}

void RadarParmFree(struct RadarParm *ptr) {
  if (ptr==NULL) return;
  if (ptr->origin.time !=NULL) free(ptr->origin.time);
  if (ptr->origin.command !=NULL) free(ptr->origin.command);
  if (ptr->pulse !=NULL) free(ptr->pulse);
  if (ptr->lag[0] !=NULL) free(ptr->lag[0]);
  if (ptr->lag[1] !=NULL) free(ptr->lag[1]);
  if (ptr->combf !=NULL) free(ptr->combf);
  free(ptr);
}

int RadarParmSetOriginTime(struct RadarParm *ptr,char *str) {
  char *tmp=NULL;
  if (ptr==NULL) return -1;

  if (str==NULL) {
    if (ptr->origin.time !=NULL) free(ptr->origin.time);
    ptr->origin.time=NULL;
    return 0;
  }

  if (ptr->origin.time==NULL) tmp=malloc(strlen(str)+1);
  else tmp=realloc(ptr->origin.time,strlen(str)+1);

  if (tmp==NULL) return -1;
  strcpy(tmp,str);
  ptr->origin.time=tmp;
  return 0;

}



int RadarParmSetOriginCommand(struct RadarParm *ptr,char *str) {
  char *tmp=NULL;
  if (ptr==NULL) return -1;

  if (str==NULL) {
    if (ptr->origin.command !=NULL) free(ptr->origin.command);
    ptr->origin.command=NULL;
    return 0;
  }

  if (ptr->origin.command==NULL) tmp=malloc(strlen(str)+1);
  else tmp=realloc(ptr->origin.command,strlen(str)+1);

  if (tmp==NULL) return -1;
  strcpy(tmp,str);
  ptr->origin.command=tmp;
  return 0;

}

int RadarParmSetCombf(struct RadarParm *ptr,char *str) {
  void *tmp=NULL;
  if (ptr==NULL) return -1;

  if (str==NULL) {
    if (ptr->combf !=NULL) free(ptr->combf);
    ptr->combf=NULL;
    return 0;
  }

  if (ptr->combf==NULL) tmp=malloc(strlen(str)+1);
  else tmp=realloc(ptr->combf,strlen(str)+1);

  if (tmp==NULL) return -1;
  strcpy(tmp,str);
  ptr->combf=tmp;
  return 0;
}


int RadarParmSetPulse(struct RadarParm *ptr,int mppul,int16 *pulse) {
  void *tmp=NULL;
  if (ptr==NULL) return -1;

  if ((mppul==0) || (pulse==NULL)) {
    if (ptr->pulse !=NULL) free(ptr->pulse);
    ptr->pulse=NULL;
    ptr->mppul=0;
    return 0;
  }

  if (ptr->pulse==NULL) tmp=malloc(sizeof(int16)*mppul);
  else tmp=realloc(ptr->pulse,sizeof(int16)*mppul);

  if (tmp==NULL) return -1;
  memcpy(tmp,pulse,sizeof(int16)*mppul);
  ptr->pulse=tmp;
  ptr->mppul=mppul;
  return 0;
}

int RadarParmSetLag(struct RadarParm *ptr,int mplgs,int16 *lag) {
  int n,x;
  void *tmp=NULL;
  if (ptr==NULL) return -1;

  if ((mplgs==0) || (lag==NULL)) {
    for (n=0;n<2;n++) {
      if (ptr->lag[n] !=NULL) free(ptr->lag[n]);
      ptr->lag[n]=NULL;
    }
    return 0;
  }

  for (n=0;n<2;n++) {
    if (ptr->lag[n]==NULL) tmp=malloc(sizeof(int16)*(mplgs+1));
    else tmp=realloc(ptr->lag[n],sizeof(int16)*(mplgs+1));
    if (tmp==NULL) return -1;
    ptr->lag[n]=tmp;
    for (x=0;x<=mplgs;x++) ptr->lag[n][x]=lag[2*x+n];
  }
  return 0;
}






int RadarParmDecode(struct DataMap *ptr,struct RadarParm *prm) {

  int n,c;
  struct DataMapScalar *s;
  struct DataMapArray *a;

  if (ptr==NULL) return -1;
  if (prm==NULL) return -2;

  if (prm->origin.time !=NULL) free(prm->origin.time);
  if (prm->origin.command !=NULL) free(prm->origin.command);
  if (prm->pulse !=NULL) free(prm->pulse);
  for (n=0;n<2;n++) if (prm->lag[n] !=NULL) free(prm->lag[n]);
  if (prm->combf !=NULL) free(prm->combf);

  memset(prm,0,sizeof(struct RadarParm));
  prm->origin.time=NULL;
  prm->origin.command=NULL;
  prm->pulse=NULL;
  prm->lag[0]=NULL;
  prm->lag[1]=NULL;
  prm->combf=NULL;

  prm->ifmode=-1;

  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];

    if ((strcmp(s->name,"radar.revision.major")==0) && (s->type==DATACHAR))
      prm->revision.major=*(s->data.cptr);
    if ((strcmp(s->name,"radar.revision.minor")==0) && (s->type==DATACHAR))
      prm->revision.minor=*(s->data.cptr);

    if ((strcmp(s->name,"origin.code")==0) && (s->type==DATACHAR))
      prm->origin.code=*(s->data.cptr);

    if ((strcmp(s->name,"origin.time")==0) && (s->type==DATASTRING))
      RadarParmSetOriginTime(prm,*((char **) s->data.vptr));

    if ((strcmp(s->name,"origin.command")==0) && (s->type==DATASTRING))
      RadarParmSetOriginCommand(prm,*((char **) s->data.vptr));

    if ((strcmp(s->name,"cp")==0) && (s->type==DATASHORT))
      prm->cp=*(s->data.sptr);
    if ((strcmp(s->name,"stid")==0) && (s->type==DATASHORT))
      prm->stid=*(s->data.sptr);
    if ((strcmp(s->name,"time.yr")==0) && (s->type==DATASHORT))
      prm->time.yr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mo")==0) && (s->type==DATASHORT))
      prm->time.mo=*(s->data.sptr);
    if ((strcmp(s->name,"time.dy")==0) && (s->type==DATASHORT))
      prm->time.dy=*(s->data.sptr);
    if ((strcmp(s->name,"time.hr")==0) && (s->type==DATASHORT))
      prm->time.hr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mt")==0) && (s->type==DATASHORT))
      prm->time.mt=*(s->data.sptr);
    if ((strcmp(s->name,"time.sc")==0) && (s->type==DATASHORT))
      prm->time.sc=*(s->data.sptr);
    if ((strcmp(s->name,"time.us")==0) && (s->type==DATAINT))
      prm->time.us=*(s->data.iptr);
    if ((strcmp(s->name,"txpow")==0) && (s->type==DATASHORT))
      prm->txpow=*(s->data.sptr);
    if ((strcmp(s->name,"nave")==0) && (s->type==DATASHORT))
      prm->nave=*(s->data.sptr);
    if ((strcmp(s->name,"atten")==0) && (s->type==DATASHORT))
      prm->atten=*(s->data.sptr);
    if ((strcmp(s->name,"lagfr")==0) && (s->type==DATASHORT))
      prm->lagfr=*(s->data.sptr);
    if ((strcmp(s->name,"smsep")==0) && (s->type==DATASHORT))
      prm->smsep=*(s->data.sptr);
    if ((strcmp(s->name,"ercod")==0) && (s->type==DATASHORT))
      prm->ercod=*(s->data.sptr);
    if ((strcmp(s->name,"stat.agc")==0) && (s->type==DATASHORT))
      prm->stat.agc=*(s->data.sptr);
    if ((strcmp(s->name,"stat.lopwr")==0) && (s->type==DATASHORT))
      prm->stat.lopwr=*(s->data.sptr);
    if ((strcmp(s->name,"noise.search")==0) && (s->type==DATAFLOAT))
      prm->noise.search=*(s->data.fptr);
    if ((strcmp(s->name,"noise.mean")==0) && (s->type==DATAFLOAT))
      prm->noise.mean=*(s->data.fptr);
    if ((strcmp(s->name,"channel")==0) && (s->type==DATASHORT))
      prm->channel=*(s->data.sptr);
    if ((strcmp(s->name,"bmnum")==0) && (s->type==DATASHORT))
      prm->bmnum=*(s->data.sptr);
    if ((strcmp(s->name,"bmazm")==0) && (s->type==DATAFLOAT))
      prm->bmazm=*(s->data.fptr);
    if ((strcmp(s->name,"scan")==0) && (s->type==DATASHORT))
      prm->scan=*(s->data.sptr);
    if ((strcmp(s->name,"offset")==0) && (s->type==DATASHORT))
      prm->offset=*(s->data.sptr);
    if ((strcmp(s->name,"rxrise")==0) && (s->type==DATASHORT))
      prm->rxrise=*(s->data.sptr);
    if ((strcmp(s->name,"intt.sc")==0) && (s->type==DATASHORT))
      prm->intt.sc=*(s->data.sptr);
    if ((strcmp(s->name,"intt.us")==0) && (s->type==DATAINT))
      prm->intt.us=*(s->data.iptr);
    if ((strcmp(s->name,"txpl")==0) && (s->type==DATASHORT))
      prm->txpl=*(s->data.sptr);
    if ((strcmp(s->name,"mpinc")==0) && (s->type==DATASHORT))
      prm->mpinc=*(s->data.sptr);
    if ((strcmp(s->name,"mppul")==0) && (s->type==DATASHORT))
      prm->mppul=*(s->data.sptr);
    if ((strcmp(s->name,"mplgs")==0) && (s->type==DATASHORT))
      prm->mplgs=*(s->data.sptr);
    if ((strcmp(s->name,"mplgexs")==0) && (s->type==DATASHORT))
      prm->mplgexs=*(s->data.sptr);
    if ((strcmp(s->name,"ifmode")==0) && (s->type==DATASHORT))
      prm->ifmode=*(s->data.sptr);
    if ((strcmp(s->name,"nrang")==0) && (s->type==DATASHORT))
      prm->nrang=*(s->data.sptr);
    if ((strcmp(s->name,"frang")==0) && (s->type==DATASHORT))
      prm->frang=*(s->data.sptr);
    if ((strcmp(s->name,"rsep")==0) && (s->type==DATASHORT))
      prm->rsep=*(s->data.sptr);
    if ((strcmp(s->name,"xcf")==0) && (s->type==DATASHORT))
      prm->xcf=*(s->data.sptr);
    if ((strcmp(s->name,"tfreq")==0) && (s->type==DATASHORT))
      prm->tfreq=*(s->data.sptr);
    if ((strcmp(s->name,"mxpwr")==0) && (s->type==DATAINT))
      prm->mxpwr=*(s->data.iptr);
    if ((strcmp(s->name,"lvmax")==0) && (s->type==DATAINT))
      prm->lvmax=*(s->data.sptr);

    if ((strcmp(s->name,"combf")==0) && (s->type==DATASTRING))
      RadarParmSetCombf(prm,*((char **) s->data.vptr));

  }

  for (c=0;c<ptr->anum;c++) {
   a=ptr->arr[c];
   if ((strcmp(a->name,"ptab")==0) && (a->type==DATASHORT) &&
       (a->dim==1)) RadarParmSetPulse(prm,a->rng[0],a->data.sptr);

   if ((strcmp(a->name,"ltab")==0) && (a->type==DATASHORT) &&
        (a->dim==2)) RadarParmSetLag(prm,a->rng[1]-1,a->data.sptr);
  }

  return 0;
}

int RadarParmEncode(struct DataMap *ptr,struct RadarParm *prm) {

  int n,x;
  int16 *pulse=NULL,*lag=NULL;
  int32 pnum;
  int32 lnum[2];

  pnum=prm->mppul;
  lnum[0]=2;
  if (prm->mplgexs !=0) lnum[1]=prm->mplgexs+1;
  else lnum[1]=prm->mplgs+1;

  DataMapAddScalar(ptr,"radar.revision.major",DATACHAR,&prm->revision.major);
  DataMapAddScalar(ptr,"radar.revision.minor",DATACHAR,&prm->revision.minor);
  DataMapAddScalar(ptr,"origin.code",DATACHAR,&prm->origin.code);
  DataMapAddScalar(ptr,"origin.time",DATASTRING,&prm->origin.time);
  DataMapAddScalar(ptr,"origin.command",DATASTRING,&prm->origin.command);
  DataMapAddScalar(ptr,"cp",DATASHORT,&prm->cp);
  DataMapAddScalar(ptr,"stid",DATASHORT,&prm->stid);
  DataMapAddScalar(ptr,"time.yr",DATASHORT,&prm->time.yr);
  DataMapAddScalar(ptr,"time.mo",DATASHORT,&prm->time.mo);
  DataMapAddScalar(ptr,"time.dy",DATASHORT,&prm->time.dy);
  DataMapAddScalar(ptr,"time.hr",DATASHORT,&prm->time.hr);
  DataMapAddScalar(ptr,"time.mt",DATASHORT,&prm->time.mt);
  DataMapAddScalar(ptr,"time.sc",DATASHORT,&prm->time.sc);
  DataMapAddScalar(ptr,"time.us",DATAINT,&prm->time.us);
  DataMapAddScalar(ptr,"txpow",DATASHORT,&prm->txpow);
  DataMapAddScalar(ptr,"nave",DATASHORT,&prm->nave);
  DataMapAddScalar(ptr,"atten",DATASHORT,&prm->atten);
  DataMapAddScalar(ptr,"lagfr",DATASHORT,&prm->lagfr);
  DataMapAddScalar(ptr,"smsep",DATASHORT,&prm->smsep);
  DataMapAddScalar(ptr,"ercod",DATASHORT,&prm->ercod);
  DataMapAddScalar(ptr,"stat.agc",DATASHORT,&prm->stat.agc);
  DataMapAddScalar(ptr,"stat.lopwr",DATASHORT,&prm->stat.lopwr);
  DataMapAddScalar(ptr,"noise.search",DATAFLOAT,&prm->noise.search);
  DataMapAddScalar(ptr,"noise.mean",DATAFLOAT,&prm->noise.mean);

  DataMapAddScalar(ptr,"channel",DATASHORT,&prm->channel);
  DataMapAddScalar(ptr,"bmnum",DATASHORT,&prm->bmnum);
  DataMapAddScalar(ptr,"bmazm",DATAFLOAT,&prm->bmazm);

  DataMapAddScalar(ptr,"scan",DATASHORT,&prm->scan);
  DataMapAddScalar(ptr,"offset",DATASHORT,&prm->offset);
  DataMapAddScalar(ptr,"rxrise",DATASHORT,&prm->rxrise);
  DataMapAddScalar(ptr,"intt.sc",DATASHORT,&prm->intt.sc);
  DataMapAddScalar(ptr,"intt.us",DATAINT,&prm->intt.us);

  DataMapAddScalar(ptr,"txpl",DATASHORT,&prm->txpl);
  DataMapAddScalar(ptr,"mpinc",DATASHORT,&prm->mpinc);
  DataMapAddScalar(ptr,"mppul",DATASHORT,&prm->mppul);
  DataMapAddScalar(ptr,"mplgs",DATASHORT,&prm->mplgs);
  DataMapAddScalar(ptr,"mplgexs",DATASHORT,&prm->mplgexs);
  DataMapAddScalar(ptr,"ifmode",DATASHORT,&prm->ifmode);

  DataMapAddScalar(ptr,"nrang",DATASHORT,&prm->nrang);
  DataMapAddScalar(ptr,"frang",DATASHORT,&prm->frang);
  DataMapAddScalar(ptr,"rsep",DATASHORT,&prm->rsep);
  DataMapAddScalar(ptr,"xcf",DATASHORT,&prm->xcf);
  DataMapAddScalar(ptr,"tfreq",DATASHORT,&prm->tfreq);

  DataMapAddScalar(ptr,"mxpwr",DATAINT,&prm->mxpwr);
  DataMapAddScalar(ptr,"lvmax",DATAINT,&prm->lvmax);

  DataMapAddScalar(ptr,"combf",DATASTRING,&prm->combf);

  pulse=(int16 *) DataMapStoreArray(ptr,"ptab",DATASHORT,1,&pnum,NULL);
  if (pulse==NULL) return -1;
  lag=(int16 *) DataMapStoreArray(ptr,"ltab",DATASHORT,2,lnum,NULL);
  if (lag==NULL) return -1;

  for (n=0;n<prm->mppul;n++) pulse[n]=prm->pulse[n];
  if (prm->mplgexs !=0) {
    for (n=0;n<2;n++) {
      for (x=0;x<=prm->mplgexs;x++) lag[2*x+n]=prm->lag[n][x];
    }
  } else {
    for (n=0;n<2;n++) {
      for (x=0;x<=prm->mplgs;x++) lag[2*x+n]=prm->lag[n][x];
    }
  }
  return 0;
}


void *RadarParmFlatten(struct RadarParm *ptr,size_t *size) {
  size_t s;
  int n,lnum;
  struct RadarParm *r;
  void *buf=NULL;
  size_t p=0;

  if (size==NULL) return NULL;

  s=sizeof(struct RadarParm);
  if (ptr->mplgexs==0) lnum=ptr->mplgs+1;
  else lnum=ptr->mplgexs+1;

  if (ptr->origin.time !=NULL) s+=strlen(ptr->origin.time)+1;
  if (ptr->origin.command !=NULL) s+=strlen(ptr->origin.command)+1;
  if (ptr->combf !=NULL) s+=strlen(ptr->combf)+1;
  if (ptr->pulse !=NULL) s+=ptr->mppul*sizeof(int16);
  if (ptr->lag[0] !=NULL) s+=(lnum)*sizeof(int16);
  if (ptr->lag[1] !=NULL) s+=(lnum)*sizeof(int16);

  buf=malloc(s);
  if (buf==NULL) return NULL;
  *size=s;

  r=(struct RadarParm *) buf;
  memcpy(buf,ptr,sizeof(struct RadarParm));
  p=sizeof(struct RadarParm);

  if (ptr->origin.time !=NULL) {
    strcpy(buf+p,ptr->origin.time);
    r->origin.time=(void *) p;
    p+=strlen(ptr->origin.time)+1;
  }

  if (ptr->origin.command !=NULL) {
    strcpy(buf+p,ptr->origin.command);
    r->origin.command=(void *) p;
    p+=strlen(ptr->origin.command)+1;
  }

  if (ptr->combf !=NULL) {
    strcpy(buf+p,ptr->combf);
    r->combf=(void *) p;
    p+=strlen(ptr->combf)+1;
  }

  if (ptr->pulse !=NULL) {
    memcpy(buf+p,ptr->pulse,ptr->mppul*sizeof(int16));
    r->pulse=(void *) p;
    p+=ptr->mppul*sizeof(int16);
  }

  for (n=0;n<2;n++) {
    if (ptr->lag[n]==NULL) continue;
    memcpy(buf+p,ptr->lag[n],lnum*sizeof(int16));
    r->lag[n]=(void *) p;
    p+=lnum*sizeof(int16);
  }

  return buf;
}

int RadarParmExpand(struct RadarParm *ptr,void *buffer) {
  void *p;
  int n,lnum;
  if (ptr==NULL) return -1;
  if (buffer==NULL) return -1;

  if (ptr->origin.time !=NULL) free(ptr->origin.time);
  if (ptr->origin.command !=NULL) free(ptr->origin.command);
  if (ptr->pulse !=NULL) free(ptr->pulse);
  if (ptr->lag[0] !=NULL) free(ptr->lag[0]);
  if (ptr->lag[1] !=NULL) free(ptr->lag[1]);
  if (ptr->combf !=NULL) free(ptr->combf);

  memcpy(ptr,buffer,sizeof(struct RadarParm));
  if (ptr->origin.time !=NULL) {
    p=buffer+(size_t) ptr->origin.time;
    ptr->origin.time=malloc(strlen(p)+1);
    strcpy(ptr->origin.time,p);
  }

  if (ptr->origin.command !=NULL) {
    p=buffer+(size_t) ptr->origin.command;
    ptr->origin.command=malloc(strlen(p)+1);
    strcpy(ptr->origin.command,p);
  }

  if (ptr->combf !=NULL) {
    p=buffer+(size_t) ptr->combf;
    ptr->combf=malloc(strlen(p)+1);
    strcpy(ptr->combf,p);
  }

  if (ptr->pulse !=NULL) {
    p=buffer+(size_t) ptr->pulse;
    ptr->pulse=malloc(ptr->mppul*sizeof(int16));
    memcpy(ptr->pulse,p,ptr->mppul*sizeof(int16));
  }

  for (n=0;n<2;n++) {
    if (ptr->lag[n]==NULL) continue;
    if (ptr->mplgexs !=0) lnum=ptr->mplgexs+1;
    else lnum=ptr->mplgs+1;
    p=buffer+(size_t) ptr->lag[n];
    ptr->lag[n]=malloc(lnum*sizeof(int16));
    memcpy(ptr->lag[n],p,lnum*sizeof(int16));
  }


  return 0;
}

