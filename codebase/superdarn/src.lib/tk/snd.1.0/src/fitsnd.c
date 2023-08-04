/* fitsnd.c
   ========
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
#include "rprm.h"
#include "fitdata.h"


int FitToSnd(struct SndData *ptr, struct RadarParm *prm,
             struct FitData *fit, int scan) {

  int c;

  ptr->radar_revision.major = prm->revision.major;
  ptr->radar_revision.minor = prm->revision.minor;
  ptr->cp = prm->cp;
  ptr->stid = prm->stid;
  ptr->time.yr = prm->time.yr;
  ptr->time.mo = prm->time.mo;
  ptr->time.dy = prm->time.dy;
  ptr->time.hr = prm->time.hr;
  ptr->time.mt = prm->time.mt;
  ptr->time.sc = prm->time.sc;
  ptr->time.us = prm->time.us;
  ptr->nave = prm->nave;
  ptr->lagfr = prm->lagfr;
  ptr->smsep = prm->smsep;
  ptr->noise.search = prm->noise.search;
  ptr->noise.mean = prm->noise.mean;
  ptr->channel = prm->channel;
  ptr->bmnum = prm->bmnum;
  ptr->bmazm = prm->bmazm;
  ptr->scan = scan;
  ptr->rxrise = prm->rxrise;
  ptr->intt.sc = prm->intt.sc;
  ptr->intt.us = prm->intt.us;
  ptr->nrang = prm->nrang;
  ptr->frang = prm->frang;
  ptr->rsep = prm->rsep;
  ptr->xcf = prm->xcf;
  ptr->tfreq = prm->tfreq;
  ptr->sky_noise = fit->noise.skynoise;
  ptr->fit_revision.major = fit->revision.major;
  ptr->fit_revision.minor = fit->revision.minor;
  ptr->snd_revision.major = SND_MAJOR_REVISION;
  ptr->snd_revision.minor = SND_MINOR_REVISION;

  SndSetRng(ptr,prm->nrang);

  for (c=0;c<prm->nrang;c++) {
    ptr->rng[c].qflg = fit->rng[c].qflg;
    ptr->rng[c].gsct = fit->rng[c].gsct;
    ptr->rng[c].v = fit->rng[c].v;
    ptr->rng[c].v_err = fit->rng[c].v_err;
    ptr->rng[c].p_l = fit->rng[c].p_l;
    ptr->rng[c].w_l = fit->rng[c].w_l;

    if ((prm->xcf) && (fit->xrng != NULL)) {
      ptr->rng[c].x_qflg = fit->xrng[c].qflg;
      ptr->rng[c].phi0 = fit->xrng[c].phi0;
      ptr->rng[c].phi0_err = fit->xrng[c].phi0_err;
    }
  }

  return 0;
}

