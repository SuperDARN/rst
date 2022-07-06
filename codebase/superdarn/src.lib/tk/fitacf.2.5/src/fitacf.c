/* fitacf.c
     ========
     Author: R.J.Barnes & K.Baker

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory


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
  E.G.Thomas 2021-08: added support for multi-channel tdiff values
  E.G.Thomas 2022-03: added support for tdiff calibration files
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include "rmath.h"
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "radar.h"
#include "rawdata.h"
#include "fitdata.h"
#include "fitblk.h"
#include "dofit.h"
#include "fitacfversion.h"

#define GOOSEBAY 1

void FitACFFree(struct FitBlock *fptr) {
    if (fptr->prm.pulse !=NULL) free(fptr->prm.pulse);
    if (fptr->prm.lag[0] !=NULL) free(fptr->prm.lag[0]);
    if (fptr->prm.lag[1] !=NULL) free(fptr->prm.lag[1]);
    if (fptr->acfd !=NULL) free(fptr->acfd);
    if (fptr->xcfd !=NULL) free(fptr->xcfd);
}


struct FitBlock *FitACFMake(struct RadarSite *hd, int year) {
    int i;
    struct FitBlock *fptr;

    fptr = malloc(sizeof(struct FitBlock));
    if (fptr==NULL) return NULL;

    if (year < 1993) fptr->prm.old=1; /* needed for old pulse seq. */
    for (i=0;i<3;i++) fptr->prm.interfer[i]=hd->interfer[i];
    fptr->prm.bmoff=hd->bmoff;
    fptr->prm.bmsep=hd->bmsep;
    fptr->prm.phidiff=hd->phidiff;
    fptr->prm.vdir=hd->vdir;
    fptr->prm.maxbeam=hd->maxbeam;
    fptr->prm.pulse=NULL;
    fptr->prm.lag[0]=NULL;
    fptr->prm.lag[1]=NULL;
    fptr->prm.pwr0=NULL;
    fptr->acfd=NULL;
    fptr->xcfd=NULL;
    return fptr;
}

int fill_fit_block(struct RadarParm *prm, struct RawData *raw,
                   struct FitBlock *input, struct FitData *fit,
                   struct RadarSite *hd){

    int i, j, n;
    void *tmp=NULL;

    input->prm.xcf=prm->xcf;
    input->prm.tfreq=prm->tfreq;
    input->prm.noise=prm->noise.search;
    input->prm.nrang=prm->nrang;
    input->prm.smsep=prm->smsep;
    input->prm.nave=prm->nave;
    input->prm.mplgs=prm->mplgs;
    input->prm.mpinc=prm->mpinc;
    input->prm.txpl=prm->txpl;
    input->prm.lagfr=prm->lagfr;
    input->prm.mppul=prm->mppul;
    input->prm.bmnum=prm->bmnum;
    input->prm.cp=prm->cp;
    input->prm.channel=prm->channel;
    input->prm.offset=prm->offset;  /* stereo offset */

    /* Check for multi-frequency data from a mono-channel radar */
    if ((input->prm.offset == 0) || (input->prm.channel < 2)) {
      input->prm.tdiff=hd->tdiff[0];
    } else {
      input->prm.tdiff=hd->tdiff[1];
    }

    /* need to incorporate Sessai's code for setting the offset
         for legacy data here.
    */
    if (input->prm.pulse==NULL) tmp=malloc(sizeof(int)*input->prm.mppul);
    else tmp=realloc(input->prm.pulse,sizeof(int)*input->prm.mppul);
    if (tmp==NULL) return -1;
    input->prm.pulse=tmp;
    for (i=0;i<input->prm.mppul;i++) input->prm.pulse[i]=prm->pulse[i];

    for (n=0;n<2;n++) {
        if (input->prm.lag[n]==NULL) tmp=malloc(sizeof(int)*(input->prm.mplgs+1));
        else tmp=realloc(input->prm.lag[n],sizeof(int)*(input->prm.mplgs+1));
        if (tmp==NULL) return -1;
        input->prm.lag[n]=tmp;
        for (i=0;i<=input->prm.mplgs;i++) input->prm.lag[n][i]=prm->lag[n][i];
    }



    if (input->prm.pwr0==NULL) tmp=malloc(sizeof(int)*input->prm.nrang);
    else tmp=realloc(input->prm.pwr0,sizeof(int)*input->prm.nrang); 
    if (tmp==NULL) return -1;
    input->prm.pwr0=tmp;

    if (input->acfd==NULL) tmp=malloc(sizeof(double complex)*input->prm.nrang*
                                                                        input->prm.mplgs);
    else tmp=realloc(input->acfd,sizeof(double complex)*input->prm.nrang*
                                                                     input->prm.mplgs); 
    if (tmp==NULL) return -1;
    input->acfd=tmp;

    if (input->xcfd==NULL) tmp=malloc(sizeof(double complex)*input->prm.nrang*
                                                                        input->prm.mplgs);
    else tmp=realloc(input->xcfd,sizeof(double complex)*input->prm.nrang*
                                                                     input->prm.mplgs);
    if (tmp==NULL) return -1;
    input->xcfd=tmp;

    memset(input->acfd,0,sizeof(double complex)*input->prm.nrang*
                                                                     input->prm.mplgs);
    memset(input->xcfd,0,sizeof(double complex)*input->prm.nrang*
                                                                     input->prm.mplgs);

    for (i=0;i<input->prm.nrang;i++) {
        input->prm.pwr0[i]=raw->pwr0[i];

        if (raw->acfd[0] !=NULL) {
            for (j=0;j<input->prm.mplgs;j++) {
                input->acfd[i*input->prm.mplgs+j] = CMPLX(raw->acfd[0][i*input->prm.mplgs+j], 
                        raw->acfd[1][i*input->prm.mplgs+j]);
            }
        }
        if (raw->xcfd[0] !=NULL) {
            for (j=0;j<input->prm.mplgs;j++) {
                input->xcfd[i*input->prm.mplgs+j] = CMPLX(raw->xcfd[0][i*input->prm.mplgs+j], 
                        raw->xcfd[1][i*input->prm.mplgs+j]);
            }
        }
    }

    return 0;
}
int FitACF(struct RadarParm *prm, struct RawData *raw, struct FitBlock *input,
           struct FitData *fit, struct RadarSite *hd, struct RadarTdiff *tdiff,
           double tdiff_fix) {

    int fnum, goose, s;

    if (prm->time.yr < 1993) {
        input->prm.old=1;
    }

    fit->revision.major=FITACF_MAJOR_REVISION;
    fit->revision.minor=FITACF_MINOR_REVISION;

    /*initialize the fitblock with prm*/
    s = fill_fit_block(prm, raw, input, fit, hd);
    if (s == -1){
        return -1;
    }

    /* Assign the tdiff value either from the hardware file,
     * calibration file, or user input (if no calibration value
     * available) */
    if (tdiff !=NULL) {
      input->prm.tdiff = tdiff->tdiff;
    } else if (tdiff_fix !=-999) {
      input->prm.tdiff = tdiff_fix;
    }
    fit->tdiff=input->prm.tdiff;

    FitSetRng(fit,input->prm.nrang);
    if (input->prm.xcf) {
     FitSetXrng(fit,input->prm.nrang);
     FitSetElv(fit,input->prm.nrang);
    }


    goose = (prm->stid == GOOSEBAY);

    fnum = do_fit(input, 5, goose, fit->rng, fit->xrng, fit->elv, &fit->noise);
    if (fnum == -1)
    {
        fprintf(stderr, "Error: do_fit returned an error\n");
        return -1;
    }
    return 0;
}
