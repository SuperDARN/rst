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
#include "sim_data.h"


/*this is a C version of Pasha's IDL data generator*/

/*generates single-compnent, Lorentzian ACF*/


/*Summary:  individual scatterers are randomly distributed in space
across a 45-km range gate and in time across an integration
period.  These signal from these scatterers is then sampled
using a standard SuperDARN pulse sequence, and ACFs are
calculated from these raw samples*/

/*random number generator from numerical recipes*/
/*needed to generate a gaussian random number*/
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
float ran1(long *idum)
{
  int j;
  long k;
  static long iy=0;
  static long iv[NTAB];
  float temp;

  if(*idum <= 0 || !iy)
  {
    if(-(*idum) < 1)
      *idum = 1;
    else
      *idum = -(*idum);
    for(j=NTAB+7; j>=0; j--)
    {
      k = (*idum)/IQ;
      *idum = IA*(*idum-k*IQ)-IR*k;
      if(*idum < 0)
        *idum += IM;
      if(j < NTAB)
        iv[j] = *idum;
    }
    iy = iv[0];
  }
  k = (*idum)/IQ;
  *idum = IA*(*idum-k*IQ)-IR*k;
  if(*idum < 0)
    *idum += IM;
  j = iy/NDIV;
  iy = iv[j];
  iv[j] = *idum;
  if((temp=AM*iy) > RNMX)
    return RNMX;
  else
    return temp;
}

/*gaussian random number generator from numerical recipes*/
float gasdev(long *idum)
{
  float ran1(long *idum);
  static int iset=0;
  static float gset;
  float fac, rsq, v1, v2;

  if(*idum < 0)
    iset = 0;
  if(iset == 0)
  {
    do
    {
      v1 = 2.0*ran1(idum)-1.;
      v2 = 2.0*ran1(idum)-1.;
      rsq = v1*v1+v2*v2;
    } while(rsq >= 1. || rsq == 0.);
    fac = sqrt(-2.0*log(rsq)/rsq);
    gset  = v1*fac;
    iset = 1;
    return v2*fac;
  }
  else
  {
    iset = 0;
    return gset;
  }
}


