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
#include "fitdata.h"
#include "fitread.h"

struct gates
{
  double vel;
	double wid;
	double pow;
};

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

/*this is a driver program from the data simulator*/

int main(int argc,char *argv[])
{

  /********************************************************
  ** definitions of variables needed for data generation **
  ********************************************************/
  double t_d = .04;                         /*Irregualrity decay time s*/
  double w = -9999.;                        /*spectral width*/
  double v_dop =450.;                       /*Background velocity (m/s)*/
  double c = 3.e8;                          /*Speed of light (m/s)*/
  double freq = 12.e6;                      /*transmit frequency*/
  double amp0 = 1.;                         /*amplitude scaling factor*/
  int noise_flg = 1;                        /*flag to indicate whether white noise is included*/
  double noise_lev = 0.;                    /*white noise level (ratio)*/
  int nave = 50;                            /*number of pulse sequences in an integration period*/
  int nrang = 100;                          /*number of range gates*/
  int lagfr = 4;                            /*lag to first range*/
  int life_dist = 0;                        /*lifetime distribution*/
  double smsep = 300.e-6;                   /*sample spearation*/
  double rngsep = 45.e3;                    /*range gate spearation*/
  int cpid = 150;                             /*control program ID number*/
  int n_samples;                            /*Number of datapoints in a single pulse sequence*/
  int n_pul,n_lags,*pulse_t,*tau;
  double dt;                                /*basic lag time*/
  int cri_flg = 1;                          /*cross-range interference flag*/
  int smp_flg = 0;                          /*output raw samples flag*/
  int decayflg = 0;

  /*other variables*/
  long i,j;
  double taus;

	/*fit file to recreate*/
	char * filename = argv[argc-1],*envstr;

	FILE *fp, *fitfp;

  /*fill the parameter structure*/
  struct FitData *fit = FitMake();
  struct RadarParm *prm = RadarParmMake();
	
	envstr=getenv("SD_RADAR");
  if(envstr==NULL)
  {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    return -1;
  }
  fp=fopen(envstr,"r");

  if(fp==NULL)
  {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp);
  if(network==NULL)
  {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if(envstr==NULL)
  {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

	/*read the radar's file*/
  fitfp=fopen(argv[argc-1],"r");
  fprintf(stderr,"%s\n",argv[argc-1]);
  if(fitfp==NULL)
  {
    fprintf(stderr,"File %s not found.\n",argv[argc-1]);
    exit(-1);
  }

	int s=FitFread(fitfp,prm,fit);

  radar=RadarGetRadar(network,prm->stid);
  if (radar==NULL)
  {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }
  site=RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc);
	
	/*array with the irregularity decay time for each range gate*/
	double * t_d_arr = malloc(nrang*sizeof(double));
	for(i=0;i<nrang;i++)
		t_d_arr[i] = 0;

	/*array with the irregularity growth time for each range gate*/
	double * t_g_arr = malloc(nrang*sizeof(double));
	for(i=0;i<nrang;i++)
		t_g_arr[i] = 1.e-6;

	/*array with the irregularity lifetime for each range gate*/
	double * t_c_arr = malloc(nrang*sizeof(double));
	for(i=0;i<nrang;i++)
		t_c_arr[i] = 1000;

	/*array with the irregularity doppler velocity for each range gate*/
	double * v_dop_arr = malloc(nrang*sizeof(double));
	for(i=0;i<nrang;i++)
		v_dop_arr[i] = 0;

	/*array with the irregularity doppler velocity for each range gate*/
	double * velo_arr = malloc(nrang*sizeof(double));
	for(i=0;i<nrang;i++)
		velo_arr[i] = 0;

	/*array with the ACF amplitude for each range gate*/
	double * amp0_arr = malloc(nrang*sizeof(double));
	for(i=0;i<nrang;i++)
		amp0_arr[i] = 0;
	
	/*flags to tell which range gates contain scatter*/
	int * qflg = malloc(nrang*sizeof(int));
	for(i=0;i<nrang;i++)
		qflg[i] = 0;

	/*Creating the output array for ACFs*/
	complex double ** acfs = malloc(nrang*sizeof(complex double *));


	do
	{

		fprintf(stderr,"%d  %d  %d  %d  %d  %d\n",prm->time.yr,prm->time.mo,prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc);

		double lambda = c/(prm->tfreq*1e3);

		/*oldscan*/
		if(cpid == 1)
		{
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
			for(i=0;i<n_lags;i++)
				tau[i] = i;
			/*no lag 16*/
			tau[16] += 1;
			tau[17] += 1;
		}
		/*tauscan*/
		else if(cpid == 503 || cpid == -3310)
		{
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
			for(i=0;i<10;i++)
				tau[i] = i;
			/*no lag 10*/
			for(i=10;i<18;i++)
				tau[i] = (i+1);
		}
		/*katscan (default)*/
		else
		{
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

		/*create a structure to store the raw samples from each pulse sequence*/
		complex double * raw_samples = malloc(n_samples*nave*sizeof(complex double));

		/**********************************************************
		****FILL THESE ARRAYS WITH THE SIMULATION PARAMETERS*******
		**********************************************************/
		for(i=0;i<nrang;i++)
		{
			t_d = lambda/(fit->rng[i].w_l*2.*PI);
			if(t_d > 999999.) t_d = 0.;
			t_d_arr[i] = t_d;

			v_dop_arr[i] = fit->rng[i].v;

			amp0 = prm->noise.search*pow(10.,(fit->rng[i].p_l/10.));
			amp0_arr[i] = amp0;

			acfs[i] = malloc(n_lags*sizeof(complex double));
			for(j=0;j<n_lags;j++)
				acfs[i][j] = 0.+I*0.;
			
			qflg[i] = fit->rng[i].qflg;
		}
		
		/*call the simulation function*/
		sim_data(t_d_arr, t_g_arr, t_c_arr, v_dop_arr, qflg, velo_arr, amp0_arr, prm->tfreq*1e3, prm->noise.search,
							1, prm->nave, prm->nrang, prm->lagfr/prm->smsep, prm->smsep*1e-6, prm->cp, 0,
							prm->mppul, 1, n_lags, pulse_t, tau, prm->mpinc*1e-6, raw_samples, acfs, 0);

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

		free(pulse_t);
		free(tau);
		free(raw_samples);
		for(i=0;i<nrang;i++)
			free(acfs[i]);
	} while(FitFread(fitfp,prm,fit) != -1);

  /*free dynamically allocated memory*/
  for(i=0;i<nrang;i++)
    free(acfs[i]);
  free(acfs);
  free(qflg);
  free(t_d_arr);
  free(t_g_arr);
  free(t_c_arr);
  free(v_dop_arr);
  free(velo_arr);
  free(amp0_arr);
	fclose(fitfp);


  return 0;
}
