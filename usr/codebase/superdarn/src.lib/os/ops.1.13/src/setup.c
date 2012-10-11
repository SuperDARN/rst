/* setup.c
   ======= 
   Author: R.J.Barnes
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
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "radar.h"
#include "rprm.h"
#include "iq.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "fitacf.h"
#include "freq.h"
#include "tcpipmsg.h"
#include "radarshell.h"
#include "errlog.h"
#include "global.h"
#include "setup.h"

int OpsSetupCommand(int argc,char *argv[]) {

  int c,n;

  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat( (char *) command," ");
    strcat( (char *) command,argv[c]);
  }
  return 0;
}

int OpsStart(char *ststr) {
  FILE *fp;
  char *envstr;
 
  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
 
  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  stid=RadarGetID(network,ststr);

  radar=RadarGetRadar(network,stid);
  if (radar==NULL) {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);

  if (site==NULL) {
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }

  envstr=getenv("SD_FREQ_TABLE");
  if (envstr !=NULL) fp=fopen(envstr,"r");
  else fp=NULL;
  if (fp !=NULL) {
    ftable=FreqLoadTable(fp);
    fclose(fp);
  }

  prm=RadarParmMake();
  raw=RawMake();
  iq=IQMake();

  return 0;
}

int OpsFitACFStart() {
  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
  if (site==NULL) return -1;
  fit=FitMake();
  fblk=FitACFMake(site,yr); 
  return 0;
}

void OpsLogStart(int sock,char *name,int argc,char *argv[]) {
  char buffer[4096];
  int i;
  sprintf(buffer,START_STRING);
  for (i=0;i<argc;i++) {
    if (i !=0) strcat(buffer," ");
    strcat(buffer,argv[i]);
  }
  ErrLog(sock,name,buffer);
}

void OpsSetupTask(int tnum,struct TCPIPMsgHost *task,int sock,char *name) {
  char buffer[4096];
 
  int n;
  fprintf(stderr,"setting up tasks.\n"); 
  for (n=0;n<tnum;n++) {
    task[n].sock=TCPIPMsgOpen(task[n].host,task[n].port);
    if (task[n].sock==-1) 
      sprintf(buffer,"Error attaching to %s:%d",task[n].host,task[n].port);
    else 
     sprintf(buffer,"Attached to %s:%d",task[n].host,task[n].port);
    ErrLog(sock,name,buffer);
  }
}


void OpsSetupShell() {
  RadarShellAdd(&rstable,"intsc",var_LONG,&intsc);
  RadarShellAdd(&rstable,"intus",var_LONG,&intus);
  RadarShellAdd(&rstable,"txpl",var_LONG,&txpl);
  RadarShellAdd(&rstable,"mpinc",var_LONG,&mpinc);
  RadarShellAdd(&rstable,"mppul",var_LONG,&mppul);
  RadarShellAdd(&rstable,"mplgs",var_LONG,&mplgs);
  RadarShellAdd(&rstable,"nrang",var_LONG,&nrang);
  RadarShellAdd(&rstable,"frang",var_LONG,&frang);
  RadarShellAdd(&rstable,"rsep",var_LONG,&rsep);
  RadarShellAdd(&rstable,"bmnum",var_LONG,&bmnum);
  RadarShellAdd(&rstable,"xcf",var_LONG,&xcf);
  RadarShellAdd(&rstable,"tfreq",var_LONG,&tfreq);
  RadarShellAdd(&rstable,"scan",var_LONG,&scan);
  RadarShellAdd(&rstable,"mxpwr",var_LONG,&mxpwr);
  RadarShellAdd(&rstable,"lvmax",var_LONG,&lvmax);
  RadarShellAdd(&rstable,"rxrise",var_LONG,&rxrise);
  RadarShellAdd(&rstable,"cp",var_LONG,&cp);
  RadarShellAdd(&rstable,"combf",var_STRING,combf);
};