void acf_27(complex double * aa, complex double * rr, int cpid)
{
  if(cpid == 1)
  {
    rr[0]=aa[0]*conj(aa[0]);
    rr[1]=aa[5]*conj(aa[6]);
    rr[2]=aa[3]*conj(aa[4]);
    rr[3]=aa[1]*conj(aa[2]);
    rr[4]=aa[4]*conj(aa[5]);
    rr[5]=aa[4]*conj(aa[6]);
    rr[6]=aa[3]*conj(aa[5]);
    rr[7]=aa[3]*conj(aa[6]);
    rr[8]=aa[2]*conj(aa[3]);
    rr[9]=aa[0]*conj(aa[1]);
    rr[10]=aa[2]*conj(aa[4]);
    rr[11]=aa[1]*conj(aa[3]);
    rr[12]=aa[0]*conj(aa[2]);
    rr[13]=aa[1]*conj(aa[4]);
    rr[14]=aa[2]*conj(aa[5]);
    rr[15]=aa[2]*conj(aa[6]);
    rr[16]=aa[1]*conj(aa[5]);
    rr[17]=aa[1]*conj(aa[6]);
  }
  if(cpid == 150)
  {
    rr[0]=aa[0]*conj(aa[0]);
    rr[1]=aa[6]*conj(aa[7]);
    rr[2]=aa[2]*conj(aa[3]);
    rr[3]=aa[3]*conj(aa[4]);
    rr[4]=aa[4]*conj(aa[5]);
    rr[5]=aa[2]*conj(aa[4]);
    rr[6]=aa[3]*conj(aa[5]);
    rr[7]=aa[1]*conj(aa[2]);
    rr[8]=aa[2]*conj(aa[5]);
    rr[9]=aa[1]*conj(aa[3]);
    rr[10]=aa[5]*conj(aa[6]);
    rr[11]=aa[5]*conj(aa[7]);
    rr[12]=aa[1]*conj(aa[4]);
    rr[13]=aa[0]*conj(aa[1]);
    rr[14]=aa[4]*conj(aa[6]);
    rr[15]=aa[4]*conj(aa[7]);
    rr[16]=aa[1]*conj(aa[5]);
    rr[17]=aa[3]*conj(aa[6]);
    rr[18]=aa[3]*conj(aa[7]);
    rr[19]=aa[2]*conj(aa[6]);
    rr[20]=aa[2]*conj(aa[7]);
    rr[21]=aa[0]*conj(aa[2]);
    rr[22]=aa[0]*conj(aa[3]);
  }
  if(cpid == 503)
  {
    complex double * temp = malloc(17*sizeof(complex double));
    int i;
    rr[0]=aa[0]*conj(aa[0]);
    rr[1]=aa[1]*conj(aa[2]);
    rr[2]=aa[4]*conj(aa[5]);
    rr[3]=aa[5]*conj(aa[6]);
    rr[4]=aa[3]*conj(aa[4]);
    rr[5]=aa[4]*conj(aa[6]);
    rr[6]=aa[3]*conj(aa[5]);
    rr[7]=aa[2]*conj(aa[3]);
    rr[8]=aa[1]*conj(aa[3]);
    rr[9]=aa[3]*conj(aa[6]);
    rr[10]=aa[2]*conj(aa[4]);
    rr[11]=aa[1]*conj(aa[4]);
    rr[12]=aa[2]*conj(aa[5]);
    rr[13]=aa[1]*conj(aa[5]);
    rr[14]=aa[0]*conj(aa[1]);
    rr[15]=aa[2]*conj(aa[6]);
    rr[16]=aa[1]*conj(aa[6]);


    temp[0]=aa[0]*conj(aa[0]);
    temp[1]=aa[11]*conj(aa[12]);
    temp[2]=aa[8]*conj(aa[9]);
    temp[3]=aa[7]*conj(aa[8]);
    temp[4]=aa[9]*conj(aa[10]);
    temp[5]=aa[7]*conj(aa[9]);
    temp[6]=aa[8]*conj(aa[10]);
    temp[7]=aa[10]*conj(aa[11]);
    temp[8]=aa[10]*conj(aa[12]);
    temp[9]=aa[7]*conj(aa[10]);
    temp[10]=aa[9]*conj(aa[11]);
    temp[11]=aa[9]*conj(aa[12]);
    temp[12]=aa[8]*conj(aa[11]);
    temp[13]=aa[8]*conj(aa[12]);
    temp[14]=aa[6]*conj(aa[7]);
    temp[15]=aa[7]*conj(aa[11]);
    temp[16]=aa[7]*conj(aa[12]);

    for(i=0;i<17;i++)
      rr[i] = (rr[i]+temp[i])/2.;

    free(temp);
  }
  return;
}

  /********************************************************
  ** input variables**
  *********************************************************

  *********arrays (1 value per range gate)*********
  t_d                         Irregualrity decay time s
  t_g                         irregularity growth time
  t_c                         precipitation time constant
  velo                        random velocity component (Gaussian)
  v_dop                       Background velocity (m/s)
  amp0                        amplitude scaling factor
  qflg                        array indicating which range gates contain scatter
  ********other arrays**********
  pulse_t                     array containing pulse table
  tau                         array containing the lags of the pulse sequence
  ********scalars*************
  freq                        transmit frequency
  noise_flg                   flag to indicate whether white noise is included
  noise_lev                   white noise level (ratio)
  nave                        number of pulse sequences in an integration period
  nrang                       number of range gates
  lagfr                       lag to first range
  life_dist                   lifetime distribution
  smsep                       sample spearation
  cpid                        control program ID number
  n_samples                   Number of datapoints in a single pulse sequence
  lagfr                       number of samples after pulse range gate 0 is
  n_pul                       number of pulses in the pulse sequence
  n_lags                      number of lags in the pulse sequence
  dt                          basic lag time for the pulse sequence
  cri_flg                     cross-range interference flag
  ********outputs***********
  out_samples                 the raw voltage samples (only valid if cri_flg=0)
  out_acfs                    the calculated ACFs, always valid

*/
void sim_data(double *t_d, double *t_g, double *t_c, double *v_dop, int * qflg,
              double *velo, double *amp0, double freq, double noise_lev,
              int noise_flg, int nave, int nrang, int lagfr,
              double smsep, int cpid, int life_dist,
              int n_pul, int cri_flg, int n_lags, int * pulse_t, int * tau,
              double dt, complex double * out_samples, complex double ** out_acfs,int decayflg)
{

  /********************************************************
  ** definitions of variables needed for data generation **
  ********************************************************/
  int n = 2000;                             /*Number of scatterers (per range gate per integration period)*/
  double t_n = dt*1.e-2;                    /*Irregularity decay time for white noise*/
  double c = 3.e8;                          /*Speed of light (m/s)*/
  double pwrtot = 0.;                       /*total power, used to normalize ACFs*/
  long numtot = 0;                          /*number of good ACFs*/
  double npwrtot = 0.;                      /*total noise power, used to normalize noise ACFs*/
  long nnumtot = 0;                         /*number of good noise ACFs*/
  long offset = 3300;                       /*time offset (in samples, ~1s) to allow irregularity*/
                                            /*generation/decay to reach steady state*/
  double rngsep = smsep*c/2.;               /*range gate spearation*/
  double smptime;                           /*time a raw sample is recorded*/
  long n_samples = (long)(pulse_t[n_pul-1]*dt/smsep+nrang+lagfr);      /*number of samples in 1 pulse sequence*/

  /*other variables*/
  long i,j,kk,p,r,smpnum,temp;
  double rng = 0,taus,amplitude,phase;
  long seed = time(NULL)*time(NULL);        /*a seed for random number generation*/

  double lambda = c/freq;

  /*control program dependent variables*/
  taus = dt/smsep;                                      /*lag time in samples*/

  complex double * aa = malloc(n_pul*sizeof(complex double));
  complex double * rr = malloc(n_lags*sizeof(complex double));

  /*Creating the output array for ACFs*/
  complex double ** acfs = malloc(nrang*sizeof(complex double *));
  for(i=0;i<nrang;i++)
  {
    acfs[i] = malloc(n_lags*sizeof(complex double));
    for(j=0;j<n_lags;j++)
      acfs[i][j] = 0.+I*0.;
  }

  /*Creating the output array for ACFs*/
  complex double ** noise_acfs = malloc(nrang*sizeof(complex double *));
  for(i=0;i<nrang;i++)
  {
    noise_acfs[i] = malloc(n_lags*sizeof(complex double));
    for(j=0;j<n_lags;j++)
      noise_acfs[i][j] = 0.+I*0.;
  }


  /*loop that populates the range gates with irregularities,
   *each with a random location and generation time*/
  struct irreg ** range_gates = malloc(nrang*sizeof(struct irreg *));
  for(i=0;i<nrang;i++)
    range_gates[i] = malloc(n*sizeof(struct irreg));



  /*create a structure to store the raw samples from each pulse sequence*/
  complex double * raw_samples = malloc(n_samples*sizeof(complex double));
  /*create a structure to store the raw noise samples from each pulse sequence*/
  complex double * noise_samples = malloc(n_samples*sizeof(complex double));


  /*assign properties to the irregularities in each range gate*/
  for(i=0;i<nrang;i++)
  {
    rng = (lagfr+i)*rngsep;
    for(j=0;j<n;j++)
    {
      srand(seed--);
      /*Uniform distribution of irregularities across 45 km range bin at distance rng*/
      range_gates[i][j].space =  ((double)rand()/(double)RAND_MAX)*rngsep+rng;
      /*random generation time for irregularities*/
      range_gates[i][j].time = ((double)rand()/(double)RAND_MAX)*(n_samples*nave+offset-1)*smsep;
      /*exponential*/
      if(!life_dist)
        range_gates[i][j].width = -1.*log((double)rand()/(double)RAND_MAX);       /*Exponential distribution around the average*/
      /*constant*/
      else
        range_gates[i][j].width = 1.;
      range_gates[i][j].start_time = -1.;
      temp = rand();
      /*random velocity component (gaussian)*/
      range_gates[i][j].velo = velo[i]*gasdev(&temp);
    }
  }

  /***********************************
  ***Perform the actual simulation****
  ***********************************/
  /*if cross-range interference is to be included in the simulation*/
  if(cri_flg)
  {
    for(kk=0;kk<nave;kk++)
    {
      /*initialize the samples*/
      for(i=0;i<n_samples;i++)
        raw_samples[i] = 0.+I*0.;

      /*sample the irregularities with each pulse of the sequence*/
      for(p=0;p<n_pul;p++)
        for(r=0;r<nrang;r++)
        {
          if(!qflg[r]) continue;
          /*sample number in the pulse sequence*/
          smpnum = pulse_t[p]*taus+r+lagfr;
          /*time that the sample is recorded*/
          smptime = (smpnum+kk*pulse_t[n_pul-1]*taus+nrang+lagfr+offset)*smsep;
					fprintf(stderr,"%d  %d  %lf\n",kk,smpnum,smptime);
          /*calculate how much signal is generated by
          each scatterer in this range gate*/
          for(i=0;i<n;i++)
          {
            /*check if the irregularity exists at the sample time,
              and hasnt surpassed its maximum lifetime*/
            if(range_gates[r][i].time <= smptime  &&
                smptime <= range_gates[r][i].time+t_c[r]*range_gates[r][i].width)
            {
              /*calculate the amplitude of the signal generated by the scatterer*/
              amplitude = (1.-exp(-(smptime-range_gates[r][i].time)/t_g[r]))*exp(-((smptime-range_gates[r][i].time)/t_d[r]));
            }
            else
              amplitude=0.;
            /*calculate phase of scattered signal depending on its location*/
            phase = -4.*PI*(smptime*(v_dop[r]+range_gates[r][i].velo)+range_gates[r][i].space)/lambda;
            /*record scattered signal as a raw sample*/
            raw_samples[smpnum] += amplitude*(cos(phase) + I*sin(phase));
          }
        }

      /*calculate an ACF from the raw samples*/
      for(r=0;r<nrang;r++)
      {
        /*calculate the ACF*/
        for(p=0;p<n_pul;p++)
        {
          smpnum = pulse_t[p]*taus+r+lagfr;
          aa[p] = raw_samples[smpnum];
        }
        acf_27(aa,rr,cpid);
        /*add this ACF to the total ACF*/
        for(i=0;i<n_lags;i++)
          acfs[r][i] += rr[i];
        if(r < pulse_t[1]*taus+lagfr && qflg[r])
        {
          pwrtot += creal(rr[0]);
          numtot++;
        }
      }
      /*save raw samples for output*/
      for(r=kk*n_samples;r<(kk+1)*n_samples-1;r++)
        out_samples[r] = raw_samples[(r%n_samples)];
    }
  }
  /*if no cross-range interference is to be included in the simulation*/
  else
  {
    for(r=0;r<nrang;r++)
    {
      for(kk=0;kk<nave;kk++)
      {
        /*initialize the samples*/
        for(i=0;i<n_samples;i++)
          raw_samples[i] = 0.+I*0.;

        /*sample the irregularities with each pulse of the sequence*/
        for(p=0;p<n_pul;p++)
        {
          if(!qflg[r]) continue;
          /*sample number in the pulse sequence*/
          smpnum = pulse_t[p]*taus+r+lagfr;
          /*time that the sample is recorded*/
          smptime = (smpnum+kk*pulse_t[n_pul-1]*taus+nrang+lagfr+offset)*smsep;
          /*calculate how much signal is generated by
          each scatterer in this range gate*/
          for(i=0;i<n;i++)
          {
            /*check if the irregularity exists at the sample time,
              and hasnt surpassed its maximum lifetime*/
            if(range_gates[r][i].time <= smptime  &&
                smptime <= range_gates[r][i].time+t_c[r]*range_gates[r][i].width)
            {
              /*calculate the amplitude of the signal generated by the scatterer*/
              amplitude = (1.-exp(-(smptime-range_gates[r][i].time)/t_g[r]))*exp(-((smptime-range_gates[r][i].time)/t_d[r]));
            }
            else
              amplitude=0.;
            /*calculate phase of scattered signal depending on its location*/
            phase = -4.*PI*(smptime*(v_dop[r]+range_gates[r][i].velo)+range_gates[r][i].space)/lambda;
            /*record scattered signal as a raw sample*/
            raw_samples[smpnum] += amplitude*(cos(phase) + I*sin(phase));
          }
        }
        /*calculate an ACF from the raw samples*/
        for(p=0;p<n_pul;p++)
        {
          smpnum = pulse_t[p]*taus+r+lagfr;
          aa[p] = raw_samples[smpnum];
        }
        acf_27(aa,rr,cpid);
        /*add this ACF to the total ACF*/
        for(i=0;i<n_lags;i++)
          acfs[r][i] += rr[i];
        if(r < pulse_t[1]*taus+lagfr && qflg[r])
        {
          pwrtot += creal(rr[0]);
          numtot++;
        }
      }
    }
  }
  /*if noise is to be included in the simulation*/
  if(noise_flg)
  {
    for(r=0;r<nrang;r++)
    {
      for(kk=0;kk<nave;kk++)
      {
        /*initialize the samples*/
        for(i=0;i<n_samples;i++)
          noise_samples[i] = 0.+I*0.;

        /*sample the irregularities with each pulse of the sequence*/
        for(p=0;p<n_pul;p++)
        {
          /*sample number in the pulse sequence*/
          smpnum = pulse_t[p]*taus+r+lagfr;
          /*time that the sample is recorded*/
          smptime = (smpnum+kk*pulse_t[n_pul-1]*taus+nrang+lagfr+offset)*smsep;
          /*calculate how much signal is generated by
          each scatterer in this range gate*/
          for(i=0;i<n;i++)
          {
            /*check if the irregularity exists at the sample time,
              and hasnt surpassed its maximum lifetime*/
            if(range_gates[r][i].time <= smptime)
            {
              if(range_gates[r][i].start_time == -1.)
                range_gates[r][i].start_time = smptime;
              /*calculate the amplitude of the signal generated by the scatterer*/
              amplitude = exp(-((smptime-range_gates[r][i].start_time)/t_n));
            }
            else
              amplitude=0.;
            /*calculate phase of scattered signal depending on its location*/
            phase = -4.*PI*(range_gates[r][i].space)/lambda;
            /*record scattered signal as a raw sample*/
            noise_samples[smpnum] += amplitude*(cos(phase) + I*sin(phase));
          }
        }
        /*calculate an ACF from the raw samples*/
        for(p=0;p<n_pul;p++)
        {
          smpnum = pulse_t[p]*taus+r+lagfr;
          aa[p] = noise_samples[smpnum];
        }
        acf_27(aa,rr,cpid);
        /*add this ACF to the total ACF*/
        for(i=0;i<n_lags;i++)
          noise_acfs[r][i] += rr[i];
        if(r < pulse_t[1]*taus+lagfr)
        {
          npwrtot += creal(rr[0]);
          nnumtot++;
        }
      }
    }
  }

  /*average and normalize ACFs*/
  for(r=0;r<nrang;r++)
	{
    for(i=0;i<n_lags;i++)
    {
      if(decayflg) out_acfs[r][i] = 1./(pow(r+1,2))*acfs[r][i]*amp0[r]/(nave*pwrtot/numtot);
			else out_acfs[r][i] = acfs[r][i]*amp0[r]/(nave*pwrtot/numtot);
      if(noise_flg)
      {
        noise_acfs[r][i] *= noise_lev*amp0[r]/(nave*npwrtot/nnumtot);
        out_acfs[r][i] += noise_acfs[r][i];
      }
    }
	}

  /*free dynamically allocated memory*/
  for(i=0;i<nrang;i++)
    free(acfs[i]);
  free(acfs);
  for(i=0;i<nrang;i++)
    free(noise_acfs[i]);
  free(noise_acfs);
  free(aa);
  for(i=0;i<nrang;i++)
    free(range_gates[i]);
  free(range_gates);
  free(rr);
  free(raw_samples);
  free(noise_samples);

  return;
}