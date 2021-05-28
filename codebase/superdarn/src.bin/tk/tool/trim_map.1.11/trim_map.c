/* trim_map.c
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
#include <math.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"



#include "errstr.h"
#include "hlpstr.h"



char *fname=NULL;
char *iname=NULL;
FILE *fp;

struct GridData *grd;
struct CnvMapData  *map;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: trim_map --help\n");
  return(-1);
}

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


  
int main(int argc,char *argv[]) {

  int old=0;
  
  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  struct RfileIndex *oinx=NULL;
  struct CnvMapIndex *inx=NULL;


  int status;

  int yr,mo,dy,hr,mt;
  double sc;

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

  int major=-1;
  int minor=-1;
  char *source=NULL;

  int rnum=0;
 
  grd=GridMake();
  map=CnvMapMake();

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
  
  OptionAdd(&opt,"s",'t',&source);
  OptionAdd(&opt,"major",'i',&major);
  OptionAdd(&opt,"minor",'i',&minor);

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
         inx=CnvMapIndexFload(fp);
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

  if (old) status=OldCnvMapFread(fp,map,grd);
  else status=CnvMapFread(fp,map,grd);

  if (status !=-1) {
      if (ssec==-1) ssec=((int) map->st_time % (24*3600));
      if (sdte==-1) ssec+=map->st_time - ((int) map->st_time % (24*3600));
      else ssec+=sdte;
      if (esec !=-1) {
         if (edte==-1) esec+=map->st_time - ((int) map->st_time % (24*3600));
         else esec+=edte;
      }
      if (exsec !=0) esec=ssec+exsec;
  }


  if (fp !=stdin) { /* do a search as we are reading from a file */
     int yr,mo,dy,hr,mt;
     double sc;
     TimeEpochToYMDHMS(ssec,&yr,&mo,&dy,&hr,&mt,&sc);
     if (old) {
       if (map->st_time<ssec) OldCnvMapFseek(fp,yr,mo,dy,hr,mt,(int) sc,
					    oinx,NULL);
     } else {
       if (map->st_time<ssec) CnvMapFseek(fp,yr,mo,dy,hr,mt,(int) sc,NULL,inx);
     }
  }


  while (status !=-1) {  

   
    rnum++;
    if (map->st_time<ssec) {
      if (old) status=OldCnvMapFread(fp,map,grd);
      else status=CnvMapFread(fp,map,grd);
      continue;
    }
    if ((esec !=-1) && (map->ed_time>esec)) break;   

    if (vb==1) {
       TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
       fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
    }  
            
    if (source !=NULL) strcpy(map->source,source);
    if (major !=-1) map->major_rev=major;
    if (minor !=-1) map->minor_rev=minor;
   
    if (old) OldCnvMapFwrite(stdout,map,grd);
    else CnvMapFwrite(stdout,map,grd);
    if (old) status=OldCnvMapFread(fp,map,grd);
    else status=CnvMapFread(fp,map,grd);
    
  }

  if (fp !=stdin) fclose(fp); 
  return 0; 
}







