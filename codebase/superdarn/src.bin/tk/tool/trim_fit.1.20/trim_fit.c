/* trim_fit.c
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rtime.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "fitwrite.h"
#include "fitindex.h"
#include "fitseek.h"


#include "oldfitread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct RadarParm *pbuf[2];
struct FitData *fbuf[2];

int fnum=0;

struct RadarParm *prm;
struct FitData *fit;
struct FitIndex *inx=NULL;

int cpid=-1;
int channel=-1;

struct OptionData opt;

double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}

double strtime(char *text) {
  int hr,mn;
  double sc;
  int i,j;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  for (j=i+1;(text[j] !=':') && (text[j] !=0);j++);
  if (text[j]==0) { 
    mn=atoi(text+i+1);
    return (double) hr*3600L+mn*60L;
  }
  text[j]=0;
  mn=atoi(text+i+1);
  sc=atof(text+j+1);
  return (double) hr*3600L+mn*60L+sc;
}   

int main (int argc,char *argv[]) {

  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  int old=0;
  int new=0;

  int arg;

  int status=0;
  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  FILE *inxfp=NULL;
  
  int irec=1;
  int drec=2;
 
  int dnum=0;
  int yr,mo,dy,hr,mt;
  double sc;
 
  double atime;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double stime=-1;
  double etime=-1;
  double extime=0;
 
  double sdate=-1;
  double edate=-1;

  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char inxflg=0;
  unsigned char set_channel=0;
  
  char *chnstr=NULL;
  char *cpstr=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"i",'x',&inxflg);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  OptionAdd(&opt,"cp",'t',&cpstr);
  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"sc",'x',&set_channel);


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if ((old) && (argc-arg<2+inxflg)) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);

  if (cpstr !=NULL) cpid=atoi(cpstr);
   
  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }
    
  if (old) {
    char vstr[256];

    pbuf[0]=RadarParmMake();
    pbuf[1]=RadarParmMake();
  
    fbuf[0]=FitMake();
    fbuf[1]=FitMake();

    if ((argc-1-inxflg-arg) >1)  fitfp=OldFitOpen(argv[arg],argv[arg+1]);
    else fitfp=OldFitOpen(argv[arg],NULL);

    if (fitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(-1);
    }
    fp=fopen(argv[argc-1-inxflg],"w");
    if (fp==NULL) {
      fprintf(stderr,"could not create fit file.\n");
      exit(-1);
    }
 
    if (inxflg !=0) {
      inxfp=fopen(argv[argc-1],"w");
      if (inxfp==NULL) fprintf(stderr,"could not create index file.\n");
    }

    if (OldFitRead(fitfp,pbuf[fnum],fbuf[fnum])==-1) {
      fprintf(stderr,"Error reading file\n");
      exit(-1);
    }
    sprintf(vstr,"%d.%d",fbuf[fnum]->revision.major,
                         fbuf[fnum]->revision.minor); 
    OldFitHeaderFwrite(fp,"trim_fit","fitacf",vstr);

    atime=TimeYMDHMSToEpoch(pbuf[fnum]->time.yr,
		    pbuf[fnum]->time.mo,
                    pbuf[fnum]->time.dy,
                    pbuf[fnum]->time.hr,
		    pbuf[fnum]->time.mt,
                    pbuf[fnum]->time.sc+pbuf[fnum]->time.us/1.0e6);


    /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
      int yr,mo,dy,hr,mt;
      double sc;  
      
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;


      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      status=OldFitSeek(fitfp,yr,mo,dy,hr,mt,sc,NULL);
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
      if (OldFitRead(fitfp,pbuf[fnum],fbuf[fnum])==-1) {
        fprintf(stderr,"Error reading file\n");
        exit(-1);
      }
    } else stime=atime;
   
    if (etime !=-1) {
       if (edate==-1) etime+=atime - ( (int) atime % (24*3600));
       else etime+=edate;
    }
    if (extime !=0) etime=stime+extime;

  
    do {
      prm=pbuf[fnum];
      fit=fbuf[fnum];
      fnum=(fnum+1) % 2;

      if ((cpid !=-1) && (prm->cp !=cpid)) continue;
      if ((set_channel==0) &&
          (channel !=-1) && (prm->channel !=channel)) continue;
 
      if (set_channel==1) prm->channel=channel;

      if ((inxfp !=NULL) && (irec==1)) OldFitInxHeaderFwrite(inxfp,prm);
      dnum=OldFitFwrite(fp,prm,fit,NULL);
      if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,prm);
      atime=TimeYMDHMSToEpoch(prm->time.yr,
	  	      prm->time.mo,
                      prm->time.dy,
                      prm->time.hr,
		      prm->time.mt,
                      prm->time.sc+prm->time.us/1.0e6);

      TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d %.2d %.2d %.4d\n",
                            yr,mo,dy,hr,mt,
                            (int) sc,prm->channel,
                            prm->bmnum,prm->cp);

      drec+=dnum;
      irec++;
      atime=TimeYMDHMSToEpoch(pbuf[fnum]->time.yr,
		    pbuf[fnum]->time.mo,
                    pbuf[fnum]->time.dy,
                    pbuf[fnum]->time.hr,
		    pbuf[fnum]->time.mt,
                    pbuf[fnum]->time.sc+pbuf[fnum]->time.us/1.0e6);
     
      if ((etime !=-1) && (atime>=etime)) break;
 
    } while (OldFitRead(fitfp,pbuf[fnum],fbuf[fnum]) !=-1);
    OldFitClose(fitfp);
    if (inxfp !=NULL) {
      fclose(inxfp);
      inxfp=fopen(argv[argc-1],"r+");
      OldFitInxFclose(inxfp,prm,irec-1);
      fclose(inxfp);
    }
    fclose(fp);
  } else {

    prm=RadarParmMake();
    fit=FitMake();

    if ((argc-arg)>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp==NULL) {
        fprintf(stderr,"Index not found.\n");
        exit(-1);
      }
      inx=FitIndexFload(fp);
      fclose(fp);
      if (inx==NULL) {
        fprintf(stderr,"Error loading index.\n");
        exit(-1);
      }
    }
    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    if (FitFread(fp,prm,fit)==-1) {
      fprintf(stderr,"Error reading file\n");
      exit(-1);
    }

    atime=TimeYMDHMSToEpoch(prm->time.yr,
		            prm->time.mo,
                            prm->time.dy,
                            prm->time.hr,
		            prm->time.mt,
                            prm->time.sc+prm->time.us/1.0e6);


    /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
      int yr,mo,dy,hr,mt;
      double sc;  
      
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;


      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      status=FitFseek(fp,yr,mo,dy,hr,mt,sc,NULL,inx);
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
      if (FitFread(fp,prm,fit)==-1) {
        fprintf(stderr,"Error reading file\n");
        exit(-1);
      }
    } else stime=atime;
   
    if (etime !=-1) {
       if (edate==-1) etime+=atime - ( (int) atime % (24*3600));
       else etime+=edate;
    }
    if (extime !=0) etime=stime+extime;

  
    do {
      if ((cpid !=-1) && (prm->cp !=cpid)) continue;
      if ((set_channel==0) &&
          (channel !=-1) && (prm->channel !=channel)) continue;
 
      if (set_channel==1) prm->channel=channel;
      atime=TimeYMDHMSToEpoch(prm->time.yr,
	  	      prm->time.mo,
                      prm->time.dy,
                      prm->time.hr,
		      prm->time.mt,
                      prm->time.sc+prm->time.us/1.0e6);
      if ((etime !=-1) && (atime>=etime)) break;


      TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d %.2d %.2d %.4d\n",
                            yr,mo,dy,hr,mt,
                            (int) sc,prm->channel,
                            prm->bmnum,prm->cp);
 
      status=FitFwrite(stdout,prm,fit);

    } while (FitFread(fp,prm,fit) !=-1);
    if (fp !=stdin) fclose(fp);
  }

  return 0; 
} 






















