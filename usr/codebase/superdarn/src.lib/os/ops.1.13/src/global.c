/* global.c
   ======== 
   Author: R.J.Barnes, J.Spaleta
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "limit.h"
#include "dmap.h"
#include "rprm.h"
#include "iq.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "tsg.h"
#include "rmsg.h"
#include "freq.h" 
#include "radarshell.h"
#include "default.h"

struct RadarParm *prm;
struct IQ *iq;
struct RawData *raw;
struct FitData *fit;
struct FitBlock *fblk;
struct RadarSite *site;
struct FreqTable *ftable;
struct RadarNetwork *network;
struct Radar *radar;

struct TSGbuf *tsgbuf;
struct TSGprm tsgprm;


struct RMsgBlock msg;

struct RShellTable rstable;


int maxatten=DEFAULT_MAX_ATTEN;
int protatten=DEFAULT_PROT_ATTEN;

unsigned char command[128];

int debug=0;

int rnum=1;
int cnum=1;

int tsgid=0;
int stid=0;
int yr=0;
int mo=0;
int dy=0;
int hr=0;
int mt=0;
int sc=0;
int us=0;
int txpow=DEFAULT_TXPOW;
int nave=0;
int atten=0;
int lagfr=DEFAULT_LAGFR;
int smsep=DEFAULT_SMSEP;
int ercod=0;
int agcstat=0;
int lopwrstat=0;
int nbaud=1;
float noise=0;
int rxrise=DEFAULT_RXRISE;
int intsc=DEFAULT_INTSC;
int intus=DEFAULT_INTUS;
int txpl=DEFAULT_TXPL;
int mpinc=DEFAULT_MPINC;
int mppul=DEFAULT_MPPUL;
int mplgs=DEFAULT_MPLGS;
int mplgexs=0;
int ifmode=0;
int nrang=DEFAULT_NRANG;
int frang=DEFAULT_FRANG;
int rsep=DEFAULT_RSEP;
int bmnum=0;
int xcf=0;
int tfreq=DEFAULT_TFREQ;
int scan=DEFAULT_SCAN;
int mxpwr=DEFAULT_MXPWR;
int lvmax=DEFAULT_LVMAX;
int cp=50;

int *pcode=NULL;

int noisefac=0;
int noiselim=0;

float noisestat[4];

char combf[128]={"A SuperDARN"};

float pwr0[MAX_RANGE];
float acfd[MAX_RANGE*LAG_SIZE*2];
float xcfd[MAX_RANGE*LAG_SIZE*2];

int backward=0;
int sbm=0;
int ebm=15;

int xcnt=0;

int day=10;
int night=20;

int dfrq=12000;
int nfrq=12000;
int dmpinc=1500;
int nmpinc=1500;
int dfrang=180;
int nfrang=180;

int stfrq=12300;
int frqrng=300;


/* raw I&Q sample buffer */

int rxchn;
int smpnum;
int skpnum;
struct timespec seqtval[MAXNAVE];
int seqatten[MAXNAVE];
float seqnoise[MAXNAVE];
int seqoff[MAXNAVE];
int seqsze[MAXNAVE];
struct {
  int num;
  unsigned int *start;
  unsigned int *length;
} seqbadtr[MAXNAVE];

int16 *samples;

unsigned int *badtr;

int shmemfd;
char sharedmemory[256]="IQBuffer";







