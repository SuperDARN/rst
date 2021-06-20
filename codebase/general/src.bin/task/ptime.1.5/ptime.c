/* ptime.c
   =======
   Author: R.J.Barnes
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

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "option.h"
#include "rtime.h"

#include "errstr.h"
#include "hlpstr.h"

struct OptionData opt;

#define BLEN 1024

double strtime(char *text,char *mult) {
  double t[4];
  double tt;
  int i,j=0,s=1,c=0;

  *mult='s';

  if ((text[0]=='+') || (text[0]=='-')) {
    j=1;
    if (text[0]=='-') s=-1;
  }

  for (i=j;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) {
    if (! isdigit(text[i-1])) {
      *mult=text[i-1];
      text[i-1]=0;
    }
    return atoi(text+j)*s;
  }
  do {
    text[i]=0;
    t[c]=atof(text+j);
    c++;
    if (c>3) break;
    j=i+1;
    for (i=j;(text[i] !=':') && (text[i] !=0);i++);
  } while (text[i] !=0);   
  text[i]=0;
  t[c]=atof(text+j);
  c++;
 
  if (c==3) tt=t[0]*(24*3600)+t[1]*3600+t[2]*60;
  else if (c==2) tt=t[0]*3600+t[1]*60;
  else tt=t[0]*60; 

  return (double) s*tt;
}   


int output(FILE *fp,char *fmt,double tval) {

  int n=0;
  int yr,mo,dy,hr,mt;
  double sc,jd;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  jd=TimeYMDHMSToJulian(yr,mo,dy,hr,mt,sc);  

  while(fmt[n] !=0) {
    switch (fmt[n]) {
    case '%':
      n++;
      if (fmt[n]==0) break;
      switch (fmt[n]) {
      case 'J':
        fprintf(fp,"%lf",jd);
        break;
      case 'q':
        fprintf(fp,"%lf",tval-floor(tval));
        break;
      }
      n++;
      break;
    case '\\':
      n++;
      switch (fmt[n]) {
      case 'n':
        fputc('\n',fp);
        break;
      case 't':
        fputc('\t',fp);
        break;
      case '"':
        fputc('\"',fp);
        break;
      case '\'':
        fputc('\'',fp);
        break;
      case '\\':
        fputc('\\',fp);
        break;
      }
      n++;
      break;
    default :
      fputc(fmt[n],fp);
      n++;
    }
  }
  return 0;
}

double apply_delta(double tval,double delta,char mult) {
  int yr,mo,dy,hr,mt;
  double sc;
  int yrmo;
  switch (mult) {
  case 'Y':
    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
    yr+=delta;
    tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    break;    
  case 'm':
    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
    yrmo=12*yr+(mo-1)+delta;
    yr=floor(yrmo/12);
    mo=yrmo-12*yr+1;
    tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);    
    break;
  case 'd':
    tval+=delta*24*3600;
    break;
  case 'H':
    tval+=delta*3600;
    break;
  case 'M':
    tval+=delta*60;
    break;  
  default:
    tval+=delta;
    break;
  }
  return tval;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: ptime --help\n");
  return(-1);
}

int main(int argc, char *argv[]) {
 
  int arg;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  
  char txt[BLEN],buf[BLEN];
  char fmt[BLEN]={"%s\n"};
  char *ufmt=NULL;
  char *dstr=NULL;

  char mult='s';
  double delta=0;
  
  unsigned char lflg=0;
  unsigned char sflg=0; /* stdin */

  time_t t;
  double tval;
  struct tm *tm=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"f",'t',&ufmt);
  OptionAdd(&opt,"d",'t',&dstr);
  
  OptionAdd(&opt,"s",'x',&sflg);
  OptionAdd(&opt,"l",'x',&lflg);
 

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

  if (ufmt !=NULL) strcpy(fmt,ufmt);

  if (dstr !=NULL) delta=strtime(dstr,&mult);

  if (sflg==0) {
    if (arg==argc) tval=time(NULL);
    else tval=atof(argv[arg]);

    tval=apply_delta(tval,delta,mult);
    t=(time_t) tval;

    if (lflg) tm=localtime(&t);
    else tm=gmtime(&t);

    
    strftime(txt,BLEN-1,fmt,tm);
    output(stdout,txt,tval);         

  } else {
    while(fgets(buf,BLEN-1,stdin) !=NULL) {
      tval=atof(buf);
      tval=apply_delta(tval,delta,mult);
      t=(time_t) tval;
      if (lflg) tm=localtime(&t);
      else tm=gmtime(&t);

      strftime(txt,BLEN-1,fmt,tm);
      output(stdout,txt,tval);
    }
  }

  exit(0);
}


