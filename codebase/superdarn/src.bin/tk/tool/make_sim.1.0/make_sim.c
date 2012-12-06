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
#include "dmap.h"
#include "sim_data.h"
#include "rtypes.h"
#include "rprm.h"
#include "rawdata.h"
#include "rawwrite.h"
#include "radar.h"
#include "iq.h"
#include "iqwrite.h"

void makeRadarParm(struct RadarParm * prm, char * argv[], int argc, int cpid, int nave,
                    int lagfr, double smsep, double noise_lev, double amp0, int n_samples,
                    double dt, int n_pul, int n_lags, int nrang, double rngsep, double freq,
                    int * pulse_t)
{
  int i;
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = gmtime(&rawtime);

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
  prm->origin.code = 0;

  RadarParmSetOriginTime(prm,asctime(timeinfo));
  char *tempstr = malloc(argc*15);
  strcpy(tempstr,argv[0]);
  for(i=1;i<argc;i++)
  {
    strcat(tempstr," ");
    strcat(tempstr,argv[i]);
  }
  RadarParmSetOriginCommand(prm,tempstr);


  prm->cp = (int16)cpid;
  prm->stid = 0;
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
  prm->bmnum = 7;
  prm->bmazm = 0;
  prm->scan = 1;
  prm->rxrise = 100;
  prm->intt.sc = (int16)(smsep*n_samples*nave);
  prm->intt.us = (int)(((smsep*n_samples*nave)-(int)(smsep*n_samples*nave))*1e6);
  prm->txpl = 300;
  prm->mpinc = (int16)(dt*1e6);
  prm->mppul = (int16)n_pul;
  prm->mplgs = (int16)n_lags;
  prm->mplgexs = (int16)n_lags;
  prm->nrang = (int16)nrang;
  prm->frang = (int16)(rngsep*lagfr*1e-3);
  prm->rsep = (int16)(rngsep*1e-3);
  prm->xcf = 0;
  prm->tfreq = (int16)(freq*1e-3);
  prm->offset = 0;
  prm->mxpwr = 1070000000;
  prm->lvmax = 20000;

  int16 temp_pul[n_pul];

  for(i=0;i<n_pul;i++)
    temp_pul[i] = (int16)pulse_t[i];

  RadarParmSetPulse(prm,n_pul,temp_pul);


  if(cpid == 1)
  {
    int16 temp_lag[100] = {0,0,26,27,20,22,9,12,22,26,22,27,20,26,20,27,12,20,0,9,
                                12,22,9,20,0,12,9,22,12,26,12,27,9,26,9,27};
    RadarParmSetLag(prm,n_lags,temp_lag);
  }
  else if(cpid == 503)
  {
    int16 temp_lag[100] = {0,0,15,16,27,29,29,32,23,27,27,32,23,29,16,23,15,23,
                            23,32,16,27,15,27,16,29,15,29,32,47,16,32,15,32};
    RadarParmSetLag(prm,n_lags,temp_lag);
  }
  else
  {
    int16 temp_lag[100] = {0,0,42,43,22,24,24,27,27,31,22,27,24,31,14,22,22,
                                31,14,24,31,42,31,43,14,27,0,14,27,42,27,43,14,31,
                                24,42,24,43,22,42,22,43,0,22,0,24};
    RadarParmSetLag(prm,n_lags,temp_lag);
  }

  RadarParmSetCombf(prm,tempstr);

  /*
  prm->pulse = malloc(n_pul*sizeof(int16));
  memcpy(prm->pulse,temp_pul,sizeof(int16)*n_pul);*/
  free(tempstr);
}
/*this is a driver program from the data simulator*/

