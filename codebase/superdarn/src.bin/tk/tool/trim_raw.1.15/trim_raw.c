/* trim_raw.c
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
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "version.h"

#include "rawread.h"
#include "rawwrite.h"
#include "rawindex.h"
#include "rawseek.h"

#include "oldrawread.h"
#include "oldrawwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct RadarParm *prm;
struct RawData *raw;
struct RawIndex *inx=NULL;

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

  int arg=0;

  int recnum=0;
  float thr=-1;
  int status=0;
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

  int yr,mo,dy,hr,mt;
  double sc;
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;

  struct OldRawFp *rawfp=NULL;
  FILE *fp=NULL;

  char vstring[256];

  prm=RadarParmMake();
  raw=RawMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'f',&thr);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

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


  if ((old) && (argc-arg<2)) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);


   if (old) {
     rawfp=OldRawOpen(argv[arg],NULL);
     if (rawfp==NULL) {
       fprintf(stderr,"File not found.\n");
       exit(-1);
     }
  } else { 
    if ((argc-arg)>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp==NULL) {
	fprintf(stderr,"Index not found.\n");
        exit(-1);
      }
      inx=RawIndexFload(fp);

      
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
  }

   if (old) {
     if (OldRawRead(rawfp,prm,raw)==-1)  {
       fprintf(stderr,"Error reading file\n");
       exit(-1);
     }
   } else {
     if (RawFread(fp,prm,raw)==-1)  {
       fprintf(stderr,"Error reading file\n");
       exit(-1);
     }
   }

   atime=TimeYMDHMSToEpoch(prm->time.yr,
	   	           prm->time.mo,
                           prm->time.dy,
                           prm->time.hr,
		           prm->time.mt,
                           prm->time.sc+prm->time.us/1.0e6);

   /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
      /* we must skip the start of the files */
      int yr,mo,dy,hr,mt;
      double sc; 
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;

      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      if (old) status=OldRawSeek(rawfp,yr,mo,dy,hr,mt,sc,NULL);
      else status=RawFseek(fp,yr,mo,dy,hr,mt,sc,NULL,inx); 
    
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
 
      if (old) {
         if (OldRawRead(rawfp,prm,raw)==-1)  {
           fprintf(stderr,"Error reading file\n");
           exit(-1);
         }
      } else {
        if (RawFread(fp,prm,raw)==-1)  {
           fprintf(stderr,"Error reading file\n");
           exit(-1);
        }
      }

    } else stime=atime;
   
    if (etime !=-1) {
       if (edate==-1) etime+=atime - ( (int) atime % (24*3600));
       else etime+=edate;
    }

   if (extime !=0) etime=stime+extime;

   if (thr !=-1) raw->thr=thr;

   if (old) {
     fp=fopen(argv[argc-1],"w");
     if (fp==NULL) {
       fprintf(stderr,"Could not open output file.\n");
       exit(-1);
     }
     sprintf(vstring,"%d.%.3d",raw->revision.major,raw->revision.minor);
     if (OldRawHeaderFwrite(fp,"rawwrite",vstring,raw->thr,
			    "trimmed with trim_raw") !=0) {
       fprintf(stderr,"Could not write header.\n");
       exit(-1);
     }
   }


 
   do {
     if (thr !=-1) raw->thr=thr;
     
     atime=TimeYMDHMSToEpoch(prm->time.yr,
		    prm->time.mo,
                    prm->time.dy,
                    prm->time.hr,
		    prm->time.mt,
                    prm->time.sc+prm->time.us/1.0e6);

     if ((etime !=-1) && (atime>=etime)) break;

     if (old) {
       recnum++;
       OldRawFwrite(fp,"rawwrite",prm,raw,recnum,NULL);
     } else RawFwrite(stdout,prm,raw);

     TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
     if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);

     if (old) status=OldRawRead(rawfp,prm,raw);
     else status=RawFread(fp,prm,raw);
  } while (status !=-1);  
  if (old) OldRawClose(rawfp);
  if (fp !=stdin) fclose(fp);
  return 0;


}






















