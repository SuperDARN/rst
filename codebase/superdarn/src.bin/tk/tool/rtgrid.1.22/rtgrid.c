/* rtgrid.c
   ========
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"

#include "dmap.h"
#include "radar.h" 
#include "rprm.h"
#include "fitdata.h"
#include "scandata.h"
#include "connex.h"
#include "fitcnx.h"
#include "fitscan.h"

#include "filter.h"
#include "checkops.h"
#include "gtable.h"
#include "oldgtablewrite.h"
#include "gtablewrite.h"
#include "bound.h"

#include "errstr.h"
#include "hlpstr.h"
#include "loginfo.h"

#include "aacgm.h"
#include "aacgmlib_v2.h"

extern int dotflag;

int resetflg;

struct RadarParm *prm;
struct FitData *fit;
struct RadarScan *src[3];
struct RadarScan *dst;
struct RadarScan *out;

struct GridTable *grid;

struct OptionData opt;


struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

int ebmno=0;
int ebm[32];

#define FNAME "grid.rt"
#define PATH "."
#define PIDFILE "gridrt.pid"
#define LOGNAME "gridrt.log"

void trap_signal(int signal) {
  resetflg=2; 
}

void day_output(char *path,struct GridTable *ptr,int xtd,
                char *code,int channel,char *sfx) {
  FILE *fp;
  char fname[256];

  int syr,smo,sdy,shr,smt;
  double sec;

  TimeEpochToYMDHMS(ptr->st_time,&syr,&smo,&sdy,&shr,&smt,&sec);

  if (channel==-1) 
    sprintf(fname,"%s/%.2d%.2d%.2d.%s.%s",path,
            syr,smo,sdy,code,sfx);
  else {
    char *chn="ab";
    sprintf(fname,"%s/%.2d%.2d%.2d.%s.%c.%s",
            path,syr,smo,sdy,code,
            chn[channel-1],sfx);
  }
  fp=fopen(fname,"a");
  GridTableFwrite(fp,ptr,NULL,xtd);
  fclose(fp);
}


void exclude_range(struct RadarScan *ptr,int minrng,int maxrng) {
  int bm,rng;
  for (bm=0;bm<ptr->num;bm++) {
    if (ptr->bm[bm].bm==-1) continue;
    if (minrng !=-1) for (rng=0;rng<minrng;rng++) ptr->bm[bm].sct[rng]=0;
    if (maxrng !=-1) for (rng=maxrng;rng<ptr->bm[bm].nrang;rng++)
       ptr->bm[bm].sct[rng]=0;

  }
}

void parse_ebeam(char *str) {
  int i,j=0;
  for (i=0;str[i] !=0;i++) {
    if (str[i]==',') {
      str[i]=0;
      ebm[ebmno]=atoi(str+j);
      ebmno++;
      j=i+1;
    } 
  }
  ebm[ebmno]=atoi(str+j);
  ebmno++;
}


void process_beam(struct RadarScan *ptr,struct RadarParm *prm,
                  struct FitData *fit) {
  int n;
  n=ptr->num;
  FitToRadarScan(ptr,prm,fit);
  ptr->ed_time=ptr->bm[n].time;
  if (ptr->st_time==-1) ptr->st_time=ptr->ed_time;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: rtgrid --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  char *logstr=NULL;
  char *pathstr=NULL;
  char *fnamestr=NULL;
  char *pidstr=NULL;
  char *chnstr=NULL;
  char *bmstr=NULL;

  int sock;
  int i;
  int remote_port=0;
  char logbuf[256];
  char wrtlog[256];
  pid_t pid;
  FILE *fp;
  char host[256];
  int flag;
  struct flock flock;
  int fid=0;

  sigset_t set;
  struct sigaction act;

  int mask=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  time_t stme;
  time_t utc;
  struct timeval tv;
  int timeout;
  int avlen=120;
  int tlen=120;
  int reset=60;

  int inx=0,num=0;

  int channel=-1;

  double min[4]={35,3,10,0};
  double max[4]={2500,60,1000,200};
  int fmax=500;

  double alt=300.0;

  int mode=0;
  unsigned char bxcar=0;
  unsigned char limit=0;
  unsigned char xtd=0;
  unsigned char iflg=0;
  unsigned char bflg=0;
  int isort=0;

  unsigned char gsflg=0,ionflg=0,bthflg=0;

  int dataflg=0;

  int minrng=-1;
  int maxrng=-1;

  char fname[256]={FNAME};
  char path[256]={PATH};
  char pidfile[256]={PIDFILE};
  char logname[256]={LOGNAME};
  char *port_fname=NULL;
  int port_flag=0;

  char *envstr;
  char *stcode=NULL;

  int nbox;
  int s;
  int chk;

  int chisham=0;
  int old_aacgm=0;

  int yr,mo,dy,hr,mt;
  double sc;

  prm=RadarParmMake();
  fit=FitMake();

  for (i=0;i<3;i++) src[i]=RadarScanMake();
  dst=RadarScanMake();
  grid=GridTableMake();

  pid=getpid();

  flock.l_type=F_WRLCK;
  flock.l_whence=SEEK_SET;
  flock.l_start=0;
  flock.l_len=0;

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
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old); 

  OptionAdd(&opt,"xtd",'x',&xtd);
  OptionAdd(&opt,"i",'i',&avlen);
  OptionAdd(&opt,"tl",'i',&tlen);

  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"ebm",'t',&bmstr);
  OptionAdd(&opt,"minrng",'i',&minrng);
  OptionAdd(&opt,"maxrng",'i',&maxrng);

  OptionAdd(&opt,"fwgt",'i',&mode);

  OptionAdd(&opt,"pmax",'d',&max[1]);
  OptionAdd(&opt,"vmax",'d',&max[0]);
  OptionAdd(&opt,"wmax",'d',&max[2]);
  OptionAdd(&opt,"vemax",'d',&max[3]);

  OptionAdd(&opt,"pmin",'d',&min[1]);
  OptionAdd(&opt,"vmin",'d',&min[0]);
  OptionAdd(&opt,"wmin",'d',&min[2]);
  OptionAdd(&opt,"vemin",'d',&min[3]);

  OptionAdd(&opt,"fmax",'i',&fmax);

  OptionAdd(&opt,"alt",'d',&alt);

  OptionAdd(&opt,"nav",'x',&bxcar);
  OptionAdd(&opt,"nlm",'x',&limit);
  OptionAdd(&opt,"nb",'x',&bflg);
  OptionAdd(&opt,"isort",'x',&isort);

  OptionAdd(&opt,"ion",'x',&ionflg);
  OptionAdd(&opt,"gs",'x',&gsflg);
  OptionAdd(&opt,"both",'x',&bthflg);

  OptionAdd(&opt,"inertial",'x',&iflg);

  OptionAdd(&opt,"rpf",'x',&port_flag);

  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"p",'t',&pathstr);
  OptionAdd(&opt,"f",'t',&fnamestr);
  OptionAdd(&opt,"if",'t',&pidstr);

  OptionAdd(&opt,"chisham",'x',&chisham);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);

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

  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }

  if (bmstr !=NULL)  parse_ebeam(bmstr);

  if (mode>0) mode--;

  bxcar=!bxcar;
  bflg=!bflg;
  limit=!limit;

  grid->gsct=1;
  if (gsflg) grid->gsct=0;
  if (ionflg) grid->gsct=1;
  if (bthflg) grid->gsct=2;

  if (logstr !=NULL) strcpy(logname,logstr);
  if (fnamestr !=NULL) strcpy(fname,fnamestr);
  if (pathstr !=NULL) strcpy(path,pathstr);
  if (pidstr !=NULL) strcpy(pidfile,pidstr);

  strcpy(host,argv[argc-2]);
  if (port_flag==0) remote_port=atoi(argv[argc-1]);
  else port_fname=argv[argc-1];

  for (i=0;i<4;i++) {
    grid->min[i]=min[i];
    grid->max[i]=max[i];
  }

  if (bxcar) nbox=3;
  else nbox=1;

  grid->st_time=-1;

  if (channel !=-1) grid->chn=channel;
  else grid->chn=0;

  out=dst;

  if (port_flag==0) sprintf(logbuf,"Host:%s %d",host,remote_port);
  else sprintf(logbuf,"Host:%s Port File:%s",host,port_fname);
  loginfo(logname,logbuf);

  if (old) sprintf(logbuf,"Output file name:%s<stid>.grd",fname);
  else sprintf(logbuf,"Output file name:%s<stid>.grdmap",fname);
  loginfo(logname,logbuf);
  sprintf(logbuf,"Daily file path:%s",path);
  loginfo(logname,logbuf);
  sprintf(logbuf,"Sample rate:%d secs.",avlen);
  loginfo(logname,logbuf);
  sprintf(logbuf,"pid file:%s",pidfile);
  loginfo(logname,logbuf);
  sprintf(logbuf,"pid:%d",(int) pid);
  loginfo(logname,logbuf);

  fp=fopen(pidfile,"w");
  fprintf(fp,"%d\n",pid);
  fclose(fp);
  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_signal;
  sigaction(SIGUSR1,&act,NULL);

  do {
    resetflg=0;

    if (port_flag==1) {
      fp=fopen(port_fname,"r");
      if (fp !=NULL) {
        if(fscanf(fp,"%d",&remote_port) !=1) {
          fclose(fp);
          exit(-1);
        }
        fclose(fp);
      } else remote_port=1024;
    }

    sprintf(logbuf,"Connecting to host:%s %d",host,remote_port);

    loginfo(logname,logbuf);

    sock=ConnexOpen(host,remote_port,NULL);
    if (sock<0) {
      loginfo(logname,"Could not connect to host - retrying.");
      sleep(10);
      continue;
    }

    stme=time(NULL);
    stme=stme-(stme % tlen);
    resetflg=0;

    src[inx]->st_time=-1;
    src[inx]->ed_time=-1;

    RadarScanReset(src[inx]);

    /* Calculate year, month, day, hour, minute, and second of
     * grid start time (needed to load AACGM_v2 coefficients) */
    TimeEpochToYMDHMS(stme,&yr,&mo,&dy,&hr,&mt,&sc);

    /* Load AACGM coefficients */
    if (old_aacgm) AACGMInit(yr);
    else AACGM_v2_SetDateTime(yr,mo,dy,0,0,0);

    do {
      utc=time(NULL);
      timeout=tlen-(utc-stme);

      tv.tv_sec=timeout;
      tv.tv_usec=0;

      s=FitCnxRead(1,&sock,prm,fit,&flag,&tv);
      if (s==0) sleep(1);

      if ((s==-1) || (flag==-1) || (resetflg !=0))  break;
      if ((s==0) && (timeout>reset)) {
        resetflg=1;
        break;
      }
      if ((s==1) && (flag==1)) {
        if (prm->scan<0) continue;

        if (channel !=-1) {
          if ((channel==1) && (prm->channel==2)) continue;
          if ((channel==2) && (prm->channel!=2)) continue;
          grid->chn=prm->channel;
        }

      }

      utc=time(NULL);

      if ((s==1) && (flag==1)) {
        fprintf(stderr,".");
        fflush(stderr);
        dotflag=1;
        dataflg=1;

        process_beam(src[inx],prm,fit);

      }

      if ((utc-stme)>=tlen) {

        src[inx]->st_time=stme;
        src[inx]->ed_time=stme+tlen;

        TimeEpochToYMDHMS(src[inx]->st_time,
          &yr,&mo,&dy,&hr,&mt,&sc);
        if (dataflg==1) sprintf(logbuf,
          "%02d:%02d:%02d : Processing scan %d (data received)",
          hr,mt,(int) sc,num);
        else sprintf(logbuf,
          "%02d:%02d:%02d : Processing scan %d (no data received)",
          hr,mt,(int) sc,num);
        dataflg=0;
        loginfo(logname,logbuf);
        stme=utc-(utc % tlen);

        RadarScanResetBeam(src[inx],ebmno,ebm);
        exclude_range(src[inx],minrng,maxrng);
        FilterBoundType(src[inx],grid->gsct);

        if (bflg) FilterBound(15,src[inx],min,max);

        if ((num>=nbox) && (limit==1) && (mode !=-1))
          chk=FilterCheckOps(nbox,src,fmax);
        else chk=0;

        if ((chk==0) && (num>=nbox)) {

          if (mode !=-1) FilterRadarScan(mode,nbox,inx,src,dst,15,isort);
          else out=src[inx];

          TimeEpochToYMDHMS(out->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

          if (site==NULL) {
            radar=RadarGetRadar(network,out->stid);
            if (radar==NULL) {
              fprintf(stderr,"Failed to get radar information.\n");
              exit(-1);
            }
            site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
            stcode=RadarGetCode(network,out->stid,0);

            strcat(fname,stcode);
            if (channel==1) strcat(fname,".a");
            if (channel==2) strcat(fname,".b");
            if (old) strcat(fname,".grd");
            else strcat(fname,".grdmap");
          }

          s=GridTableTest(grid,out);

          if (s==1) {
             fid=open(fname,O_WRONLY | O_TRUNC | O_CREAT,mask);
             if (fid==0) continue;
             fcntl(fid,F_SETLKW,&flock);

             if (old) OldGridTableWrite(fid,grid,wrtlog,xtd);
             else GridTableWrite(fid,grid,wrtlog,xtd);
             sprintf(logbuf,"Storing:%s\n",wrtlog);
             loginfo(logname,logbuf);
             close(fid);
             if (old) day_output(path,grid,xtd,stcode,channel,"grd");
             else day_output(path,grid,xtd,stcode,channel,"grdmap");
          }
          GridTableMap(grid,out,site,avlen,iflg,alt,chisham,old_aacgm);
        }
        if (bxcar) inx++;
        if (inx>2) inx=0;

        src[inx]->st_time=-1;
        src[inx]->ed_time=-1;
        RadarScanReset(src[inx]);
        num++;
      }

    } while (1);

    if (resetflg==0) loginfo(logname,"Connection failed.");
    if (resetflg==1) loginfo(logname,"Connection timed out.");
    if (resetflg==2) loginfo(logname,"Connection reset by signal.");
    ConnexClose(sock);
    sleep(5);
  } while(1);

  return 0;
}

