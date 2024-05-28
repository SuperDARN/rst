 /*COPYRIGHT:
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

 Evan Thomas 2021
 Modified to use standard RST documentation and command line options,
 fixed iqdat-format output, and added several other user options

 E.G.Thomas 2022-08: added support for bmoff parameter, XCFs, extended pulse sequence
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
#include "rtime.h"
#include "rmath.h"
#include "dmap.h"
#include "sim_data.h"
#include "rtypes.h"
#include "rprm.h"
#include "rawdata.h"
#include "rawwrite.h"
#include "radar.h"
#include "iq.h"
#include "iqwrite.h"
#include "rpos.h"
#include "rpos_v2.h"

#include "errstr.h"
#include "hlpstr.h"

struct OptionData opt;
struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_sim --help\n");
  return(-1);
}

void makeRadarParm(struct RadarParm *prm, char *argv[], int argc, int cpid, int nave,
                    int lagfr, double smsep, double noise_lev, double amp0, int n_samples,
                    double dt, int n_pul, int n_lags, int nrang, double rngsep, double freq,
                    int *pulse_t, int stid, int beam, int xcf)
{
  int i;
  time_t rawtime;
  struct tm *timeinfo;
  char tmstr[40];

  char *envstr=NULL;
  FILE *fp;
  float offset;

  rawtime = time((time_t) 0);
  timeinfo = gmtime(&rawtime);

  strcpy(tmstr,asctime(timeinfo));
  tmstr[24]=0;

  if (prm->origin.time !=NULL) free(prm->origin.time);
  if (prm->origin.command !=NULL) free(prm->origin.command);
  if (prm->pulse !=NULL) free(prm->pulse);
  for (i=0;i<2;i++) if (prm->lag[i] !=NULL) free(prm->lag[i]);

  memset(prm,0,sizeof(struct RadarParm));
  prm->origin.time=NULL;
  prm->origin.command=NULL;
  prm->pulse=NULL;
  prm->lag[0]=NULL;
  prm->lag[1]=NULL;
  prm->combf=NULL;


  prm->revision.major = 1;
  prm->revision.minor = 0;
  // set to 1 as it is not produced on site 
  prm->origin.code = 1;

  RadarParmSetOriginTime(prm,tmstr);
  char *tempstr = malloc(argc*15);
  strcpy(tempstr,argv[0]);
  for (i=1;i<argc;i++) {
    strcat(tempstr," ");
    strcat(tempstr,argv[i]);
  }
  RadarParmSetOriginCommand(prm,tempstr);

  prm->cp = (int16)cpid;
  prm->stid = stid;
  prm->time.yr = (int16)(timeinfo->tm_year+1900);
  prm->time.mo = (int16)(timeinfo->tm_mon+1);
  prm->time.dy = (int16)timeinfo->tm_mday;
  prm->time.hr = (int16)timeinfo->tm_hour;
  prm->time.mt = (int16)timeinfo->tm_min;
  prm->time.sc = (int16)timeinfo->tm_sec;
  prm->time.us = 0;

  prm->txpow = 9000;
  prm->nave = (int16)nave;
  prm->atten = 0;
  prm->lagfr = (int16)(lagfr*smsep*1e6);
  prm->smsep = (int16)(smsep*1.e6);
  prm->ercod = 0;
  prm->stat.agc = 8192;
  prm->stat.lopwr = 8192;
  prm->noise.search = noise_lev;
  prm->noise.mean = 0.;
  prm->channel = 0;
  prm->bmnum = beam;

  envstr = getenv("SD_RADAR");
  fp = fopen(envstr,"r");
  network = RadarLoad(fp);
  fclose(fp);
  envstr = getenv("SD_HDWPATH");
  RadarLoadHardware(envstr,network);
  radar = RadarGetRadar(network,prm->stid);
  site = RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,prm->time.dy,
                            prm->time.hr,prm->time.mt,prm->time.sc);

  if (site == NULL || stid == 0) prm->bmazm = 0;
  else {
    offset = site->maxbeam/2.0-0.5;
    prm->bmazm = site->boresite + site->bmsep*(prm->bmnum-offset) + site->bmoff;
  }

  prm->scan = 1;
  prm->rxrise = 100;
  prm->intt.sc = (int16)(smsep*n_samples*nave);
  prm->intt.us = (int)(((smsep*n_samples*nave)-(int)(smsep*n_samples*nave))*1e6);
  prm->txpl = (int16)((rngsep*20)/3*1e-3);
  prm->mpinc = (int16)(dt*1e6);
  prm->mppul = (int16)n_pul;
  prm->mplgs = (int16)n_lags;

  if (cpid == 503) prm->mplgexs = (int16)n_lags;
  else             prm->mplgexs = 0;

  prm->nrang = (int16)nrang;
  prm->frang = (int16)(rngsep*lagfr*1e-3);
  prm->rsep = (int16)(rngsep*1e-3);
  prm->xcf = (int16)xcf;
  prm->tfreq = (int16)(freq*1e-3);
  prm->offset = 0;
  prm->mxpwr = 1070000000;
  prm->lvmax = 20000;

  int16 temp_pul[n_pul];

  for (i=0;i<n_pul;i++)
    temp_pul[i] = (int16)pulse_t[i];

  RadarParmSetPulse(prm,n_pul,temp_pul);


  if (cpid == 1) {
    int16 temp_lag[100] = {0,0,26,27,20,22,9,12,22,26,22,27,20,26,20,27,12,20,0,9,
                                12,22,9,20,0,12,9,22,12,26,12,27,9,26,9,27};
    RadarParmSetLag(prm,n_lags,temp_lag);
  } else if (cpid == 503) {
    int16 temp_lag[100] = {0,0,15,16,27,29,29,32,23,27,27,32,23,29,16,23,15,23,
                            23,32,16,27,15,27,16,29,15,29,32,47,16,32,15,32};
    RadarParmSetLag(prm,n_lags,temp_lag);
  } else if (cpid == 9100) {
    int16 temp_lag[244] = {1495,1495,0,4,4,19,0,19,19,42,42,78,4,42,0,42,78,127,19,78,
                           127,191,4,78,0,78,191,270,42,127,270,364,19,127,364,474,78,191,4,127,
                           474,600,0,127,127,270,600,745,42,191,745,905,191,364,19,191,905,1083,4,191,
                           0,191,78,270,1083,1280,270,474,1280,1495,42,270,127,364,364,600,19,270,4,270,
                           0,270,474,745,191,474,78,364,600,905,42,364,270,600,745,1083,19,364,127,474,
                           4,364,0,364,905,1280,364,745,78,474,191,600,1083,1495,474,905,42,474,19,474,
                           4,474,127,600,0,474,270,745,600,1083,78,600,745,1280,364,905,191,745,42,600,
                           19,600,905,1495,4,600,0,600,474,1083,127,745,270,905,78,745,600,1280,42,745,
                           191,905,364,1083,19,745,4,745,0,745,745,1495,127,905,474,1280,270,1083,78,905,
                           42,905,19,905,191,1083,600,1495,4,905,0,905,364,1280,127,1083,78,1083,270,1280,
                           474,1495,42,1083,19,1083,4,1083,0,1083,191,1280,364,1495,127,1280,78,1280,270,1495,
                           42,1280,19,1280,4,1280,0,1280,191,1495,127,1495,78,1495,42,1495,19,1495,4,1495,
                           0,1495,1495,1495};
    RadarParmSetLag(prm,n_lags,temp_lag);
  } else {
    int16 temp_lag[100] = {0,0,42,43,22,24,24,27,27,31,22,27,24,31,14,22,22,
                                31,14,24,31,42,31,43,14,27,0,14,27,42,27,43,14,31,
                                24,42,24,43,22,42,22,43,0,22,0,24};
    RadarParmSetLag(prm,n_lags,temp_lag);
  }

  RadarParmSetCombf(prm,tempstr);

  free(tempstr);
}
/*this is a driver program for the data simulator*/

