/* site.c
   ====== 
   Author R.J.Barnes & J.Spaleta
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


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "rtypes.h"
#include "limit.h"
#include "tsg.h"
#include "maketsg.h"
#include "acf.h"
#include "tcpipmsg.h"
#include "rosmsg.h"
#include "shmem.h"
#include "global.h"
#include "site.h"

int sock;
char server[256];
int port;
int num_transmitters;

struct timeval tock;
struct ControlPRM rprm;
struct RosData rdata;
struct DataPRM dprm;
struct TRTimes badtrdat;
struct TXStatus txstatus;
struct SiteLibrary sitelib;
int *exit_flag=NULL;
int cancel_count=0;




int SiteStart(char *host) {
  rdata.main=NULL;
  rdata.back=NULL;
  badtrdat.start_usec=NULL;
  badtrdat.duration_usec=NULL; 
  return (sitelib.start)(host);
}

int SiteSetupRadar() {
  return (sitelib.setupradar)();
}
 
int SiteStartScan() {
  return (sitelib.startscan)();
}

int SiteStartIntt(int sec,int usec) {
  return (sitelib.startintt)(sec,usec);
}

int SiteFCLR(int stfreq,int edfreq) {
  return (sitelib.fclr)(stfreq,edfreq);
}

int SiteTimeSeq(int *ptab) {
  return (sitelib.tmseq)(ptab);
}

int SiteIntegrate(int (*lags)[2]) {
  return (sitelib.integrate)(lags);
}

int SiteEndScan(int bsc,int bus) {
  return (sitelib.endscan)(bsc,bus);
}

void SiteExit(int signo) {
  (sitelib.exit)(signo);
}




