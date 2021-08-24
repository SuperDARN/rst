/* sndwrite.c
   ========== 
   Author E.G.Thomas
   
   
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
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "snddata.h"


int SndWrite(int fid, struct SndData *snd) {

  int s;
  struct DataMap *ptr=NULL;

  int c,x;
  int32 snum,xnum;

  int16 *slist=NULL;

  char *qflg=NULL;
  char *gflg=NULL;

  float *v=NULL;
  float *v_e=NULL;
  float *p_l=NULL;
  float *w_l=NULL;

  char *x_qflg=NULL;

  float *phi0=NULL;
  float *phi0_e=NULL;

  float sky_noise;

  ptr=DataMapMake();
  if (ptr==NULL) return -1;

  DataMapAddScalar(ptr,"radar.revision.major",DATACHAR,&snd->radar_revision.major);
  DataMapAddScalar(ptr,"radar.revision.minor",DATACHAR,&snd->radar_revision.minor);
  DataMapAddScalar(ptr,"origin.code",DATACHAR,&snd->origin.code);
  DataMapAddScalar(ptr,"origin.time",DATASTRING,&snd->origin.time);
  DataMapAddScalar(ptr,"origin.command",DATASTRING,&snd->origin.command);
  DataMapAddScalar(ptr,"cp",DATASHORT,&snd->cp);
  DataMapAddScalar(ptr,"stid",DATASHORT,&snd->stid);
  DataMapAddScalar(ptr,"time.yr",DATASHORT,&snd->time.yr);
  DataMapAddScalar(ptr,"time.mo",DATASHORT,&snd->time.mo);
  DataMapAddScalar(ptr,"time.dy",DATASHORT,&snd->time.dy);
  DataMapAddScalar(ptr,"time.hr",DATASHORT,&snd->time.hr);
  DataMapAddScalar(ptr,"time.mt",DATASHORT,&snd->time.mt);
  DataMapAddScalar(ptr,"time.sc",DATASHORT,&snd->time.sc);
  DataMapAddScalar(ptr,"time.us",DATAINT,&snd->time.us);
  DataMapAddScalar(ptr,"nave",DATASHORT,&snd->nave);
  DataMapAddScalar(ptr,"lagfr",DATASHORT,&snd->lagfr);
  DataMapAddScalar(ptr,"smsep",DATASHORT,&snd->smsep);
  DataMapAddScalar(ptr,"noise.search",DATAFLOAT,&snd->noise.search);
  DataMapAddScalar(ptr,"noise.mean",DATAFLOAT,&snd->noise.mean);

  DataMapAddScalar(ptr,"channel",DATASHORT,&snd->channel);
  DataMapAddScalar(ptr,"bmnum",DATASHORT,&snd->bmnum);
  DataMapAddScalar(ptr,"bmazm",DATAFLOAT,&snd->bmazm);

  DataMapAddScalar(ptr,"scan",DATASHORT,&snd->scan);
  DataMapAddScalar(ptr,"rxrise",DATASHORT,&snd->rxrise);
  DataMapAddScalar(ptr,"intt.sc",DATASHORT,&snd->intt.sc);
  DataMapAddScalar(ptr,"intt.us",DATAINT,&snd->intt.us);

  DataMapAddScalar(ptr,"nrang",DATASHORT,&snd->nrang);
  DataMapAddScalar(ptr,"frang",DATASHORT,&snd->frang);
  DataMapAddScalar(ptr,"rsep",DATASHORT,&snd->rsep);
  DataMapAddScalar(ptr,"xcf",DATASHORT,&snd->xcf);
  DataMapAddScalar(ptr,"tfreq",DATASHORT,&snd->tfreq);

  sky_noise=snd->sky_noise;
  DataMapStoreScalar(ptr,"noise.sky",DATAFLOAT,&sky_noise);

  DataMapAddScalar(ptr,"combf",DATASTRING,&snd->combf);

  DataMapAddScalar(ptr,"fitacf.revision.major",DATAINT,&snd->fit_revision.major);
  DataMapAddScalar(ptr,"fitacf.revision.minor",DATAINT,&snd->fit_revision.minor);

  DataMapAddScalar(ptr,"snd.revision.major",DATASHORT,&snd->snd_revision.major);
  DataMapAddScalar(ptr,"snd.revision.minor",DATASHORT,&snd->snd_revision.minor);

  snum=0;
  for (c=0;c<snd->nrang;c++) {
    if ( (snd->rng[c].qflg==1) || (snd->rng[c].x_qflg==1) ) snum++;
  }

  if (snd->xcf !=0) xnum=snum;
  else xnum=0;

  if (snum !=0) {

    slist=DataMapStoreArray(ptr,"slist",DATASHORT,1,&snum,NULL);

    qflg=DataMapStoreArray(ptr,"qflg",DATACHAR,1,&snum,NULL);
    gflg=DataMapStoreArray(ptr,"gflg",DATACHAR,1,&snum,NULL);

    v=DataMapStoreArray(ptr,"v",DATAFLOAT,1,&snum,NULL);
    v_e=DataMapStoreArray(ptr,"v_e",DATAFLOAT,1,&snum,NULL);
    p_l=DataMapStoreArray(ptr,"p_l",DATAFLOAT,1,&snum,NULL);
    w_l=DataMapStoreArray(ptr,"w_l",DATAFLOAT,1,&snum,NULL);

    if (snd->xcf !=0) {
      x_qflg=DataMapStoreArray(ptr,"x_qflg",DATACHAR,1,&xnum,NULL);

      phi0=DataMapStoreArray(ptr,"phi0",DATAFLOAT,1,&xnum,NULL);
      phi0_e=DataMapStoreArray(ptr,"phi0_e",DATAFLOAT,1,&xnum,NULL);
    }

    x=0;

    for (c=0;c<snd->nrang;c++) {
      if ( (snd->rng[c].qflg==1) || (snd->rng[c].x_qflg==1)) {
        slist[x]=c;

        qflg[x]=snd->rng[c].qflg;
        gflg[x]=snd->rng[c].gsct;

        p_l[x]=snd->rng[c].p_l;
        v[x]=snd->rng[c].v;
        v_e[x]=snd->rng[c].v_err;
        w_l[x]=snd->rng[c].w_l;

        if (xnum !=0) {
          x_qflg[x]=snd->rng[c].x_qflg;

          phi0[x]=snd->rng[c].phi0;
          phi0_e[x]=snd->rng[c].phi0_err;
        }
        x++;
      }
    }
  }

  if (fid !=-1) s=DataMapWrite(fid,ptr);
  else s=DataMapSize(ptr);

  DataMapFree(ptr);
  return s;

}


int SndFwrite(FILE *fp, struct SndData *snd) {
  return SndWrite(fileno(fp),snd);
}

