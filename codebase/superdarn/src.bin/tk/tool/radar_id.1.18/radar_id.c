/* radar_id.c
   ==========
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h> 
#include "option.h"
#include "rtime.h"
#include "radar.h" 
#include "errstr.h"
#include "hlpstr.h"

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;
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

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: radar_id --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int arg=0,farg;
  int yr,mo,dy,hr,mt,isc,usc;
  double sc;
  int st=0;
  FILE *fp;
  char *envstr;
  char *tmetxt=NULL,*dtetxt=NULL;
  double tval=0,dval=0;
 


  char *nlab[2]={"","name:"};
  char *slab[2]={"","status:"};
  char *olab[2]={"","operator:"};
  char *clab[2]={"","code:"};
  char *ilab[2]={"","id:"};
  char *hlab[2]={"","location:"};
  char *plab[2]={"","pole:"};
  
  unsigned char nflg=0;
  unsigned char sflg=0;
  unsigned char oflg=0;
  unsigned char cflg=0;
  unsigned char iflg=0;
  unsigned char hflg=0;
  unsigned char pflg=0;
  unsigned char lflg=0;
 
  unsigned char aflg=0;

  unsigned char stflg=0;
  unsigned char edflg=0;

  unsigned char vb=0; 
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int status,c;

  char *tmp=NULL;
  char ststr[1024];
  int stid[256];
  int stcnt=0;

  char *nldstr="\n";
  char *tdstr="\t";
  char *dstr=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"n",'x',&nflg);
  OptionAdd(&opt,"s",'x',&sflg);
  OptionAdd(&opt,"o",'x',&oflg);
  OptionAdd(&opt,"c",'x',&cflg);
  OptionAdd(&opt,"i",'x',&iflg);
  OptionAdd(&opt,"h",'x',&hflg);
  OptionAdd(&opt,"p",'x',&pflg);

  OptionAdd(&opt,"st",'x',&stflg);
  OptionAdd(&opt,"ed",'x',&edflg);

  OptionAdd(&opt,"l",'x',&lflg);

  OptionAdd(&opt,"a",'x',&aflg);

  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);

  OptionAdd(&opt,"delim",'t',&dstr);

  farg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (farg==-1) {
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

  if (arg==argc) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }




  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  tval+=dval;

  if (tval !=0) TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  else { 
    TimeReadClock(&yr,&mo,&dy,&hr,&mt,&isc,&usc);
    sc=isc;
    tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  }

  if (dstr==NULL) {
    if (lflg) dstr=tdstr;
    else dstr=nldstr;
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
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }


  RadarLoadHardware(envstr,network);

  for (arg=farg;arg<argc;arg++) {
    strcpy(ststr,argv[arg]);
    tmp=strtok(ststr,",");
    do {
      if (isdigit(tmp[0])) stid[stcnt]=atoi(tmp);
      else stid[stcnt]=RadarGetID(network,tmp);
      if (stid[stcnt] !=-1) stcnt++;
    } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  if (stcnt==0) {
    stcnt=network->rnum;
    for (st=0;st<stcnt;st++) stid[st]=network->radar[st].id;
  }

  for (st=0;st<stcnt;st++) {
 
    
    status=RadarGetStatus(network,stid[st]);
    /* if ((aflg) && (status !=1)) continue; */
    
    radar=RadarGetRadar(network,stid[st]);
    if (radar==NULL) continue;

    if (aflg) {
      if (radar->st_time==-1) continue;
      if (tval<radar->st_time) continue;
      if ((radar->ed_time !=-1) && (tval>radar->ed_time)) continue;
    }
 
    if (iflg==1) fprintf(stdout,"%s%d%s",ilab[vb],stid[st],dstr);  

    if (cflg==1) {
      fprintf(stdout,"%s",clab[vb]);
      for (c=0;(c<RadarGetCodeNum(network,stid[st]));c++) 
        if (c !=0) fprintf(stdout,",%s",RadarGetCode(network,stid[st],c));
        else fprintf(stdout,"%s",RadarGetCode(network,stid[st],c));
      fprintf(stdout,"%s",dstr);
    }

    if (nflg==1) 
      fprintf(stdout,"%s\"%s\"%s",nlab[vb],RadarGetName(network,stid[st]),
                                  dstr);

    if (sflg==1) {
        fprintf(stdout,"%s",slab[vb]);
        if (status==1) fprintf(stdout,"active");
        else if (status==0) fprintf(stdout,"under construction");
        else if (status==-1) fprintf(stdout,"defunct");
        fprintf(stdout,"%s",dstr);
    }

    if (oflg==1) fprintf(stdout,"%s\"%s\"%s",olab[vb],
                         RadarGetOperator(network,stid[st]),dstr);

    if (stflg) {
      if (radar->st_time !=-1) {
        TimeEpochToYMDHMS(radar->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d%.2d%.2d%s",yr,mo,dy,dstr);
      } else fprintf(stdout,"-%s",dstr);
    }
      

    if (edflg) {
      if (radar->ed_time !=-1) { 
        TimeEpochToYMDHMS(radar->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d%.2d%.2d%s",yr,mo,dy,dstr);
      } else fprintf(stdout,"-%s",dstr);
 
    }



    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
    site=RadarYMDHMSGetSite(RadarGetRadar(network,stid[st]),
			    yr,mo,dy,hr,mt,(int) sc);
    

     
    if (hflg) {
      if (site !=NULL) {
        fprintf(stdout,"%s%g,%g,%g,%g,%d,%d%s",hlab[vb],
                      site->geolat,site->geolon,
	              site->alt,
		      site->boresite,site->maxbeam,site->maxrange,dstr);
      } else fprintf(stdout,"unknown%s",dstr);
    }

    if (pflg) {
      if (site !=NULL) {
        if (site->geolat>0) fprintf(stdout,"%snorth",plab[vb]);
        else fprintf(stdout,"%ssouth",plab[vb]);
      } else if (radar->site !=NULL) {
        if (radar->site[0].geolat>0) fprintf(stdout,"%snorth",plab[vb]);
        else fprintf(stdout,"%ssouth",plab[vb]);
       } else fprintf(stdout,"unknown");
      
      fprintf(stdout,"%s",dstr);
    }
    if (lflg) fprintf(stdout,"\n");    
  }
  return 0;
}






 



   











