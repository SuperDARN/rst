/* etime.c
   =======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtime.h"
#include "option.h"
#include "hlpstr.h"


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
  if (text[i]==0) return atof(text);
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


char buffer[256];

struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
  FILE *fp=NULL;
  int status=0;
  int yr,mo,dy,hr,mt,sc;

  double stime=-1;
  double etime=-1;
  double extime=0;

  double sdate=-1;
  double edate=-1;

  double tval=0;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;

  char *exstr=NULL;

  unsigned char day=0;
  unsigned char year=0;
  unsigned char rel=0;
  unsigned char help=0;
  unsigned char option=0;
  
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"day",'x',&day);
  OptionAdd(&opt,"year",'x',&year);
  OptionAdd(&opt,"rel",'x',&rel);

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

  if ((stime !=-1) && (sdate !=-1)) stime+=sdate;

  if (etime !=-1) {
    if (edate==-1) etime+=stime - ( (int) stime % (24*3600));
    else etime+=edate;
  }
  if ((stime !=-1) && (extime !=0)) etime=stime+extime;
  
  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;
  
  while ((fscanf(fp,"%d %d %d %d %d %d",&yr,&mo,&dy,&hr,&mt,&sc)==6) && 
	 (status==0)) {

   if (fgets(buffer,256,fp)==NULL) {
      status=-1;
      break;
    }

    tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  
    if ((stime !=-1) && (tval<stime)) continue;
    if ((etime !=-1) && (tval>etime)) break;
    

    if (rel) {
      if (stime==-1) {
        stime=tval;
        etime=tval+extime;
      }
      tval=tval-stime;
    }
    if (year) tval=tval-TimeYMDHMSToYrsec(yr,0,0,0,0,0);
    if (day==0) fprintf(stdout,"%d",(int) tval);
    else fprintf(stdout,"%d",(int) tval % (24*3600));

    fprintf(stdout,buffer);
  }
  if (fp !=stdin) fclose(fp);
  return 0;

}
  
 







