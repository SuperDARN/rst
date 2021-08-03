/* trim_grid.c
   ===========
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
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "gridindex.h"
#include "gridseek.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "radar.h"
#include "errstr.h"
#include "hlpstr.h"





char *fname=NULL;
char *iname=NULL;
FILE *fp;

struct GridData *ircd;
struct GridData *orcd;
struct GridData *arcd;

struct OptionData opt;

struct RadarNetwork *network;  

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
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return (double) hr*3600L+mn*60L;
}   

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: trim_grid --help\n");
  return(-1);
}
  
int main(int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  struct RfileIndex *oinx=NULL;
  struct GridIndex *inx=NULL;


  char *envstr;
  int st_id[32];
  int s;
  double err[3]={100,100,100};
  int st_cnt=0;
  int intg=0;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double sdte=-1;
  double edte=-1;

  double ssec=-1; /* specified start time */
  double esec=-1; /* end time */
  double exsec=0;

  int yr,mo,dy,hr,mt;
  double sc;

  char *ststr=NULL;
  char *chnstr=NULL;
  int channel=-1;

  int rnum=0;
  int i,j;

  int rcnt=0;

  ircd=GridMake();
  orcd=GridMake();
  arcd=GridMake();

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

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"exc",'t',&ststr);
  OptionAdd(&opt,"i",'i',&intg);
 
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



  if (exstr !=NULL) exsec=strtime(exstr);
  if (stmestr !=NULL) ssec=strtime(stmestr);
  if (etmestr !=NULL) esec=strtime(etmestr);
  if (sdtestr !=NULL) sdte=strdate(sdtestr);
  if (edtestr !=NULL) edte=strdate(edtestr);

  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }
    
  if (ststr !=NULL) {
    char *tmp;
    tmp=strtok(ststr,",");
    do {
       st_id[st_cnt]=RadarGetID(network,tmp);
       if (st_id[st_cnt] !=-1) st_cnt++;
       else {
	 st_id[st_cnt]=atoi(tmp);
         st_cnt++;
       }
     } while ((tmp=strtok(NULL,",")) !=NULL);
  }



  if (argc-arg>1) {
    fname=argv[arg];
    iname=argv[arg+1];
  } else if (arg !=argc) fname=argv[arg];

  if (old) {
   if (iname !=NULL) {
       fp=fopen(iname,"r");
       if (fp !=NULL) { 
         oinx=RfileLoadIndex(fp);
         fclose(fp);
       }
    }
  } else {
    if (argc-arg>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp !=NULL) { 
         inx=GridIndexFload(fp);
         fclose(fp);
      }
    } 
  }

  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;

  if (old) s=OldGridFread(fp,ircd);
  else s=GridFread(fp,ircd);
 
  if (s !=-1) {
      if (ssec==-1) ssec=((int) ircd->st_time % (24*3600));
      if (sdte==-1) ssec+=ircd->st_time - ((int) ircd->st_time % (24*3600));
      else ssec+=sdte;
      if (esec !=-1) {
         if (edte==-1) esec+=ircd->st_time - ((int) ircd->st_time % (24*3600));
         else esec+=edte;
      }
      if (exsec !=0) esec=ssec+exsec;
  }

  if (fp !=stdin) { /* do a search as we are reading from a file */
   
     TimeEpochToYMDHMS(ssec,&yr,&mo,&dy,&hr,&mt,&sc);
     if (old) {
       if (ircd->st_time<ssec) {
         OldGridFseek(fp,yr,mo,dy,hr,mt,(int) sc,oinx,NULL);
         s=OldGridFread(fp,ircd);
       }
     } else {
       if (ircd->st_time<ssec) {
         GridFseek(fp,yr,mo,dy,hr,mt,(int) sc,NULL,inx);
         s=GridFread(fp,ircd);
       }
     }
  }

  while (s !=-1)  {
    rnum++;
    
    if (fp==stdin) { /* not reading from a file so do a manual search */   
      if (ircd->st_time<ssec) {
        if (old) s=OldGridFread(fp,ircd);
        else s=GridFread(fp,ircd);
        continue;
      }
    }
    
    if ((esec !=-1) && (ircd->ed_time>esec)) break;   
         
    for (i=0;i<ircd->stnum;i++) {
      for (j=0;(j<st_cnt) &&  (ircd->sdata[i].st_id !=st_id[j]);j++);
      if (j<st_cnt) {
        if (channel==-1) ircd->sdata[i].st_id=-1;
        else if (ircd->sdata[i].chn==channel) ircd->sdata[i].st_id=-1;
      }
    }  

    for (i=0;i<ircd->vcnum;i++) {
      for (j=0;(j<st_cnt) &&  (ircd->data[i].st_id !=st_id[j]);j++);
      if (j<st_cnt) {
        if (channel==-1) ircd->data[i].st_id=-1;
        else if (ircd->data[i].chn==channel) ircd->data[i].st_id=-1;
      }
    }
    
    if (intg !=0) { /* must do integral calculation */
   
      if ((rcnt>0) && (ircd->st_time>=orcd->ed_time)) {
        /* write the record out */ 
        GridAdd(orcd,NULL,rcnt);
        GridIntegrate(arcd,orcd,err);
        if (vb) {
           TimeEpochToYMDHMS(arcd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
		   "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",yr,mo,dy,hr,mt,(int) sc);
           TimeEpochToYMDHMS(arcd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
             "%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d (%d stations)\n",yr,mo,dy,
		   hr,mt,(int) sc,arcd->vcnum,arcd->stnum);

	}
        if (old) OldGridFwrite(stdout,arcd);
        else GridFwrite(stdout,arcd);
        rcnt=0;
      }

      if (rcnt==0) {
        GridCopy(orcd,ircd);
        orcd->ed_time=orcd->st_time+intg;
        rcnt++;
      } else {
        GridAdd(orcd,ircd,rcnt);
        rcnt++;
        
      }
    } else {
       if (vb) {
         TimeEpochToYMDHMS(ircd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,
	         "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",yr,mo,dy,hr,mt,(int) sc);
         TimeEpochToYMDHMS(ircd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,
            "%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d (%d stations)\n",yr,mo,dy,
	     hr,mt,(int) sc,ircd->vcnum,ircd->stnum);

         
       }

       if (old) OldGridFwrite(stdout,ircd);
       else GridFwrite(stdout,ircd);
    }
    if (old) s=OldGridFread(fp,ircd);
    else s=GridFread(fp,ircd);
  }
  if (intg !=0) {
    if (rcnt>0) {
    
      /* write the record out */
   
      GridAdd(orcd,NULL,rcnt);
      
      GridIntegrate(arcd,orcd,err);
    
      if (vb) {
           TimeEpochToYMDHMS(arcd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
		   "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",yr,mo,dy,hr,mt,(int) sc);
           TimeEpochToYMDHMS(arcd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
             "%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d (%d stations)\n",yr,mo,dy,
		   hr,mt,(int) sc,arcd->vcnum,arcd->stnum);
      }


      if (old) OldGridFwrite(stdout,arcd);
      else GridFwrite(stdout,arcd);

      rcnt=0;
    }
  }
 
  if (fp !=stdin) fclose(fp); 
  return 0; 
}







