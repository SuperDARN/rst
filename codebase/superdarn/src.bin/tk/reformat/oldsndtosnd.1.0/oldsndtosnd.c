/* oldsndtosnd.c
   =============
   Author: E.G.Thomas
*/

/*
 Copyright (C) <year>  <name of author>
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rtime.h"
#include "rmath.h"
#include "option.h"
#include "dmap.h"
#include "radar.h"

#include "snddata.h"
#include "sndread.h"
#include "sndwrite.h"
#include "sndseek.h"

#include "errstr.h"
#include "hlpstr.h"


#define SND_NRANG 75
#define MAX_RANGE 300

struct SndData *snd;
struct OptionData opt;
struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

struct header_struct {
  int   stime;
  short site_id;
  short beam_no;
  short freq;
  short noise;
  short frange;
  short rsep;
  short gsct[SND_NRANG];
  short qflg[SND_NRANG];
  char  program_name[40];
} header;

struct data_struct {
  short pwr;
  short vel;
  short width;
  short AOA;
} data;

struct header_struct_adak {
  long  stime;
  short site_id;
  short beam_no;
  short freq;
  short noise;
  short frange;
  short rsep;
  short gsct[SND_NRANG];
  short qflg[SND_NRANG];
  char  program_name[40];
} header_adak;

#pragma pack(1)
struct header_struct_new {
  int   stime;
  short site_id;
  short beam_no;
  short freq;
  short noise;
  short frange;
  short rsep;
  short nrang;
  short gsct[MAX_RANGE];
  short qflg[MAX_RANGE];
  char  program_name[40];
} header_new;

struct header_struct_old {
  double stime;
  char   site_id;
  char   beam_no;
  short  freq;
  short  noise;
  short  frange;
  short  rsep;
  char   gsct[10];
  char   qflg[10];
  char   program_name[40];
  short  unused1;
  short  unused2;
  short  unused3;
} header_old;
#pragma pack()

struct data_struct_old {
  short vel;
  unsigned short width;
  unsigned char pwr;
  unsigned char AOA;
  short unused1;
} data_old;

double calc_psi_obs(struct RadarSite *site, int bmnum, int tfreq, double elevation);

int rst_opterr (char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: oldsndtosnd --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int arg;

  int i,status=0;
  FILE *fp=NULL;

  char *envstr=NULL;

  int yr,mo,dy,hr,mt;
  double sc;
 
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int old=0;
  int new=0;
  int adak=0;

  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];

  float offset;

  int byte=0;
  double min_vel=-3000;
  double max_vel=3000;
  double max_power=50;
  double max_width=1000;
  double max_AOA=90;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"adak",'x',&adak);
  OptionAdd(&opt,"vb",'x',&vb);

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

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
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
    fprintf(stderr,"Failed to load radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);


  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }

  snd=SndMake();

  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (old) {
    /* Read the sounding data files that used internal compression */
    while(fread(&header_old,sizeof(header_old),1,fp) == 1) {

      if ((header_old.site_id <= 0) || (header_old.site_id > network->radar[network->rnum-1].id)) {
        fprintf(stderr,"Invalid site_id (%d): ",header_old.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      snd->origin.code=1;
      ctime = time((time_t) 0);
      SndSetOriginCommand(snd,command);
      strcpy(tmstr,asctime(gmtime(&ctime)));
      tmstr[24]=0;
      SndSetOriginTime(snd,tmstr);

      TimeEpochToYMDHMS(header_old.stime,&yr,&mo,&dy,&hr,&mt,&sc);

      radar = RadarGetRadar(network,header_old.site_id);
      if (radar == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header_old.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      site = RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);
      if (site == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header_old.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }
      offset = site->maxbeam/2.0-0.5;

      snd->stid = header_old.site_id;
      snd->time.yr = yr;
      snd->time.mo = mo;
      snd->time.dy = dy;
      snd->time.hr = hr;
      snd->time.mt = mt;
      snd->time.sc = (int) sc;
      snd->lagfr = header_old.frange*20/3;
      snd->smsep = header_old.rsep*20/3;
      snd->noise.mean = header_old.noise;
      snd->bmnum = header_old.beam_no;
      snd->bmazm = site->boresite + site->bmsep*(header_old.beam_no-offset) + site->bmoff;
      snd->intt.sc = 2;
      snd->intt.us = 0;
      snd->nrang = SND_NRANG;
      snd->frang = header_old.frange;
      snd->rsep = header_old.rsep;
      snd->xcf = 1;
      snd->tfreq = header_old.freq;
      snd->snd_revision.major = SND_MAJOR_REVISION;
      snd->snd_revision.minor = SND_MINOR_REVISION;

      SndSetCombf(snd,header_old.program_name);

      SndSetRng(snd,SND_NRANG);

      for (i=0;i<SND_NRANG;i++) {
        byte=i/8;
        if ( (header_old.qflg[byte]>>(i%8)) & 0x01 ) {
          snd->rng[i].qflg = 1;
          if ( (header_old.gsct[byte]>>(i%8)) & 0x01 ) {
            snd->rng[i].gsct = 1;
          }
          status=fread(&data_old,sizeof(data_old),1,fp);
          if (status != 1) break;
          if (data_old.vel < 0) {
            snd->rng[i].v = -(data_old.vel+1)*min_vel/32767.;
          } else {
            snd->rng[i].v = data_old.vel*max_vel/32767.;
          }
          snd->rng[i].p_l = max_power*data_old.pwr/255.;
          snd->rng[i].w_l = max_width*data_old.width/65535.;
          if ((data_old.AOA == 0) || (data_old.AOA == 255)) {
            snd->rng[i].x_qflg = 0;
            snd->rng[i].phi0 = 0;
          } else {
            snd->rng[i].x_qflg = 1;
            snd->rng[i].phi0 = calc_psi_obs(site,snd->bmnum,snd->tfreq,max_AOA*data_old.AOA/255.);
          }
        }
      }

      status=SndFwrite(stdout,snd);

      if (status==-1) break;

      if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d  (b:%.2d f:%5d)\n",
                      snd->time.yr,snd->time.mo,snd->time.dy,
                      snd->time.hr,snd->time.mt,snd->time.sc,
                      snd->bmnum,snd->tfreq);
    }

  } else if (new) {
    /* Read the sounding data files that do not use internal compression
     * and have a variable number of range gates */
    while(fread(&header_new,sizeof(header_new),1,fp) == 1) {

      if ((header_new.site_id <= 0) || (header_new.site_id > network->radar[network->rnum-1].id)) {
        fprintf(stderr,"Invalid site_id (%d): ",header_new.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      snd->origin.code=1;
      ctime = time((time_t) 0);
      SndSetOriginCommand(snd,command);
      strcpy(tmstr,asctime(gmtime(&ctime)));
      tmstr[24]=0;
      SndSetOriginTime(snd,tmstr);

      TimeEpochToYMDHMS(header_new.stime,&yr,&mo,&dy,&hr,&mt,&sc);

      radar = RadarGetRadar(network,header_new.site_id);
      if (radar == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header_new.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      site = RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);
      if (site == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header_new.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }
      offset = site->maxbeam/2.0-0.5;

      snd->stid = header_new.site_id;
      snd->time.yr = yr;
      snd->time.mo = mo;
      snd->time.dy = dy;
      snd->time.hr = hr;
      snd->time.mt = mt;
      snd->time.sc = (int) sc;
      snd->lagfr = header_new.frange*20/3;
      snd->smsep = header_new.rsep*20/3;
      snd->noise.mean = header_new.noise;
      snd->bmnum = header_new.beam_no;
      snd->bmazm = site->boresite + site->bmsep*(header_new.beam_no-offset) + site->bmoff;
      snd->intt.sc = 2;
      snd->intt.us = 0;
      snd->nrang = header_new.nrang;
      snd->frang = header_new.frange;
      snd->rsep = header_new.rsep;
      snd->xcf = 1;
      snd->tfreq = header_new.freq;
      snd->snd_revision.major = SND_MAJOR_REVISION;
      snd->snd_revision.minor = SND_MINOR_REVISION;

      SndSetCombf(snd,header_new.program_name);

      SndSetRng(snd,header_new.nrang);

      for (i=0;i<header_new.nrang;i++) {
        snd->rng[i].qflg = header_new.qflg[i];
        snd->rng[i].gsct = header_new.gsct[i];
        if (header_new.qflg[i] == 1) {
          status=fread(&data,sizeof(data),1,fp);
          if (status != 1) break;
          snd->rng[i].v = data.vel;
          snd->rng[i].p_l = data.pwr;
          snd->rng[i].w_l = data.width;
          if (data.AOA == 0) {
            snd->rng[i].x_qflg = 0;
            snd->rng[i].phi0 = 0;
          } else {
            snd->rng[i].x_qflg = 1;
            snd->rng[i].phi0 = calc_psi_obs(site,snd->bmnum,snd->tfreq,data.AOA);
          }
        }
      }

      status=SndFwrite(stdout,snd);

      if (status==-1) break;

      if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d  (b:%.2d f:%5d)\n",
                      snd->time.yr,snd->time.mo,snd->time.dy,
                      snd->time.hr,snd->time.mt,snd->time.sc,
                      snd->bmnum,snd->tfreq);
    }

  } else if (adak) {
    /* Read the sounding data files that do not use internal compression
     * and have a different size stime in the header */
    while(fread(&header_adak,sizeof(header_adak),1,fp) == 1) {

      if ((header_adak.site_id <= 0) || (header_adak.site_id > network->radar[network->rnum-1].id)) {
        fprintf(stderr,"Invalid site_id (%d): ",header_adak.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      snd->origin.code=1;
      ctime = time((time_t) 0);
      SndSetOriginCommand(snd,command);
      strcpy(tmstr,asctime(gmtime(&ctime)));
      tmstr[24]=0;
      SndSetOriginTime(snd,tmstr);

      TimeEpochToYMDHMS(header_adak.stime,&yr,&mo,&dy,&hr,&mt,&sc);

      radar = RadarGetRadar(network,header_adak.site_id);
      if (radar == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header_adak.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      site = RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);
      if (site == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header_adak.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }
      offset = site->maxbeam/2.0-0.5;

      snd->stid = header_adak.site_id;
      snd->time.yr = yr;
      snd->time.mo = mo;
      snd->time.dy = dy;
      snd->time.hr = hr;
      snd->time.mt = mt;
      snd->time.sc = (int) sc;
      snd->lagfr = header_adak.frange*20/3;
      snd->smsep = header_adak.rsep*20/3;
      snd->noise.mean = header_adak.noise;
      snd->bmnum = header_adak.beam_no;
      snd->bmazm = site->boresite + site->bmsep*(header_adak.beam_no-offset) + site->bmoff;
      snd->intt.sc = 2;
      snd->intt.us = 0;
      snd->nrang = SND_NRANG;
      snd->frang = header_adak.frange;
      snd->rsep = header_adak.rsep;
      snd->xcf = 1;
      snd->tfreq = header_adak.freq;
      snd->snd_revision.major = SND_MAJOR_REVISION;
      snd->snd_revision.minor = SND_MINOR_REVISION;

      SndSetCombf(snd,header_adak.program_name);

      SndSetRng(snd,SND_NRANG);

      for (i=0;i<SND_NRANG;i++) {
        snd->rng[i].qflg = header_adak.qflg[i];
        snd->rng[i].gsct = header_adak.gsct[i];
        if (header_adak.qflg[i] == 1) {
          status=fread(&data,sizeof(data),1,fp);
          if (status != 1) break;
          snd->rng[i].v = data.vel;
          snd->rng[i].p_l = data.pwr;
          snd->rng[i].w_l = data.width;
          if (data.AOA == 0) {
            snd->rng[i].x_qflg = 0;
            snd->rng[i].phi0 = 0;
          } else {
            snd->rng[i].x_qflg = 1;
            snd->rng[i].phi0 = calc_psi_obs(site,snd->bmnum,snd->tfreq,data.AOA);
          }
        }
      }

      status=SndFwrite(stdout,snd);

      if (status==-1) break;

      if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d  (b:%.2d f:%5d)\n",
                      snd->time.yr,snd->time.mo,snd->time.dy,
                      snd->time.hr,snd->time.mt,snd->time.sc,
                      snd->bmnum,snd->tfreq);
    }
  } else {
    /* Read the sounding data files that do not use internal compression */
    while(fread(&header,sizeof(header),1,fp) == 1) {

      if ((header.site_id <= 0) || (header.site_id > network->radar[network->rnum-1].id)) {
        fprintf(stderr,"Invalid site_id (%d): ",header.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      snd->origin.code=1;
      ctime = time((time_t) 0);
      SndSetOriginCommand(snd,command);
      strcpy(tmstr,asctime(gmtime(&ctime)));
      tmstr[24]=0;
      SndSetOriginTime(snd,tmstr);

      TimeEpochToYMDHMS(header.stime,&yr,&mo,&dy,&hr,&mt,&sc);

      radar = RadarGetRadar(network,header.site_id);
      if (radar == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }

      site = RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);
      if (site == NULL) {
        fprintf(stderr,"Invalid site_id (%d): ",header.site_id);
        fprintf(stderr,"file likely in different snd format\n");
        break;
      }
      offset = site->maxbeam/2.0-0.5;

      snd->stid = header.site_id;
      snd->time.yr = yr;
      snd->time.mo = mo;
      snd->time.dy = dy;
      snd->time.hr = hr;
      snd->time.mt = mt;
      snd->time.sc = (int) sc;
      snd->lagfr = header.frange*20/3;
      snd->smsep = header.rsep*20/3;
      snd->noise.mean = header.noise;
      snd->bmnum = header.beam_no;
      snd->bmazm = site->boresite + site->bmsep*(header.beam_no-offset) + site->bmoff;
      snd->intt.sc = 2;
      snd->intt.us = 0;
      snd->nrang = SND_NRANG;
      snd->frang = header.frange;
      snd->rsep = header.rsep;
      snd->xcf = 1;
      snd->tfreq = header.freq;
      snd->snd_revision.major = SND_MAJOR_REVISION;
      snd->snd_revision.minor = SND_MINOR_REVISION;

      SndSetCombf(snd,header.program_name);

      SndSetRng(snd,SND_NRANG);

      for (i=0;i<SND_NRANG;i++) {
        snd->rng[i].qflg = header.qflg[i];
        snd->rng[i].gsct = header.gsct[i];
        if (header.qflg[i] == 1) {
          status=fread(&data,sizeof(data),1,fp);
          if (status != 1) break;
          snd->rng[i].v = data.vel;
          snd->rng[i].p_l = data.pwr;
          snd->rng[i].w_l = data.width;
          if (data.AOA == 0) {
            snd->rng[i].x_qflg = 0;
            snd->rng[i].phi0 = 0;
          } else {
            snd->rng[i].x_qflg = 1;
            snd->rng[i].phi0 = calc_psi_obs(site,snd->bmnum,snd->tfreq,data.AOA);
          }
        }
      }

      status=SndFwrite(stdout,snd);

      if (status==-1) break;

      if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d  (b:%.2d f:%5d)\n",
                      snd->time.yr,snd->time.mo,snd->time.dy,
                      snd->time.hr,snd->time.mt,snd->time.sc,
                      snd->bmnum,snd->tfreq);
    }
  }

  /* Close the sounding data file */
  if (fp !=stdin) fclose(fp);

  return 0;
}


double calc_psi_obs(struct RadarSite *site, int bmnum, int tfreq, double elevation) {

  double Y;         /* interferometer Y-offset [m]                   */
  double boff;      /* offset in beam widths to edge of FOV          */
  double phi0;      /* beam direction off boresight [rad]            */
  double cp0;       /* cosine of phi0                                */
  double selv;      /* sine of elevation angle                       */
  double psi_obs;   /* observed phase difference [rad]               */

  Y = site->interfer[1];

  boff = site->maxbeam/2. - 0.5;
  phi0 = (site->bmoff + site->bmsep*(bmnum - boff))*PI/180.;
  cp0 = cos(phi0);

  selv = sin(elevation*PI/180.);

  /* only consider interferometer Y-offset to replicate original function */
  psi_obs = 2.*PI * tfreq*1e3 * ((1./C)*(Y*sqrt(cp0*cp0 - selv*selv)) - site->tdiff[0]*1e-6);

  while (psi_obs > PI) psi_obs -= 2.*PI;
  while (psi_obs < -PI) psi_obs += 2.*PI;

  return psi_obs;
}
