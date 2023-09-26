 /*COPYRIGHT:
TODO: Find author and check with VT if we can change this to GPL or LGPL
   Copyright (C) 2011 by Virginia Tech

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


 MODIFICATION HISTORY:
 Written by AJ Ribeiro 06/16/2011
 Based on code orginally written by Pasha Ponomarenko

 E.G.Thomas 2022-08: modified to use FITACF files as input and follow RST conventions
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <sys/time.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rmath.h"
#include "dmap.h"
#include "sim_data.h"
#include "rtypes.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "fitread.h"
#include "rawwrite.h"
#include "radar.h"
#include "iq.h"
#include "iqwrite.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm *prm;
struct FitData *fit;
struct RadarParm *prm2;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: sim_real --help\n");
  return(-1);
}

void makeRadarParm2(struct RadarParm *prm2, char *argv[], int argc, int cpid,
                    int nave, double smsep, double amp0, int n_samples, double dt,
                    int n_pul, int n_lags, int *pulse_t, struct RadarParm *in_prm)
{
  int i;
  time_t rawtime;
  struct tm *timeinfo;
  char tmstr[40];

  rawtime = time((time_t) 0);
  timeinfo = gmtime(&rawtime);

  strcpy(tmstr,asctime(timeinfo));
  tmstr[24]=0;

  if (prm2->origin.time !=NULL) free(prm2->origin.time);
  if (prm2->origin.command !=NULL) free(prm2->origin.command);
  if (prm2->pulse !=NULL) free(prm2->pulse);
  for (i=0;i<2;i++) if (prm2->lag[i] !=NULL) free(prm2->lag[i]);

  memset(prm2,0,sizeof(struct RadarParm));
  prm2->origin.time=NULL;
  prm2->origin.command=NULL;
  prm2->pulse=NULL;
  prm2->lag[0]=NULL;
  prm2->lag[1]=NULL;
  prm2->combf=NULL;

  prm2->revision.major = 1;
  prm2->revision.minor = 0;
  // set to 1 as it is not produced on site 
  prm2->origin.code = 1;

  RadarParmSetOriginTime(prm2,tmstr);
  char *tempstr = malloc(argc*15);
  strcpy(tempstr,argv[0]);
  for(i=1;i<argc;i++) {
    strcat(tempstr," ");
    strcat(tempstr,argv[i]);
  }
  RadarParmSetOriginCommand(prm2,tempstr);

  prm2->cp = (int16)cpid;
  prm2->stid = in_prm->stid;

  prm2->time.yr = in_prm->time.yr;
  prm2->time.mo = in_prm->time.mo;
  prm2->time.dy = in_prm->time.dy;
  prm2->time.hr = in_prm->time.hr;
  prm2->time.mt = in_prm->time.mt;
  prm2->time.sc = in_prm->time.sc;
  prm2->time.us = in_prm->time.us;
  prm2->bmnum = in_prm->bmnum;

  prm2->txpow = in_prm->txpow;
  prm2->nave = in_prm->nave;
  prm2->atten = in_prm->atten;
  prm2->lagfr = in_prm->lagfr;
  prm2->smsep = in_prm->smsep;
  prm2->ercod = in_prm->ercod;
  prm2->stat.agc = in_prm->stat.agc;
  prm2->stat.lopwr = in_prm->stat.lopwr;
  prm2->noise.search = in_prm->noise.search;
  prm2->noise.mean = in_prm->noise.mean;
  prm2->channel = in_prm->channel;
  prm2->bmazm = in_prm->bmazm;
  prm2->scan = in_prm->scan;
  prm2->rxrise = in_prm->rxrise;
  prm2->intt.sc = (int16)(smsep*n_samples*nave);
  prm2->intt.us = (int)(((smsep*n_samples*nave)-(int)(smsep*n_samples*nave))*1e6);
  prm2->txpl = 300;
  prm2->mpinc = (int16)(dt*1e6);
  prm2->mppul = (int16)n_pul;
  prm2->mplgs = (int16)n_lags;
  prm2->mplgexs = (int16)n_lags;
  prm2->nrang = in_prm->nrang;
  prm2->frang = in_prm->frang;
  prm2->rsep = in_prm->rsep;
  prm2->xcf = 0;
  prm2->tfreq = in_prm->tfreq;
  prm2->offset = in_prm->offset;
  prm2->mxpwr = in_prm->mxpwr;
  prm2->lvmax = in_prm->lvmax;

  int16 temp_pul[n_pul];

  for (i=0;i<n_pul;i++)
    temp_pul[i] = (int16)pulse_t[i];

  RadarParmSetPulse(prm2,n_pul,temp_pul);


  if (cpid == 1) {
    int16 temp_lag[100] = {0,0,26,27,20,22,9,12,22,26,22,27,20,26,20,27,12,20,0,9,
                                12,22,9,20,0,12,9,22,12,26,12,27,9,26,9,27};
    RadarParmSetLag(prm2,n_lags,temp_lag);
  } else if (cpid == 503) {
    int16 temp_lag[100] = {0,0,15,16,27,29,29,32,23,27,27,32,23,29,16,23,15,23,
                            23,32,16,27,15,27,16,29,15,29,32,47,16,32,15,32};
    RadarParmSetLag(prm2,n_lags,temp_lag);
  } else {
    int16 temp_lag[100] = {0,0,42,43,22,24,24,27,27,31,22,27,24,31,14,22,22,
                                31,14,24,31,42,31,43,14,27,0,14,27,42,27,43,14,31,
                                24,42,24,43,22,42,22,43,0,22,0,24};
    RadarParmSetLag(prm2,n_lags,temp_lag);
  }

  RadarParmSetCombf(prm2,"sim_real");

  free(tempstr);
}
/*this is a driver program for the data simulator*/

