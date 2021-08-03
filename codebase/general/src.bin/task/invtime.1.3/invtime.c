/* invtime.c
   =========
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "rtime.h"
#include "option.h"
#include "hlpstr.h"




char buffer[256];
char cflg[1024];

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

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: invtime --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int arg;
  FILE *fp=NULL;
  unsigned char day=0;
  int status=0;
  int yr,mo,dy,hr,mt;
  double sc;
  double etime;
  double dtval=-1;
  int i,c;
  char *dtstr=NULL;
  char *cstr=NULL;
  char *vstr=NULL;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"d",'x',&day);
  OptionAdd(&opt,"sd",'t',&dtstr);
  OptionAdd(&opt,"c",'t',&cstr);

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
 
  if (cstr !=NULL) {
    char *tmp;
    tmp=strtok(cstr,",");
    do {
       c=atoi(tmp);
       if (c<2) continue;
       cflg[c-2]=1;
      } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  fprintf(stderr,"%s\n",dtstr);
  if (dtstr !=NULL) dtval=strdate(dtstr);

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;
  
  while ((fscanf(fp,"%lg",&etime)==1) && 
	 (status==0)) {

    if (day==1) {
      hr=(int) etime/3600;
      mt=( (int) etime/60) % 60;
      sc=( (int) etime % 60);
      if (dtval !=-1) { 
        etime+=dtval;
        TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);
      }
    } else TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);

    if ((day==0) || (dtval !=-1)) 
       fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d",
                        yr,mo,dy,hr,mt,(int) sc);
    else fprintf(stdout,"%.2d %.2d %.2d",hr,mt,(int) sc);
    if (fgets(buffer,256,fp)==NULL) {
      status=-1;
      break;
    }

    /* okay now print out just the columns asked for */
 
    vstr=strtok(buffer," \t\n");
    i=0;
    do {
      if (cflg[i]==1) fprintf(stdout," %s",vstr);
      i++;
    } while ((vstr=strtok(NULL," \t\n")) !=NULL);
    fprintf(stdout,"\n");
  }

  if (fp !=stdin) fclose(fp);
  return 0;

}
  
 