int main(int argc,char *argv[])
{

  /********************************************************
  ** definitions of variables needed for data generation **
  ********************************************************/
  double t_d = .04;                         /*Irregualrity decay time s*/
  double w = -9999.;                        /*spectral width*/
  double t_g = 1e-6;                        /*irregularity growth time*/
  double t_c = 1000.;                       /*precipitation time constant*/
  double v_dop =450.;                       /*Background velocity (m/s)*/
  double c = 3.e8;                          /*Speed of light (m/s)*/
  double freq = 12.e6;                      /*transmit frequency*/
  double amp0 = 1.;                         /*amplitude scaling factor*/
  int noise_flg = 0;                        /*flag to indicate whether white noise is included*/
  double noise_lev = 0.;                    /*white noise level (ratio)*/
  int nave = 50;                            /*number of pulse sequences in an integration period*/
  int nrang = 100;                          /*number of range gates*/
  int lagfr = 4;                            /*lag to first range*/
  int n_good = 40;                          /*number of range gates containing scatter*/
  int life_dist = 0;                        /*lifetime distribution*/
  double smsep = 300.e-6;                   /*sample spearation*/
  double rngsep = 45.e3;                    /*range gate spearation*/
  int cpid = 150;                             /*control program ID number*/
  int n_samples;                            /*Number of datapoints in a single pulse sequence*/
  int n_pul,n_lags,*pulse_t,*tau,nave_flg=0;
  double dt;                                /*basic lag time*/
  double velo = 0.;                         /*standard devation of gaussian velocity spread*/
  int cri_flg = 1;                          /*cross-range interference flag*/
  int smp_flg = 0;                          /*output raw samples flag*/
  int decayflg = 0;

  /*other variables*/
  long i,j;
  int output = 0;
  double taus;

  char helpstr[] =
  "\nmake_sim:  generates simulated single-component lorentzian ACFs\n\n"
  "Calling Sequence:  ./sim_fitacf [-options] > output.txt\n"
  "Options:\n"
  "--help: show this information\n"
  "-katscan: use Kathryn McWilliams' 8 pulse sequence (default)\n"
  "-tauscan: use Ray Greenwald's 13 pulse sequence\n"
  "-oldscan: use old 7 pulse sequence\n"
  "-freq f: set radar frequency to f (in kHz)\n"
  "         default is 12000 kHz\n"
  "-vel v: set Doppler velocity to v (in m/s)\n"
  "         default is 400 m/s\n"
  "-v_spread v: set gaussian Doppler velocity spread (standard devation) to v\n"
  "         default is 0\n"
  "-t_d t: set decay time to t (in milliseconds)\n"
  "         default is 40 ms\n"
  "-t_g t: set growth time to t (in microseconds)\n"
  "         default is 1 us (negligible)\n"
  "-t_c t: set precipitation time constant (lifetime) to t (in microseconds)\n"
  "         default is 1e6 ms (negligible)\n"
  "-constant: set irregularity lifetime distribution constant\n"
  "         default is exponential\n"
  "-smsep s: set sample separation to s (in microseconds)\n"
  "         default is 300 us\n"
  "-noise n: add in white noise level to produce SNR n (in dB)\n"
  "         default is no noise\n"
  "-nave n: set number of averages in the integration period to n\n"
  "         default is 70/50/20 for oldscan/katscan/tauscan\n"
  "-nrang n: set number of range gates to n\n"
  "         default is 100\n"
  "-amp a: set average ACF amplitude to a\n"
  "         default is 1\n"
  "-nocri: remove cross range interference from the ACFs\n"
  "         default is CRI on\n"
  "         WARNING: removing cross-range interference will make\n"
  "         the raw samples unuseable, since each range gate will\n"
  "         have to be integrated seperately\n"
  "-n_good n: set number of range gates with scatter to n\n"
  "         default is 40\n"
  "         WARNING: setting this above ~70 for katscan or default\n"
  "         sequence will cause cross-range interference at lag 0\n"
  "         from range gates ~70 and above (lag 0 ~ twice the value\n"
  "         at other range gates)\n"
  "-samples: output raw samples (to iqdat file) instead of ACFs\n"
  "         default is output ACFs (to rawacf file)\n"
  "-decay: set ACFs to have a decaying amplitude by a\n"
  "         factor of 1/r^2\n"
  "\nNOTE: all option inputs must be integers\n";




  /*process command line options*/
  for (i = 1; i < argc; i++)
  {
    /*command line control program*/
    if (strcmp(argv[i], "-katscan") == 0)
      cpid = 150;
    if (strcmp(argv[i], "-oldscan") == 0)
      cpid = 1;
    else if (strcmp(argv[i], "-tauscan") == 0)
      cpid = 503;
    /*command line irregularity distribution*/
    else if (strcmp(argv[i], "-constant") == 0)
      life_dist = 1;
    /*command line frequency*/
    else if (strcmp(argv[i], "-freq") == 0)
      freq = atoi(argv[i+1])*1e3;
    /*command line velocity*/
    else if (strcmp(argv[i], "-vel") == 0)
      v_dop = (double)atoi(argv[i+1]);
    /*command line velocity spread*/
    else if (strcmp(argv[i], "-v_spread") == 0)
      velo = (double)atoi(argv[i+1]);
    /*command line spectral width*/
    else if (strcmp(argv[i], "-width") == 0)
      w = ((double)atoi(argv[i+1]));
    /*command line decorrelation time*/
    else if (strcmp(argv[i], "-t_d") == 0)
      t_d = 1e-3*atoi(argv[i+1]);
    /*command line growth time*/
    else if (strcmp(argv[i], "-t_g") == 0)
      t_g = 1e-6*atoi(argv[i+1]);
    /*command line precipitation time constant*/
    else if (strcmp(argv[i], "-t_c") == 0)
      t_c = 1e-3*atoi(argv[i+1]);
    /*command line number of range gates*/
    else if (strcmp(argv[i], "-nrang") == 0)
      nrang = atoi(argv[i+1]);
		/*command line power decay with range*/
    else if (strcmp(argv[i], "-decay") == 0)
      decayflg = 1;
    /*command line nave*/
    else if (strcmp(argv[i], "-nave") == 0)
    {
      nave_flg = 1;
      nave = atoi(argv[i+1]);
    }
    /*command line noise*/
    else if (strcmp(argv[i], "-noise") == 0)
    {
      noise_flg = 1;
      noise_lev = 1./pow(10.,((double)atoi(argv[i+1]))/10.);
    }
    /*command line amplitude*/
    else if (strcmp(argv[i], "-amp") == 0)
      amp0 = (double)atoi(argv[i+1]);
    /*command line sample separation*/
    else if (strcmp(argv[i], "-smsep") == 0)
      smsep = 1e-6*atoi(argv[i+1]);
    /*command line range gates with scatter*/
    else if (strcmp(argv[i], "-n_good") == 0)
      n_good = atoi(argv[i+1]);
    /*command line CRI flag*/
    else if (strcmp(argv[i], "-nocri") == 0)
      cri_flg = 0;
    /*command line output samples*/
    else if (strcmp(argv[i], "-samples") == 0)
      smp_flg = 1;
    /*display help*/
    else if (strcmp(argv[i], "--help") == 0)
    {
      fprintf(stderr,"%s\n",helpstr);
      exit(0);
    }
  }

  double lambda = c/freq;
  if(w != -9999.)
    t_d = lambda/(w*2.*PI);

  /*oldscan*/
  if(cpid == 1)
  {
    dt = 2.4e-3;                          /*basic lag time*/
    n_pul = 7;                            /*number of pulses*/
    n_lags = 18;                          /*number of lags in the ACFs*/
    /*if the user did not set nave*/
    if(!nave_flg)
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
    for(i=0;i<n_lags;i++)
      tau[i] = i;
    /*no lag 16*/
    tau[16] += 1;
    tau[17] += 1;
  }
  /*tauscan*/
  else if(cpid == 503)
  {
    dt = 2.4e-3;                          /*basic lag time*/
    n_pul = 13;                           /*number of pulses*/
    n_lags = 17;                          /*number of lags in the ACFs*/
    /*if the user did not set nave*/
    if(!nave_flg)
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
    for(i=0;i<10;i++)
      tau[i] = i;
    /*no lag 10*/
    for(i=10;i<18;i++)
      tau[i] = (i+1);
  }
  /*katscan (default)*/
  else
  {
    dt = 1.5e-3;                          /*basic lag time*/
    n_pul = 8;                            /*number of pulses*/
    n_lags = 23;                          /*number of lags in the ACFs*/
    /*if the user did not set nave*/
    if(!nave_flg)
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
    for(i=0;i<6;i++)
      tau[i] = i;
    /*no lag 6*/
    for(i=6;i<22;i++)
      tau[i] = (i+1);
    /*no lag 23*/
    tau[22] = 24;
  }


  /*control program dependent variables*/
  taus = dt/smsep;                                      /*lag time in samples*/
  n_samples = (pulse_t[n_pul-1]*taus+nrang+lagfr);      /*number of samples in 1 pulse sequence*/

  /*Creating the output array for ACFs*/
  complex double ** acfs = malloc(nrang*sizeof(complex double *));
  for(i=0;i<nrang;i++)
  {
    acfs[i] = malloc(n_lags*sizeof(complex double));
    for(j=0;j<n_lags;j++)
      acfs[i][j] = 0.+I*0.;
  }

  /*flags to tell which range gates contain scatter*/
  int * qflg = malloc(nrang*sizeof(int));
  for(i=0;i<nrang;i++)
    if(i < n_good)
      qflg[i] = 1;
    else
      qflg[i] = 0;



  /*create a structure to store the raw samples from each pulse sequence*/
  complex double * raw_samples = malloc(n_samples*nave*sizeof(complex double));

  /**********************************************************
  ****FILL THESE ARRAYS WITH THE SIMULATION PARAMETERS*******
  **********************************************************/

  /*array with the irregularity decay time for each range gate*/
  double * t_d_arr = malloc(nrang*sizeof(double));
  for(i=0;i<nrang;i++)
    t_d_arr[i] = t_d;

  /*array with the irregularity growth time for each range gate*/
  double * t_g_arr = malloc(nrang*sizeof(double));
  for(i=0;i<nrang;i++)
    t_g_arr[i] = t_g;

  /*array with the irregularity lifetime for each range gate*/
  double * t_c_arr = malloc(nrang*sizeof(double));
  for(i=0;i<nrang;i++)
    t_c_arr[i] = t_c;

  /*array with the irregularity doppler velocity for each range gate*/
  double * v_dop_arr = malloc(nrang*sizeof(double));
  for(i=0;i<nrang;i++)
    v_dop_arr[i] = v_dop;

  /*array with the irregularity doppler velocity for each range gate*/
  double * velo_arr = malloc(nrang*sizeof(double));
  for(i=0;i<nrang;i++)
    velo_arr[i] = velo;


  /*array with the ACF amplitude for each range gate*/
  double * amp0_arr = malloc(nrang*sizeof(double));
  for(i=0;i<nrang;i++)
    amp0_arr[i] = amp0;
	
	if(noise_flg) noise_lev *= amp0;

  /*call the simulation function*/
  sim_data(t_d_arr, t_g_arr, t_c_arr, v_dop_arr, qflg, velo_arr, amp0_arr, freq, noise_lev,
            noise_flg, nave, nrang, lagfr, smsep, cpid, life_dist,
            n_pul, cri_flg, n_lags, pulse_t, tau, dt, raw_samples, acfs, decayflg);

  /*pill the parameter structure*/
  struct RadarParm * prm;
  prm = RadarParmMake();
  makeRadarParm(prm, argv, argc, cpid, nave, lagfr, smsep, noise_lev, amp0, n_samples,
                    dt, n_pul, n_lags, nrang, rngsep, freq, pulse_t);

  if(!smp_flg)
  {
    /*fill the rawdata structure*/
    struct RawData * raw;
    raw = RawMake();

    raw->revision.major = 1;
    raw->revision.minor = 1;
    raw->thr=0.0;
    int * slist = malloc(nrang*sizeof(int));
    float * pwr0 = malloc(nrang*sizeof(float));
    float * acfd = malloc(nrang*n_lags*2*sizeof(float));
    float * xcfd = malloc(nrang*n_lags*2*sizeof(float));
    for(i=0;i<nrang;i++)
    {
      slist[i] = i;
      pwr0[i] = creal(acfs[i][0]);
      for(j=0;j<n_lags;j++)
      {
        acfd[i*n_lags*2+j*2] = creal(acfs[i][j]);
        acfd[i*n_lags*2+j*2+1] = cimag(acfs[i][j]);
        xcfd[i*n_lags*2+j*2] = 0.;
        xcfd[i*n_lags*2+j*2+1] = 0.;
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
  }
  else
  {
    /*fill the iqdata structure*/
    struct IQ *iq;
    iq=IQMake();

    int16 * samples = malloc(n_samples*nave*2*2*sizeof(int16));
    for(i=0;i<nave;i++)
    {
      /*main array samples*/
      for(j=0;j<n_samples;j++)
      {
        samples[i*n_samples*2*2+j*2] = (int16)(creal(raw_samples[i*n_samples+j]));
        samples[i*n_samples*2*2+j*2+1] = (int16)(cimag(raw_samples[i*n_samples+j]));
      }
      /*interferometer array samples*/
      for(j=0;j<n_samples;j++)
      {
        samples[i*n_samples*2*2+j*2+n_samples] = 0;
        samples[i*n_samples*2*2+j*2+1+n_samples] = 0;
      }
    }

    int * badtr = malloc(nave*n_pul*2*sizeof(int));

    IQFwrite(stdout,prm,iq,badtr,samples);
    free(samples);
    free(badtr);
  }

  /*output ACFs to rawacf file
  if(output == 0)
  {
  }

  fprintf(stdout,"%d  %d  %d  %d  %d  %d  %d  %lf  %lf  %lf\n",
                  cpid,nrang,n_lags,n_samples,nave,n_pul,n_lags,dt,smsep,freq);
  fprintf(stdout,"%lf  %lf  %lf  %lf  %lf  %lf  %lf  %lf\n",
                  amp0,v_dop*2./lambda,v_dop,t_d,lambda/(2.*PI*t_d),
                  t_g,t_c,20.*log10(1./noise_lev));

  for(i=0;i<n_samples*nave;i++)
  {
    fprintf(stdout,"%lf  %lf\n",creal(raw_samples[i]),cimag(raw_samples[i]));
  }
  /*print the ACFs
  for(r=0;r<nrang;r++)
  {
    fprintf(stdout,"%d  %d\n",r,qflg[r]);
    for(i=0;i<n_lags;i++)
      fprintf(stdout,"%d  %lf  %lf\n",tau[i],creal(acfs[r][i]),cimag(acfs[r][i]));
  }*/


  /*free dynamically allocated memory*/
  for(i=0;i<nrang;i++)
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


  return 0;
}
