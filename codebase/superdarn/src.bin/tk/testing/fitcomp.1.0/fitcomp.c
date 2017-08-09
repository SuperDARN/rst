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
 Written by AJ Ribeiro 08/03/2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <zlib.h>

#include "rtypes.h"
#include "option.h"
 
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"

#include "lmfit.h"
#include "fitacf.h"
#include "rawread.h"
#include "fitwrite.h"

#include "oldrawread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"
#include "fitacfex2.h"


struct RadarParm *prm1;
struct RawData *raw1;
struct RadarParm *prm2;
struct RawData *raw2;
struct RadarParm *prm3;
struct RawData *raw3;
struct FitData *fitlm;
struct FitData *fitacf;
struct FitData *fitex;
struct FitBlock *fblklm;
struct FitBlock *fblkacf;
struct FitBlock *fblkex;

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

struct OptionData opt;

double calc_acf_err(struct RawData * raw, struct FitData * fit, struct RadarParm * prm, int i)
{
	double acferr, tau, lag, re, im, ref, imf;
	int j;
	double lambda = 3e8/(prm->tfreq*1e3);
	double t_if = lambda/(fit->rng[i].w_l*2.*3.14159);
	double f_if = 4.*3.14159*fit->rng[i].v/lambda;
	double noise = prm->noise.mean;
	double p0 = noise*pow(10.,fit->rng[i].p_l/10.)-noise;
	acferr = 0.;
	for(j=0;j<prm->mplgs;j++)
	{
		lag = fabs(prm->lag[0][j]-prm->lag[1][j]);
		tau = lag*prm->mpinc*1.e-6;
		re = raw->acfd[0][i*prm->mplgs+j]/(10000.);
		im = raw->acfd[1][i*prm->mplgs+j]/(10000.);
		ref = p0*exp(-1.0*tau/t_if)*cos(tau*f_if)/(10000.);
		imf = p0*exp(-1.0*tau/t_if)*sin(tau*f_if)/(10000.);
		acferr += (pow(re-ref,2) + pow(im-imf,2));
	}
	acferr /= prm->mplgs;
	acferr = sqrt(acferr);
	return acferr;
}