int main(int argc,char *argv[])
{

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  FILE *fp=NULL;

  int katscan = 0;
  int oldscan = 0;
  int tauscan = 0;

  /********************************************************
  ** definitions of variables needed for data generation **
  ********************************************************/
  double t_d = .04;                         /*Irregualrity decay time s*/
  double w = -9999.;                        /*spectral width*/
  double v_dop = 450.;                      /*Background velocity (m/s)*/
  double freq = 12.e6;                      /*transmit frequency*/
  double amp0 = 1.;                         /*amplitude scaling factor*/
  int noise_flg = 1;                        /*flag to indicate whether white noise is included*/
  double noise_lev = 0.;                    /*white noise level (ratio)*/
  int nave = 50;                            /*number of pulse sequences in an integration period*/
  int nrang = 100;                          /*number of range gates*/
  int lagfr = 4;                            /*lag to first range*/
  int life_dist = 0;                        /*lifetime distribution*/
  double smsep = 300.e-6;                   /*sample spearation*/
  int cpid = 150;                           /*control program ID number*/
  int n_samples;                            /*Number of datapoints in a single pulse sequence*/
  int n_pul,n_lags,*pulse_t,*tau;
  double dt;                                /*basic lag time*/
  int cri_flg = 0;                          /*cross-range interference flag*/
  int smp_flg = 0;                          /*output raw samples flag*/
  int decayflg = 0;

  /*other variables*/
  long i,j;
  double taus;
  double lambda;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"katscan",'x',&katscan);       /* control program */
  OptionAdd(&opt,"oldscan",'x',&oldscan);
  OptionAdd(&opt,"tauscan",'x',&tauscan);

  OptionAdd(&opt,"constant",'x',&life_dist);    /* irregularity distribution */
  OptionAdd(&opt,"nocri",'x',&cri_flg);         /* remove cross-range interference */
  OptionAdd(&opt,"iq",'x',&smp_flg);            /* output raw samples (iqdat) */

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  /*read the first radar's file*/
  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  /*fill the parameter structure*/
  prm = RadarParmMake();
  fit = FitMake();
  prm2 = RadarParmMake();

  cri_flg = !cri_flg;

  /*array with the irregularity decay time for each range gate*/
  double *t_d_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    t_d_arr[i] = 0;

  /*array with the irregularity growth time for each range gate*/
  double *t_g_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    t_g_arr[i] = 1.e-6;

  /*array with the irregularity lifetime for each range gate*/
  double *t_c_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    t_c_arr[i] = 1000;

  /*array with the irregularity doppler velocity for each range gate*/
  double *v_dop_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    v_dop_arr[i] = 0;

  /*array with the irregularity doppler velocity for each range gate*/
  double *velo_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    velo_arr[i] = 0;

  /*array with the ACF amplitude for each range gate*/
  double *amp0_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    amp0_arr[i] = 0;

  /*flags to tell which range gates contain scatter*/
  int *qflg = malloc(nrang*sizeof(int));
  for (i=0;i<nrang;i++)
    qflg[i] = 0;

  /*Creating the output array for ACFs*/
  complex double **acfs = malloc(nrang*sizeof(complex double *));


  while (FitFread(fp,prm,fit) !=-1) {

    if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
                    prm->time.yr,prm->time.mo,prm->time.dy,
                    prm->time.hr,prm->time.mt,prm->time.sc);

    nrang = prm->nrang;
    lagfr = prm->lagfr/prm->smsep;
    smsep = prm->smsep*1.e-6;
    dt = prm->mpinc*1.e-6;
    freq = prm->tfreq*1.e3;
    nave = prm->nave;
    noise_lev = prm->noise.search;

    lambda = C/freq;

    /*oldscan*/
    if (oldscan) {
      cpid = 1;
      n_pul = 7;                            /*number of pulses*/
      n_lags = 18;                          /*number of lags in the ACFs*/

      /*fill the pulse table*/
      pulse_t = malloc(n_pul*sizeof(int));
      pulse_t[0] = 0;
      pulse_t[1] = 9;
      pulse_t[2] = 12;
      pulse_t[3] = 20;
      pulse_t[4] = 22;
      pulse_t[5] = 26;
      pulse_t[6] = 27;

      /*Creating lag array*/
      tau = malloc(n_lags*sizeof(int));
      for (i=0;i<n_lags;i++)
        tau[i] = i;
      /*no lag 16*/
      tau[16] += 1;
      tau[17] += 1;
    }
    /*tauscan*/
    else if (tauscan) {
      cpid = 503;
      n_pul = 13;                           /*number of pulses*/
      n_lags = 17;                          /*number of lags in the ACFs*/

      /*fill the pulse table*/
      pulse_t = malloc(n_pul*sizeof(int));
      pulse_t[0] = 0;
      pulse_t[1] = 15;
      pulse_t[2] = 16;
      pulse_t[3] = 23;
      pulse_t[4] = 27;
      pulse_t[5] = 29;
      pulse_t[6] = 32;
      pulse_t[7] = 47;
      pulse_t[8] = 50;
      pulse_t[9] = 52;
      pulse_t[10] = 56;
      pulse_t[11] = 63;
      pulse_t[12] = 64;

      /*Creating lag array*/
      tau = malloc(n_lags*sizeof(int));
      for (i=0;i<10;i++)
        tau[i] = i;
      /*no lag 10*/
      for (i=10;i<n_lags;i++)
        tau[i] = (i+1);
    }
    /*katscan (default)*/
    else {
      cpid = 150;
      n_pul = 8;                            /*number of pulses*/
      n_lags = 23;                          /*number of lags in the ACFs*/

      /*fill the pulse table*/
      pulse_t = malloc(n_pul*sizeof(int));
      pulse_t[0] = 0;
      pulse_t[1] = 14;
      pulse_t[2] = 22;
      pulse_t[3] = 24;
      pulse_t[4] = 27;
      pulse_t[5] = 31;
      pulse_t[6] = 42;
      pulse_t[7] = 43;
      /*Creating lag array*/
      tau = malloc(n_lags*sizeof(int));
      for (i=0;i<6;i++)
        tau[i] = i;
      /*no lag 6*/
      for (i=6;i<22;i++)
        tau[i] = (i+1);
      /*no lag 23*/
      tau[22] = 24;
    }


    /*control program dependent variables*/
    taus = dt/smsep;                                      /*lag time in samples*/
    n_samples = (pulse_t[n_pul-1]*taus+nrang+lagfr);      /*number of samples in 1 pulse sequence*/

    /*create a structure to store the raw samples from each pulse sequence*/
    complex double *raw_samples = malloc(n_samples*nave*sizeof(complex double));
    makeRadarParm2(prm2, argv, argc, cpid, nave, smsep, amp0, n_samples,
                   dt, n_pul, n_lags, pulse_t, prm);

    /**********************************************************
    ****FILL THESE ARRAYS WITH THE SIMULATION PARAMETERS*******
    **********************************************************/
    for (i=0;i<prm->nrang;i++) {

      qflg[i] = fit->rng[i].qflg;

      t_d = lambda/(fit->rng[i].w_l*2.*PI);
      if (t_d > 999999.) t_d = 0.;
      t_d_arr[i] = t_d;

      v_dop_arr[i] = fit->rng[i].v;

      amp0 = noise_lev*pow(10.,(fit->rng[i].p_0/10.));
      amp0_arr[i] = amp0;

      acfs[i] = malloc(n_lags*sizeof(complex double));
      for (j=0;j<n_lags;j++)
        acfs[i][j] = 0.+I*0.;
    }

    /*call the simulation function*/
    sim_data(t_d_arr, t_g_arr, t_c_arr, v_dop_arr, qflg, velo_arr, amp0_arr, freq, noise_lev,
             noise_flg, nave, nrang, lagfr, smsep, cpid, life_dist,
             n_pul, cri_flg, n_lags, pulse_t, tau, dt, raw_samples, acfs, decayflg);

    if (!smp_flg) {
      /*fill the rawdata structure*/
      struct RawData *raw;
      raw = RawMake();

      raw->revision.major = 1;
      raw->revision.minor = 1;
      raw->thr=0.0;
      int *slist = malloc(nrang*sizeof(int));
      float *pwr0 = malloc(nrang*sizeof(float));
      float *acfd = malloc(nrang*n_lags*2*sizeof(float));
      float *xcfd = malloc(nrang*n_lags*2*sizeof(float));
      for (i=0;i<nrang;i++) {
        slist[i] = i;
        pwr0[i] = creal(acfs[i][0]);
        for (j=0;j<n_lags;j++) {
          acfd[i*n_lags*2+j*2] = creal(acfs[i][j]);
          acfd[i*n_lags*2+j*2+1] = cimag(acfs[i][j]);
          xcfd[i*n_lags*2+j*2] = 0.;
          xcfd[i*n_lags*2+j*2+1] = 0.;
        }
      }

      RawSetPwr(raw,nrang,pwr0,nrang,slist);
      RawSetACF(raw,nrang,n_lags,acfd,nrang,slist);
      RawSetXCF(raw,nrang,n_lags,xcfd,nrang,slist);
      i=RawFwrite(stdout,prm2,raw);
      free(slist);
      free(pwr0);
      free(acfd);
      free(xcfd);
    } else {
      /*fill the iqdata structure*/
      struct IQ *iq;
      iq=IQMake();

      struct timespec seqtval[nave];
      int seqatten[nave];
      float seqnoise[nave];
      int seqoff[nave];
      int seqsze[nave];

      iq->seqnum = nave;
      iq->chnnum = 2;
      iq->smpnum = n_samples;
      iq->skpnum = 4;

      int16 *samples = malloc(n_samples*nave*2*2*sizeof(int16));
      for (i=0;i<nave;i++) {
        /*iq structure values*/
        seqtval[i].tv_sec = prm->time.sc + (int)(i*n_samples*smsep);
        seqtval[i].tv_nsec = (prm->time.us + (i*n_samples*smsep-(int)(i*n_samples*smsep))*1e6)*1000;
        seqatten[i] = 0;
        seqnoise[i] = 0.;
        seqoff[i] = i*n_samples*2*2;
        seqsze[i] = n_samples*2*2;
  
        /*main array samples*/
        for (j=0;j<n_samples;j++) {
          samples[i*n_samples*2*2+j*2] = (int16)(creal(raw_samples[i*n_samples+j]));
          samples[i*n_samples*2*2+j*2+1] = (int16)(cimag(raw_samples[i*n_samples+j]));
        }
        /*interferometer array samples*/
        for (j=0;j<n_samples;j++) {
          samples[i*n_samples*2*2+j*2+n_samples] = 0;
          samples[i*n_samples*2*2+j*2+1+n_samples] = 0;
        }
      }

      IQSetTime(iq,nave,seqtval);
      IQSetAtten(iq,nave,seqatten);
      IQSetNoise(iq,nave,seqnoise);
      IQSetOffset(iq,nave,seqoff);
      IQSetSize(iq,nave,seqsze);

      unsigned int *badtr = malloc(nave*n_pul*2*sizeof(int));

      IQFwrite(stdout,prm2,iq,badtr,samples);
      free(samples);
      free(badtr);
    }

    free(pulse_t);
    free(tau);
    free(raw_samples);
    for (i=0;i<nrang;i++)
      free(acfs[i]);
  }

  /*free dynamically allocated memory*/
  free(acfs);
  free(qflg);
  free(t_d_arr);
  free(t_g_arr);
  free(t_c_arr);
  free(v_dop_arr);
  free(velo_arr);
  free(amp0_arr);
  fclose(fp);

  return 0;
}
