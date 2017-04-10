/* mlt.c
   =====
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "option.h"
#include "rtime.h"
#include "errstr.h"
#include "hlpstr.h"
#include "mlt.h"



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


int main(int argc,char *argv[]) {
  int arg;
  int yr,mo,dy,hr,mt,isc,usc;
  double sc;
  double mlon=0.0;
  double mlt;
  char *fmt=NULL;
  char *dfmt="%f\n";
  char *fname=NULL;
  FILE *fp;

  char *tmetxt=NULL;
  char *dtetxt=NULL;
  unsigned char option=0;
  unsigned char help=0;

  double dval=0,tval=0;
  int c;

  char txt[256];
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  OptionAdd(&opt,"l",'d',&mlon);
  OptionAdd(&opt,"fmt",'t',&fmt);
  OptionAdd(&opt,"f",'t',&fname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (fmt==NULL) fmt=dfmt;

  if (fname==NULL) {
    if (tmetxt !=NULL) tval=strtime(tmetxt);
    if (dtetxt !=NULL) dval=strdate(dtetxt);
    tval+=dval;
    if (tval !=0) {
      TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
      isc=sc;
    } else TimeReadClock(&yr,&mo,&dy,&hr,&mt,&isc,&usc);
    mlt=MLTConvertYMDHMS(yr,mo,dy,hr,mt,isc,mlon);
    fprintf(stdout,fmt,mlt);    
  } else {
    if (strcmp(fname,"-")==0) fp=stdin;
    else fp=fopen(fname,"r");
    if (fp==NULL) exit(1);
    while(fgets(txt,255,fp) !=NULL) {
      for (c=0;(txt[c] !=0) && (c<256);c++) 
      if (txt[c]=='#') break;
      if (txt[c]=='#') continue;
      if (sscanf(txt,"%d %d %d %d %d %lf %lf\n",
          &yr,&mo,&dy,&hr,&mt,&sc,&mlon) !=7) continue;
      isc=sc;
      mlt=MLTConvertYMDHMS(yr,mo,dy,hr,mt,isc,mlon);
      fprintf(stdout,fmt,mlt);  
    }
  }
  return 0;
}