int main(int argc,char *argv[]) 
{

  /* File format transistion
   * ------------------------
   *
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  unsigned char old=0;
  unsigned char new=0;

  char *envstr;
  int status;
  int arg,i,vel,t_d,amp;
	int j,k,l,vbin,tbin;

  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  FILE *fpa=NULL;
	FILE *fpb=NULL;
	FILE *fpc=NULL;
	FILE * fp = NULL;

  int c,n, lag;
  char command[128];
	double lambda, t_if, f_if, p0, noise, acferr, re, im, ref, imf, tau;

  prm1=RadarParmMake();
  raw1=RawMake();
  prm2=RadarParmMake();
  raw2=RawMake();
  prm3=RadarParmMake();
  raw3=RawMake();
  fitacf=FitMake();
	fitex=FitMake();
	fitlm=FitMake();

	double **** errors = malloc(20*sizeof(double ***));
	for(i=0;i<20;i++)
	{
		errors[i] = malloc(10*sizeof(double **));
		for(j=0;j<10;j++)
		{
			errors[i][j] = malloc(3*sizeof(double *));
			for(k=0;k<3;k++)
			{
				errors[i][j][k] = malloc(5*sizeof(double));
				for(l=0;l<5;l++)
					errors[i][j][k][l] = 0.;
			}
		}
	}

	double *** num = malloc(20*sizeof(double **));
	for(i=0;i<20;i++)
	{
		num[i] = malloc(10*sizeof(double * ));
		for(j=0;j<10;j++)
		{
			num[i][j] = malloc(3*sizeof(double));
			for(k=0;k<3;k++)
				num[i][j][k] = 0.;
		}
	}


  envstr=getenv("SD_RADAR");
  if (envstr==NULL)
	{
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL)
	{
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp);
  if (network==NULL)
	{
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL)
	{
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }
  RadarLoadHardware(envstr,network);

	char *filename;
	filename = argv[argc-1];
	char * cmd = malloc(snprintf(NULL, 0, "%s %s %s", " cp", filename, " > f1.rawacf") + 2);
	sprintf(cmd, "%s %s %s", "cp", filename, " f1.rawacf");
	system(cmd);

	sprintf(cmd, "%s %s %s", "cp", filename, " f2.rawacf");
	system(cmd);

	sprintf(cmd, "%s %s %s", "cp", filename, " f3.rawacf");
	system(cmd);
	
	free(cmd);
	
	fpa=fopen("f1.rawacf","r");
	fpb=fopen("f2.rawacf","r");
	fpc=fopen("f3.rawacf","r");

	if (fpa==NULL || fpb==NULL || fpc==NULL)
	{
		fprintf(stderr,"File not found.\n");
		exit(-1);
	}
	status=RawFread(fpa,prm1,raw1);
	status=RawFread(fpb,prm2,raw2);
	status=RawFread(fpc,prm3,raw3);


  radar=RadarGetRadar(network,prm1->stid);
  if (radar==NULL)
	{
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm1->time.yr,prm1->time.mo,
		          prm1->time.dy,prm1->time.hr,prm1->time.mt,
                          prm1->time.sc);


  if (site==NULL)
	{
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }


  command[0]=0;
  n=0;
  for (c=0;c<argc;c++)
	{
    n+=strlen(argv[c])+1;
    if (n>127) break; 
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }

	double lm_v_err[100000],ex_v_err[100000],acf_v_err[100000];
	long n_acf=0, n_lm=0,n_ex=0;
	double mean_v_acf,mean_v_lm,mean_v_ex;

	double lm_t_err[100000],ex_t_err[100000],acf_t_err[100000];
	double mean_t_acf,mean_t_lm,mean_t_ex;

	double lm_f_err[100000],ex_f_err[100000],acf_f_err[100000];
	double mean_f_acf,mean_f_lm,mean_f_ex;

  if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm1->time.yr,prm1->time.mo,
	     prm1->time.dy,prm1->time.hr,prm1->time.mt,prm1->time.sc,prm1->bmnum);

	fblkacf=FitACFMake(site,prm1->time.yr);
	fblkex=FitACFMake(site,prm2->time.yr);
	fblklm=FitACFMake(site,prm3->time.yr);

	FitACF(prm1,raw1,fblkacf,fitacf);
	fitacfex2(prm2,raw2,fitex,fblkex,0);
	lmfit(prm3,raw3,fitlm,fblklm,0);

	int ** very_bad = malloc(20*sizeof(int *));
	for(i=0;i<20;i++)
	{
		very_bad[i] = malloc(3*sizeof(int));
		for(j=0;j<3;j++)
			very_bad[i][j] = 0;
	}

	n = 0;
	int print=0;
	int one=0, two=0, three=0;
  do
  {
		/*read the simulation params*/
		sscanf(prm1->combf,"%*c%*c%*c%*c%*c%*c%*c%*c %*c%*c%*c%*c %d %*c%*c%*c%*c %d %*c%*c%*c%*c %d %*s\n",&vel,&t_d,&amp);
		lambda = 3.e8/(prm1->tfreq*1.e3);


		/*go through all of the range gates*/
		for(i=0;i<10;i++)
		{
			/*fitacf error calculations*/
			if(fitacf->rng[i].qflg == 1)
			{
				vbin = (vel/100);
				tbin = (t_d/10)-1;
				num[vbin][tbin][0] += 1.;

				acf_v_err[n_acf] = fabs((double)vel-fitacf->rng[i].v);
				acf_t_err[n_acf] = fabs((double)t_d*1.e-3-lambda/(fitacf->rng[i].w_l*2.*3.14159))/((double)t_d*1.e-3);

				/*rms velocity*/
				errors[vbin][tbin][0][0] += pow((double)vel-fitacf->rng[i].v,2);
				/*true velocity*/
				errors[vbin][tbin][0][1] += fitacf->rng[i].v-(double)vel;
				errors[vbin][tbin][0][2] += pow((double)t_d*1.e-3-lambda/(fitacf->rng[i].w_l*2.*3.14159),2);


				if(pow((double)t_d*1.e-3-lambda/(fitacf->rng[i].w_l*2.*3.14159),2) > 999999.) exit(-1);
				/*calc ACFERR
				t_if = lambda/(fitacf->rng[i].w_l*2.*3.14159);
				f_if = 4.*3.14159*fitacf->rng[i].v/lambda;
				noise = fitacf->rng[i].phi0_err;
				p0 = noise*pow(10.,fitacf->rng[i].p_l/10.)-noise;
				acferr = 0.;
				for(j=0;j<prm1->mplgs;j++)
				{
					lag = fabs(prm1->lag[0][j]-prm1->lag[1][j]);
					tau = lag*prm1->mpinc*1.e-6;
					re = raw1->acfd[0][i*prm1->mplgs+j]/(10000./pow(i+1,2));
					im = raw1->acfd[1][i*prm1->mplgs+j]/(10000./pow(i+1,2));
					ref = p0*exp(-1.0*tau/t_if)*cos(tau*f_if)/(10000./pow(i+1,2));
					imf = p0*exp(-1.0*tau/t_if)*sin(tau*f_if)/(10000./pow(i+1,2));
					acferr += (pow(re-ref,2) + pow(im-imf,2));
				}
				acferr /= prm1->mplgs;
				acferr = sqrt(acferr);*/
				acferr = calc_acf_err(raw1,fitacf,prm1,i);
				errors[vbin][tbin][0][4] += acferr;
				acf_f_err[n_acf] = acferr;
				n_acf++;
			}
			if(fitex->rng[i].qflg)
			{
				vbin = (vel/100);
				tbin = (t_d/10)-1;
				num[vbin][tbin][1] += 1.;

				ex_v_err[n_ex] = fabs((double)vel-fitex->rng[i].v);
				ex_t_err[n_ex] = fabs((double)t_d*1.e-3-lambda/(fitex->rng[i].w_l*2.*3.14159))/((double)t_d*1.e-3);

				errors[vbin][tbin][1][0] += pow((double)vel-fitex->rng[i].v,2); /*fitex2*/
				errors[vbin][tbin][1][1] += fitex->rng[i].v-(double)vel;  /*fitex2*/
				errors[vbin][tbin][1][2] += pow((double)t_d*1.e-3-lambda/(fitex->rng[i].w_l*2.*3.14159),2); /*fitex2*/
				errors[vbin][tbin][1][3] += lambda/(fitex->rng[i].w_l*2.*3.14159)-(double)t_d*1.e-3; /*fitex2*/

				/*fitex2*/
				t_if = lambda/(fitex->rng[i].w_l*2.*3.14159);
				f_if = 4.*3.14159*fitex->rng[i].v/lambda;
				noise = prm2->noise.mean;
				p0 = noise*pow(10.,fitex->rng[i].p_l/10.)-noise;
				acferr = 0.;
				for(j=0;j<prm2->mplgs;j++)
				{
					lag = fabs(prm2->lag[0][j]-prm2->lag[1][j]);
					tau = lag*prm2->mpinc*1.e-6;
					re = raw2->acfd[0][i*prm2->mplgs+j]/(10000./pow(i+1,2));
					im = raw2->acfd[1][i*prm2->mplgs+j]/(10000./pow(i+1,2));
					ref = p0*exp(-1.0*tau/t_if)*cos(tau*f_if)/(10000./pow(i+1,2));
					imf = p0*exp(-1.0*tau/t_if)*sin(tau*f_if)/(10000./pow(i+1,2));
					acferr += (pow(re-ref,2) + pow(im-imf,2));
				}
				acferr /= prm2->mplgs;
				acferr = sqrt(acferr);
				acferr = calc_acf_err(raw2,fitex,prm2,i);
				errors[vbin][tbin][1][4] += acferr;

				ex_f_err[n_ex] = acferr;
				n_ex++;
			}
			if(fitlm->rng[i].qflg)
			{
				vbin = (vel/100);
				tbin = (t_d/10)-1;
				num[vbin][tbin][2] += 1.;

				lm_v_err[n_lm] = fabs((double)vel-fitlm->rng[i].v);
				lm_t_err[n_lm] = fabs((double)t_d*1.e-3-lambda/(fitlm->rng[i].w_l*2.*3.14159))/((double)t_d*1.e-3);

				errors[vbin][tbin][2][0] += pow((double)vel-fitlm->rng[i].v,2); /*lmfit*/
				errors[vbin][tbin][2][1] += fitlm->rng[i].v-(double)vel;  /*lmfit*/
				errors[vbin][tbin][2][2] += pow((double)t_d*1.e-3-lambda/(fitlm->rng[i].w_l*2.*3.14159),2); /*lmfit*/
				errors[vbin][tbin][2][3] += lambda/(fitlm->rng[i].w_l*2.*3.14159)-(double)t_d*1.e-3; /*lmfit*/


				/*lmfit
				t_if = lambda/(fitlm->rng[i].w_l*2.*3.14159);
				f_if = 4.*3.14159*fitlm->rng[i].v/lambda;
				noise = prm3->noise.mean;
				p0 = noise*pow(10.,fitlm->rng[i].p_l/10.)-noise;
				acferr = 0.;
				for(j=0;j<prm3->mplgs;j++)
				{
					lag = fabs(prm3->lag[0][j]-prm3->lag[1][j]);
					tau = lag*prm3->mpinc*1.e-6;
					re = raw1->acfd[0][i*prm3->mplgs+j]/(10000./pow(i+1,2));
					im = raw1->acfd[1][i*prm3->mplgs+j]/(10000./pow(i+1,2));
					ref = p0*exp(-1.0*tau/t_if)*cos(tau*f_if)/(10000./pow(i+1,2));
					imf = p0*exp(-1.0*tau/t_if)*sin(tau*f_if)/(10000./pow(i+1,2));
					acferr += (pow(re-ref,2) + pow(im-imf,2));
				}
				acferr /= prm3->mplgs;
				acferr = sqrt(acferr);*/
				acferr = calc_acf_err(raw3,fitlm,prm3,i);
				errors[vbin][tbin][2][4] += acferr;
				lm_f_err[n_lm] = acferr;
				n_lm++;
			}
			/*check for 3 good fits*/
			if(fitacf->rng[i].qflg && fitex->rng[i].qflg && fitlm->rng[i].qflg )
			{
				n++;
				vbin = (vel/100);
				tbin = (t_d/10)-1;

				if(fabs(fitacf->rng[i].v-(double)vel) < fabs(fitex->rng[i].v-(double)vel) &&
					fabs(fitacf->rng[i].v-(double)vel) < fabs(fitlm->rng[i].v-(double)vel))
					one++;

				else if(fabs(fitex->rng[i].v-(double)vel) < fabs(fitacf->rng[i].v-(double)vel) &&
					fabs(fitex->rng[i].v-(double)vel) < fabs(fitlm->rng[i].v-(double)vel))
					two++;

				else if(fabs(fitlm->rng[i].v-(double)vel) < fabs(fitacf->rng[i].v-(double)vel) &&
					fabs(fitlm->rng[i].v-(double)vel) < fabs(fitex->rng[i].v-(double)vel))
					three++;
			}
		}

    status=RawFread(fpa,prm1,raw1);
		status=RawFread(fpb,prm2,raw2);
		status=RawFread(fpc,prm3,raw3);

    if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm1->time.yr,prm1->time.mo,
	     prm1->time.dy,prm1->time.hr,prm1->time.mt,prm1->time.sc,prm1->bmnum);

    if (status==0)
		{
			FitACF(prm1,raw1,fblkacf,fitacf);
			fitacfex2(prm2,raw2,fitex,fblkex,0);
			lmfit(prm3,raw3,fitlm,fblklm,0);
		}

  } while (status==0 && print==0);
	

	fprintf(stderr,"number of times each had the best vel fit:\nfitacf: %d  fitex2: %d  lmfit: %d  total # of fits: %d\n",one, two, three, n);

	qsort(acf_v_err, n_acf, sizeof(double), lm_dbl_cmp);
	qsort(ex_v_err, n_ex, sizeof(double), lm_dbl_cmp);
	qsort(lm_v_err, n_lm, sizeof(double), lm_dbl_cmp);

	for(i=0;i<n_acf;i++)
		mean_v_acf += acf_v_err[i];
	mean_v_acf /= (double)n_acf;

	for(i=0;i<n_ex;i++)
		mean_v_ex += ex_v_err[i];
	mean_v_ex /= (double)n_ex;

	for(i=0;i<n_lm;i++)
		mean_v_lm += lm_v_err[i];
	mean_v_lm /= (double)n_lm;

	qsort(acf_t_err, n_acf, sizeof(double), lm_dbl_cmp);
	qsort(ex_t_err, n_ex, sizeof(double), lm_dbl_cmp);
	qsort(lm_t_err, n_lm, sizeof(double), lm_dbl_cmp);

	for(i=0;i<n_acf;i++)
		mean_t_acf += acf_t_err[i];
	mean_t_acf /= (double)n_acf;

	for(i=0;i<n_ex;i++)
		mean_t_ex += ex_t_err[i];
	mean_t_ex /= (double)n_ex;

	for(i=0;i<n_lm;i++)
		mean_t_lm += lm_t_err[i];
	mean_t_lm /= (double)n_lm;

	qsort(acf_f_err, n_acf, sizeof(double), lm_dbl_cmp);
	qsort(ex_f_err, n_ex, sizeof(double), lm_dbl_cmp);
	qsort(lm_f_err, n_lm, sizeof(double), lm_dbl_cmp);

	for(i=0;i<n_acf;i++)
		mean_f_acf += acf_f_err[i];
	mean_f_acf /= (double)n_acf;

	for(i=0;i<n_ex;i++)
		mean_f_ex += ex_f_err[i];
	mean_f_ex /= (double)n_ex;

	for(i=0;i<n_lm;i++)
		mean_f_lm += lm_f_err[i];
	mean_f_lm /= (double)n_lm;

	fprintf(stderr,"           acf        ex        lm\n");
	fprintf(stderr,"number:    %d      %d      %d\n",n_acf,n_ex,n_lm);
	fprintf(stderr,"v median:  %lf   %lf   %lf\n",acf_v_err[(int)(n_acf/2)],ex_v_err[(int)(n_ex/2)],lm_v_err[(int)(n_lm/2)]);
	fprintf(stderr,"v mean:    %lf   %lf   %lf\n",mean_v_acf,mean_v_ex,mean_v_lm);
	fprintf(stderr,"t median:  %lf   %lf   %lf\n",acf_t_err[(int)(n_acf/2)],ex_t_err[(int)(n_ex/2)],lm_t_err[(int)(n_lm/2)]);
	fprintf(stderr,"t mean:    %lf   %lf   %lf\n",mean_t_acf,mean_t_ex,mean_t_lm);
	fprintf(stderr,"f median:  %lf   %lf   %lf\n",acf_f_err[(int)(n_acf/2)],ex_f_err[(int)(n_ex/2)],lm_f_err[(int)(n_lm/2)]);
	fprintf(stderr,"f mean:    %lf   %lf   %lf\n",mean_f_acf,mean_f_ex,mean_f_lm);

	for(i=0;i<20;i++)
		for(j=0;j<10;j++)
			for(k=0;k<5;k++)
				if(k % 2 == 0)
					fprintf(stdout,"%f  %f  %d  %lf  %lf  %lf  %lf\n",i*100.+50.,(j+1)*1.e-2,k,
									sqrt(errors[i][j][0][k]/num[i][j][0]),sqrt(errors[i][j][1][k]/num[i][j][1]),sqrt(errors[i][j][2][k]/num[i][j][2]),num[i][j][2]);
				else
					fprintf(stdout,"%f  %f  %d  %lf  %lf  %lf  %lf\n",i*100.+50.,(j+1)*1.e-2,k,
									errors[i][j][0][k]/num[i][j][0],errors[i][j][1][k]/num[i][j][1],errors[i][j][2][k]/num[i][j][2],num[i][j][2]);

	/*for(i=0;i<20;i++)
		fprintf(stderr,"%d  %d  %d  %d\n",i,very_bad[i][0],very_bad[i][1],very_bad[i][2]);

	fprintf(stderr,"%d  %d\n",n,n2);*/

	for(i=0;i<20;i++)
	{
		for(j=0;j<10;j++)
			free(num[i][j]);
		free(num[i]);
	}
	free(num[i]);

	for(i=0;i<20;i++)
	{
		for(j=0;j<10;j++)
		{
			for(k=0;k<3;k++)
				free(errors[i][j][k]);
			free(errors[i][j]);
		}
		free(errors[i]);
	}
	free(errors);

	free(very_bad);
	FitACFFree(fblkacf);
  FitACFFree(fblklm);
	FitACFFree(fblkex);
	
	system("rm f1.rawacf");
	system("rm f2.rawacf");
	system("rm f3.rawacf");
	
	
  return 0;
}