int main(int argc,char *argv[])
{

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int stid = 0;
  int beam = 7;
  int mpinc = -1;
  int xcf = 0;

  int katscan = 0;
  int oldscan = 0;
  int tauscan = 0;
  int spaletascan = 0;

  /********************************************************
  ** definitions of variables needed for data generation **
  ********************************************************/
  double t_d = 40.;                         /*Irregualrity decay time (ms)*/
  double w = -9999.;                        /*spectral width*/
  double t_g = 1.;                          /*irregularity growth time (us)*/
  double t_c = 1e6;                         /*precipitation time constant (ms)*/
  double v_dop = 450.;                      /*Background velocity (m/s)*/
  double freq = 12.;                        /*transmit frequency (MHz)*/
  double amp0 = 1.;                         /*amplitude scaling factor*/
  int noise_flg = 0;                        /*flag to indicate whether white noise is included*/
  double noise_lev = 0.;                    /*white noise level (ratio)*/
  int nave = 0;                             /*number of pulse sequences in an integration period*/
  int nrang = 100;                          /*number of range gates*/
  int lagfr = 4;                            /*lag to first range*/
  int n_good = 40;                          /*number of range gates containing scatter*/
  int life_dist = 0;                        /*lifetime distribution*/
  double smsep = 300.;                      /*sample spearation (us)*/
  double rngsep = 45.e3;                    /*range gate spearation (m)*/
  int cpid = 150;                           /*control program ID number*/
  int n_samples;                            /*Number of datapoints in a single pulse sequence*/
  int n_pul,n_lags,*pulse_t,*tau,nave_flg=0;
  double dt;                                /*basic lag time*/
  double velo = 0.;                         /*standard devation of gaussian velocity spread*/
  int cri_flg = 0;                          /*cross-range interference flag*/
  int smp_flg = 0;                          /*output raw samples flag*/
  int decayflg = 0;
  int srng = 0;                             /*first range gate containing scatter*/
  double elv = -9999.;                      /*elevation angle*/
  double vht = -9999.;                      /*virtual height*/
  double tdiff = -9999.;                    /*tdiff*/

  /*other variables*/
  long i,j;
  double taus;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"katscan",'x',&katscan);       /* control program */
  OptionAdd(&opt,"oldscan",'x',&oldscan);
  OptionAdd(&opt,"tauscan",'x',&tauscan);
  OptionAdd(&opt,"spaletascan",'x',&spaletascan);

  OptionAdd(&opt,"xcf",'x',&xcf);               /* calculate interferometer samples / XCFs */
  OptionAdd(&opt,"elv",'d',&elv);               /* elevation angle [deg] */
  OptionAdd(&opt,"vht",'d',&vht);               /* virtual height [km] */
  OptionAdd(&opt,"tdiff",'d',&tdiff);           /* tdiff [us] */

  OptionAdd(&opt,"constant",'x',&life_dist);    /* irregularity distribution */
  OptionAdd(&opt,"freq",'d',&freq);             /* frequency [MHz] */
  OptionAdd(&opt,"vel",'d',&v_dop);             /* velocity [m/s] */
  OptionAdd(&opt,"v_spread",'d',&velo);         /* velocity spread [m/s] */
  OptionAdd(&opt,"width",'d',&w);               /* spectral width [m/s] */
  OptionAdd(&opt,"t_d",'d',&t_d);               /* decorrelation time [ms] */
  OptionAdd(&opt,"t_g",'d',&t_g);               /* growth time [us] */
  OptionAdd(&opt,"t_c",'d',&t_c);               /* precipitation time constant [ms] */
  OptionAdd(&opt,"nrang",'i',&nrang);           /* number of range gates */
  OptionAdd(&opt,"decay",'x',&decayflg);        /* set ACFs to have decaying amplitude */
  OptionAdd(&opt,"nave",'i',&nave);             /* number of averages */
  OptionAdd(&opt,"noise",'d',&noise_lev);       /* white noise level [db] */
  OptionAdd(&opt,"amp",'d',&amp0);              /* ACF amplitude */
  OptionAdd(&opt,"smsep",'d',&smsep);           /* sample separation [us] */
  OptionAdd(&opt,"n_good",'i',&n_good);         /* number of ranges with scatter */
  OptionAdd(&opt,"srng",'i',&srng);             /* first range gate containing scatter */
  OptionAdd(&opt,"nocri",'x',&cri_flg);         /* remove cross-range interference */
  OptionAdd(&opt,"iq",'x',&smp_flg);            /* output raw samples (iqdat) */
  OptionAdd(&opt,"stid",'i',&stid);             /* radar station ID number */
  OptionAdd(&opt,"beam",'i',&beam);             /* radar beam number */
  OptionAdd(&opt,"mpinc",'i',&mpinc);           /* multi-pulse increment [us] */

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

  freq = 1e6*freq;
  t_d = 1e-3*t_d;
  t_g = 1e-6*t_g;
  t_c = 1e-3*t_c;
  smsep = 1e-6*smsep;
  if (nave) nave_flg = 1;
  if (noise_lev != 0.) {
    noise_flg = 1;
    noise_lev = 1./pow(10.,noise_lev/10.);
  }
  cri_flg = !cri_flg;

  double lambda = C/freq;
  if (w != -9999.)
    t_d = lambda/(w*2.*PI);

  /*oldscan*/
  if (oldscan) {
    cpid = 1;
    dt = 2.4e-3;                          /*basic lag time*/
    n_pul = 7;                            /*number of pulses*/
    n_lags = 18;                          /*number of lags in the ACFs*/
    /*if the user did not set nave*/
    if (!nave_flg)
      nave = 70;                          /*number of averages*/

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
    dt = 2.4e-3;                          /*basic lag time*/
    n_pul = 13;                           /*number of pulses*/
    n_lags = 17;                          /*number of lags in the ACFs*/
    /*if the user did not set nave*/
    if (!nave_flg)
      nave = 20;                          /*number of averages*/

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
  /*spaletascan*/
  else if (spaletascan) {
    cpid = 9100;
    dt = 0.1e-3;                          /*basic lag time*/
    n_pul = 16;                           /*number of pulses*/
    n_lags = 121;                         /*number of lags in the ACFs*/
    /*if the user did not set nave*/
    if (!nave_flg)
      nave = 16;                          /*number of averages*/

    smsep = 100.e-6;
    rngsep = 15.0e3;
    lagfr = 12;
    nrang = 225;

    /*fill the pulse table*/
    pulse_t = malloc(n_pul*sizeof(int));
    pulse_t[0] = 0;
    pulse_t[1] = 4;
    pulse_t[2] = 19;
    pulse_t[3] = 42;
    pulse_t[4] = 78;
    pulse_t[5] = 127;
    pulse_t[6] = 191;
    pulse_t[7] = 270;
    pulse_t[8] = 364;
    pulse_t[9] = 474;
    pulse_t[10] = 600;
    pulse_t[11] = 745;
    pulse_t[12] = 905;
    pulse_t[13] = 1083;
    pulse_t[14] = 1280;
    pulse_t[15] = 1495;

    /*Creating lag array*/
    tau = malloc(n_lags*sizeof(int));
    for (i=0;i<n_lags;i++)
      tau[i] = i;
  }
  /*katscan (default)*/
  else {
    dt = 1.5e-3;                          /*basic lag time*/
    n_pul = 8;                            /*number of pulses*/
    n_lags = 23;                          /*number of lags in the ACFs*/
    /*if the user did not set nave*/
    if (!nave_flg)
      nave = 50;                          /*number of averages*/

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
  if (mpinc != -1) dt = 1e-6*mpinc;                     /* allow user to override lag time */
  taus = dt/smsep;                                      /*lag time in samples*/
  n_samples = (pulse_t[n_pul-1]*taus+nrang+lagfr);      /*number of samples in 1 pulse sequence*/

  /*Creating the output array for ACFs*/
  complex double **acfs = malloc(nrang*sizeof(complex double *));
  for (i=0;i<nrang;i++) {
    acfs[i] = malloc(n_lags*sizeof(complex double));
    for (j=0;j<n_lags;j++)
      acfs[i][j] = 0.+I*0.;
  }

  /*flags to tell which range gates contain scatter*/
  int *qflg = malloc(nrang*sizeof(int));
  for (i=0;i<srng;i++)
    qflg[i] = 0;
  for (i=srng;i<nrang;i++)
    if (i < srng+n_good)
      qflg[i] = 1;
    else
      qflg[i] = 0;



  /*create a structure to store the raw samples from each pulse sequence*/
  complex double *raw_samples = malloc(n_samples*nave*sizeof(complex double));

  /**********************************************************
  ****FILL THESE ARRAYS WITH THE SIMULATION PARAMETERS*******
  **********************************************************/

  /*array with the irregularity decay time for each range gate*/
  double *t_d_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    t_d_arr[i] = t_d;

  /*array with the irregularity growth time for each range gate*/
  double *t_g_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    t_g_arr[i] = t_g;

  /*array with the irregularity lifetime for each range gate*/
  double *t_c_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    t_c_arr[i] = t_c;

  /*array with the irregularity doppler velocity for each range gate*/
  double *v_dop_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    v_dop_arr[i] = v_dop;

  /*array with the irregularity doppler velocity for each range gate*/
  double *velo_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    velo_arr[i] = velo;

  /*array with the ACF amplitude for each range gate*/
  double *amp0_arr = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    amp0_arr[i] = amp0;

  double *psi_obs = malloc(nrang*sizeof(double));
  for (i=0;i<nrang;i++)
    psi_obs[i] = 0.;

  if (noise_flg) noise_lev *= amp0;

  /*call the simulation function*/
  sim_data(t_d_arr, t_g_arr, t_c_arr, v_dop_arr, qflg, velo_arr, amp0_arr, freq, noise_lev,
           noise_flg, nave, nrang, lagfr, smsep, cpid, life_dist,
           n_pul, cri_flg, n_lags, pulse_t, tau, dt, raw_samples, acfs, decayflg);

  /*fill the parameter structure*/
  struct RadarParm *prm;
  prm = RadarParmMake();
  makeRadarParm(prm, argv, argc, cpid, nave, lagfr, smsep, noise_lev, amp0, n_samples,
                dt, n_pul, n_lags, nrang, rngsep, freq, pulse_t, stid, beam, xcf);

  if (xcf) {
    /* Calculate the observed phase difference between the main and
     * interferometer antenna arrays, using a virtual height model */
    double rng;
    double xh;
    double hop;

    double X,Y,Z;
    double boff;
    double phi0;
    double cp0,sp0;
    double alpha;
    double sa;

    if (tdiff == -9999.) {
      tdiff = site->tdiff[0];
    }

    X = site->interfer[0];
    Y = site->interfer[1];
    Z = site->interfer[2];

    boff = site->maxbeam/2.-0.5;
    phi0 = (site->bmoff + site->bmsep*(beam - boff))*PI/180.;
    cp0 = cos(phi0);
    sp0 = sin(phi0);

    for (i=0;i<nrang;i++) {
      if (elv != -9999.) {
        alpha = elv*PI/180.;
      } else {
        rng = slant_range(rngsep*lagfr*1e-3, rngsep*1e-3, 0, 0, i+1);
        if (vht != -9999.) {
          xh = vht;
          hop = 0.5;
        } else {
          xh = calc_cv_vhm(rng, 0, &hop);
        }
        alpha = calc_elevation_angle(rng, xh, hop, 0)*PI/180.;
      }
      sa = sin(alpha);

      psi_obs[i] = 2*PI*freq*((1/C)*(X*sp0 + Y*sqrt(cp0*cp0-sa*sa) + Z*sa) - tdiff*1e-6);
    }
  }

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
        xcfd[i*n_lags*2+j*2] = xcf*creal(acfs[i][j]*(cos(psi_obs[i])+I*sin(psi_obs[i])));
        xcfd[i*n_lags*2+j*2+1] = xcf*cimag(acfs[i][j]*(cos(psi_obs[i])+I*sin(psi_obs[i])));
      }
    }

    RawSetPwr(raw,nrang,pwr0,nrang,slist);
    RawSetACF(raw,nrang,n_lags,acfd,nrang,slist);
    RawSetXCF(raw,nrang,n_lags,xcfd,nrang,slist);
    i=RawFwrite(stdout,prm,raw);
    free(slist);
    free(pwr0);
    free(acfd);
    free(xcfd);
  } else {
    /*fill the iqdata structure*/
    struct IQ *iq;
    iq=IQMake();

    struct timeval tick;
    struct timespec seqtval[nave];
    int seqatten[nave];
    float seqnoise[nave];
    int seqoff[nave];
    int seqsze[nave];

    iq->seqnum = nave;
    iq->chnnum = 2;
    iq->smpnum = n_samples;
    iq->skpnum = 4;

    gettimeofday(&tick,NULL);

    int16 *samples = malloc(n_samples*nave*2*2*sizeof(int16));
    for (i=0;i<nave;i++) {
      /*iq structure values*/
      seqtval[i].tv_sec = tick.tv_sec + (int)(i*n_samples*smsep);
      seqtval[i].tv_nsec = (tick.tv_usec + (i*n_samples*smsep-(int)(i*n_samples*smsep))*1e6)*1000;
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
        samples[i*n_samples*2*2+j*2+n_samples] = (int16)(xcf*creal(raw_samples[i*n_samples+j]*(cos(psi_obs[i])+I*sin(psi_obs[i]))));
        samples[i*n_samples*2*2+j*2+1+n_samples] = (int16)(xcf*cimag(raw_samples[i*n_samples+j]*(cos(psi_obs[i])+I*sin(psi_obs[i]))));
      }
    }

    IQSetTime(iq,nave,seqtval);
    IQSetAtten(iq,nave,seqatten);
    IQSetNoise(iq,nave,seqnoise);
    IQSetOffset(iq,nave,seqoff);
    IQSetSize(iq,nave,seqsze);

    unsigned int *badtr = malloc(nave*n_pul*2*sizeof(int));

    IQFwrite(stdout,prm,iq,badtr,samples);
    free(samples);
    free(badtr);
  }

  /*free dynamically allocated memory*/
  for (i=0;i<nrang;i++)
    free(acfs[i]);
  free(acfs);
  free(pulse_t);
  free(tau);
  free(qflg);
  free(raw_samples);
  free(t_d_arr);
  free(t_g_arr);
  free(t_c_arr);
  free(v_dop_arr);
  free(velo_arr);
  free(amp0_arr);
  free(psi_obs);

  return 0;
}
