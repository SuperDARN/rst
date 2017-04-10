/* trim_iq.c
   =========
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
#include "dmap.h"
#include "rtime.h"
#include "option.h"
#include "rprm.h"
#include "iq.h"
#include "version.h"

#include "iqread.h"
#include "iqwrite.h"
#include "iqindex.h"
#include "iqseek.h"

#include "errstr.h"
#include "hlpstr.h"




struct RadarParm *prm=NULL;
struct IQ *iq=NULL;
struct IQIndex *inx=NULL;
unsigned int *badtr=NULL;
int16 *samples=NULL;


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

  int arg=0;


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


  FILE *fp=NULL;

  int n;

  prm=RadarParmMake();
  iq=IQMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);



  if ((argc-arg)>1) {
    fp=fopen(argv[arg+1],"r");
    if (fp==NULL) {
      fprintf(stderr,"Index not found.\n");
      exit(-1);
    }
    inx=IQIndexFload(fp);
   
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
  

  if (IQFread(fp,prm,iq,&badtr,&samples)==-1)  {
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
      /* we must skip the start of the files */
      int yr,mo,dy,hr,mt;
      double sc; 
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;

      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      status=IQFseek(fp,yr,mo,dy,hr,mt,sc,NULL,inx); 
    
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
      if (IQFread(fp,prm,iq,&badtr,&samples)==-1)  {
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

     fprintf(stderr,"%s\n",prm->origin.time);
     fprintf(stderr,"%s\n",prm->origin.command);
     fprintf(stderr,"%s\n",prm->combf);
     fprintf(stderr,"%d:",prm->mppul);
     for (n=0;n<prm->mppul;n++) fprintf(stderr,"%d ",prm->pulse[n]);
     fprintf(stderr,"\n");
     fprintf(stderr,"%d\n",prm->mplgs);
     for (n=0;n<=prm->mplgs;n++) fprintf(stderr,"%d,%d ",
          prm->lag[0][n],prm->lag[1][n]);
     fprintf(stderr,"\n");


         
     IQFwrite(stdout,prm,iq,badtr,samples);
     
     atime=TimeYMDHMSToEpoch(prm->time.yr,
		    prm->time.mo,
                    prm->time.dy,
                    prm->time.hr,
		    prm->time.mt,
                    prm->time.sc+prm->time.us/1.0e6);
     TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
     if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);

      if ((etime !=-1) && (atime>=etime)) break;
      status=IQFread(fp,prm,iq,&badtr,&samples);
 
  } while (status !=-1);  
 
  if (fp !=stdin) fclose(fp);
  return 0;


}






















