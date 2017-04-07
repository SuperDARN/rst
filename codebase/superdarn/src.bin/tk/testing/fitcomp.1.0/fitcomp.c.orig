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



struct RadarParm *prm;
struct RawData *raw;
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

  FILE *fp=NULL;

  int c,n;
  char command[128];
	double lambda;

  prm=RadarParmMake();
  raw=RawMake();
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
	
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"new",'x',&new);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;

  if (help==1)
	{
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1)
	{
    OptionDump(stdout,&opt);
    exit(0);
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


	if (arg==argc) fp=stdin;
	else fp=fopen(argv[arg],"r");

	if (fp==NULL)
	{
		fprintf(stderr,"File not found.\n");
		exit(-1);
	}
	status=RawFread(fp,prm,raw);

  radar=RadarGetRadar(network,prm->stid);
  if (radar==NULL)
	{
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,
		          prm->time.dy,prm->time.hr,prm->time.mt,
                          prm->time.sc);

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



  if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);

  fblklm=FitACFMake(site,prm->time.yr);
	fblkacf=FitACFMake(site,prm->time.yr);
	fblkex=FitACFMake(site,prm->time.yr);

	FitACF(prm,raw,fblkacf,fitacf);
	lmfit(prm,raw,fitlm,fblklm,0);
	fitacfex2(prm,raw,fitex,fblkex,0);

	n = 0;
	int print=0,n2=0;
  do
  {
		sscanf(prm->combf,"%*c%*c%*c%*c%*c%*c%*c%*c %*c%*c%*c%*c %d %*c%*c%*c%*c %d %*c%*c%*c%*c %d %*s\n",&vel,&t_d,&amp);
		lambda = 3.e8/(prm->tfreq*1.e3);

		/*go thought all of the range gates*/
		for(i=0;i<25;i++)
		{
			/*check for 3 good fits*/
			if(fitacf->rng[i].qflg && fitlm->rng[i].qflg && fitex->rng[i].qflg && fitacf->rng[i].v != 0.)
			{
				n++;
				vbin = (vel/100);
				tbin = (t_d/10)-1;
				/*first, MSE velocity*/
				errors[vbin][tbin][0][0] += pow((double)vel-fitacf->rng[i].v,2); /*fitacf*/
				errors[vbin][tbin][1][0] += pow((double)vel-fitex->rng[i].v,2); /*fitex2*/
				errors[vbin][tbin][2][0] += pow((double)vel-fitlm->rng[i].v,2); /*lmfit*/

				/*second, absolute velocity*/
				errors[vbin][tbin][0][1] += (double)vel-fitacf->rng[i].v; /*fitacf*/
				errors[vbin][tbin][1][1] += (double)vel-fitex->rng[i].v; /*fitex2*/
				errors[vbin][tbin][2][1] += (double)vel-fitlm->rng[i].v; /*lmfit*/

				/*third, RMS decay time*/
				errors[vbin][tbin][0][2] += pow((double)t_d*1.e-3-lambda/(fitacf->rng[i].w_l*2.*3.14159),2); /*fitacf*/
				errors[vbin][tbin][1][2] += pow((double)t_d*1.e-3-lambda/(fitex->rng[i].w_l*2.*3.14159),2); /*fitex2*/
				errors[vbin][tbin][2][2] += pow((double)t_d*1.e-3-lambda/(fitlm->rng[i].w_l*2.*3.14159),2); /*lmfit*/

				/*fourth, absolute decay time*/
				errors[vbin][tbin][0][3] += (double)t_d*1.e-3-lambda/(fitacf->rng[i].w_l*2.*3.14159); /*fitacf*/
				errors[vbin][tbin][1][3] += (double)t_d*1.e-3-lambda/(fitex->rng[i].w_l*2.*3.14159); /*fitex2*/
				errors[vbin][tbin][2][3] += (double)t_d*1.e-3-lambda/(fitlm->rng[i].w_l*2.*3.14159); /*lmfit*/

				if(fabs((double)vel-fitacf->rng[i].v) > 500)
				{
					fprintf(stderr,"%lf  %lf\nfitacf %lf  %lf  \nfitex2  %lf  %lf\nlmfit  %lf  %lf\n",(double)vel,t_d*1.e-3,
									fitacf->rng[i].v,(double)vel-fitacf->rng[i].v,
									fitex->rng[i].v,(double)vel-fitex->rng[i].v,
									fitlm->rng[i].v,(double)vel-fitlm->rng[i].v);
									n2++;
				}


			}
		}


    status=RawFread(fp,prm,raw);

    if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);/*

    if (prm->time.hr==20 && prm->time.mt==44 && prm->time.sc == 35)
			print=1;*/
    if (status==0)
		{
			lmfit(prm,raw,fitlm,fblklm,0);
			FitACF(prm,raw,fblkacf,fitacf);
			fitacfex2(prm,raw,fitex,fblkex,0);
		}

  } while (status==0 && print==0);

	for(i=0;i<20;i++)
		for(j=0;j<10;j++)
<<<<<<< HEAD
			for(k=1;k<2;k++)
=======
			for(k=0;k<4;k++)
>>>>>>> fitcomp
				fprintf(stdout,"%f  %f  %lf  %lf  %lf\n",i*100.+50.,(j+1)*1.e-2,errors[i][j][0][k]/n,errors[i][j][1][k]/n,errors[i][j][2][k]/n);

	fprintf(stderr,"%d  %d\n",n,n2);

	FitACFFree(fblkacf);
  FitACFFree(fblklm);
	FitACFFree(fblkex);
  return 0;
}













