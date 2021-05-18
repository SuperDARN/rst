/* maptoefield.c
   ============= 
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
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"

#include "rfile.h"

#include "errstr.h"
#include "hlpstr.h"

#include "rmath.h"
#include "griddata.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapsolve.h"
#include "make_pgrid.h"
#include "aacgmlib_v2.h"
#include "igrflib.h"
#include "shfconst.h"
#include "calc_bmag.h"


struct OptionData opt;

struct CnvMapData *map;
struct GridData *grd;

struct CnvGrid *grid;

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
  return hr*3600L+mn*60L;
}   

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: maptoefield --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;
  int old_aacgm=0;

  int arg;
  struct RfileIndex *oinx=NULL;
  struct CnvMapIndex *inx=NULL;

  unsigned char mlt_flg=0; 
  unsigned char pot_flg=0;
  unsigned char vel_flg=0;

  int col=3;

  int yr,mo,dy,hr,mt;
  double sc;
  
  FILE *grdfp;
  int i;

  int status=0;
 
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  float latmin=60.0;
 
  float vx,vy;
  double bpolar,bmag;

  char *stime_txt=NULL;
  char *sdate_txt=NULL;

  char *extime_txt=NULL;
  char *etime_txt=NULL;
  char *edate_txt=NULL;

  double stime=-1;
  double etime=-1;
  double extime=0;
  double sdate=-1;
  double edate=-1;

  int step=1;
  
  int lat,lon;
  int32 *count=NULL;
  
  unsigned char vb=0;

  int fnum=0;

  struct DataMap *data=NULL;

  int16 syr,smo,sdy,shr,smt;
  double ssc;
  int16 eyr,emo,edy,ehr,emt;
  double esc;
  float mlt;

  float *gmlon=NULL;
  float *gmlat=NULL;
  float *mltp=NULL;
  float *pot=NULL;
  float *Enorth=NULL;
  float *Eeast=NULL;
  float *Vmag=NULL;
  float *Vazm=NULL;
  int size=0;
  int32 npnt[2];

  unsigned char *buf=NULL;
  unsigned char *bptr=NULL;

  int tme;
  int yrsec;
  int first;
  int noigrf=0;
  float decyear;
  double theta;
  int hemi=1;

  grd=GridMake();
  map=CnvMapMake();

  grid=CnvGridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"st",'t',&stime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"et",'t',&etime_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"s",'i',&step);
  OptionAdd(&opt,"l",'f',&latmin);
  OptionAdd(&opt,"mlt",'x',&mlt_flg);
  OptionAdd(&opt,"p",'x',&pot_flg);
  OptionAdd(&opt,"v",'x',&vel_flg);  

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

  if (stime_txt !=NULL)  stime=strtime(stime_txt);
  if (etime_txt !=NULL)  etime=strtime(etime_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL)  sdate=strdate(sdate_txt);
  if (edate_txt !=NULL)  edate=strdate(edate_txt);
 
  if (old) {
    if (arg<(argc-1)) {
       FILE *fp;
       fprintf(stderr,"Loading index.\n");
       fp=fopen(argv[argc-1],"r");
       if (fp !=NULL) {
         oinx=RfileLoadIndex(fp);
         fclose(fp);
       }
    }
  } else {
    if (arg<(argc-1)) {
       FILE *fp;
       fprintf(stderr,"Loading index.\n");
       fp=fopen(argv[argc-1],"r");
       if (fp !=NULL) {
         inx=CnvMapIndexFload(fp);
         fclose(fp);
       }
    }
  }

  col+=2+pot_flg+2*vel_flg;

  if (arg !=argc) grdfp=fopen(argv[arg],"r");
  else grdfp=stdin;
  if (grdfp==NULL) {
     fprintf(stderr,"File not found.\n");
     exit(-1);
  }

  if (old) {
    if (OldCnvMapFread(grdfp,map,grd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  } else {
    if (CnvMapFread(grdfp,map,grd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  }

  if ((map->hemisphere==-1) && (latmin>0)) latmin=-latmin;

  if (map->hemisphere==1) {
    bpolar = BNorth;
    hemi = 1;
  } else {
    bpolar = BSouth;
    hemi = -1;
  }

  if (stime !=-1) { /* we must skip the start of the files */
    int yr,mo,dy,hr,mt;
    double sc;  

    if (stime==-1) stime= ( (int) grd->st_time % (24*3600));
    if (sdate==-1) stime+=grd->st_time - ( (int) grd->st_time % (24*3600));
    else stime+=sdate;

    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    if (old) status=OldCnvMapFseek(grdfp,yr,mo,dy,hr,mt,sc,oinx,NULL);
    else status=CnvMapFseek(grdfp,yr,mo,dy,hr,mt,sc,NULL,inx);
    if (status ==-1) {
      fprintf(stderr,"File does not contain the requested interval.\n");
      exit(-1);
    }
    if (old) OldCnvMapFread(grdfp,map,grd);
    else CnvMapFread(grdfp,map,grd);
  } else stime=grd->st_time;

  if (etime !=-1) {
    if (edate==-1) etime+=grd->st_time - ( (int) grd->st_time % (24*3600));
    else etime+=edate;
  } 

  if (extime !=0) etime=stime+extime;

  first = 1;
  do {

    if (vb !=0) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",dy,mo,yr,hr,mt,(int) sc);
    }

    tme = (grd->st_time + grd->ed_time)/2.0;
    TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int)sc);
    decyear = yr + (float)yrsec/TimeYMDHMSToYrsec(yr,12,31,23,59,59);

    noigrf = map->noigrf;

    if (first) {
      if (!noigrf)    IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      first = 0;
    }

    if (fnum==0)  {
      make_pgrid(latmin,grid);
      count=malloc(sizeof(int)*grid->num);
    }
    memset(count,0,grid->num*sizeof(int));
   
    grid->type=2;
    CnvMapSolve(map,grid,decyear,old_aacgm);

    if (pot_flg !=0) {
      grid->type=0;
      CnvMapSolve(map,grid,decyear,old_aacgm);
      for (i=0;i<grid->num;i++) if (fabs(grid->lat[i])<fabs(map->latmin)) 
        grid->mag[i]=0;
    }

    for (i=0;i<grd->vcnum;i++) {
      lat= (fabs(grd->data[i].mlat) - 60.0) / 1.0;
      if (lat<0) continue;
      lon= grd->data[i].mlon /2.0;
      count[lat*180+lon]++;
    }

    size=sizeof(int16)+4*sizeof(float);
    if (pot_flg) size+=sizeof(float);
    if (vel_flg) size+=2*sizeof(float);
    size=size*grid->num;
      
    buf=malloc(size);
    if (buf==NULL) break;
    bptr=buf;
   
    gmlat=(float *) bptr;
    bptr+=grid->num*sizeof(float);
    if (mlt_flg) mltp=(float *) bptr;
    else gmlon=(float *) bptr;
    bptr+=grid->num*sizeof(float);
   
    Enorth=(float *) bptr;
    bptr+=grid->num*sizeof(float);
    Eeast=(float *) bptr;
    bptr+=grid->num*sizeof(float);
    if (pot_flg) {
      pot=(float *) bptr;
      bptr+=grid->num*sizeof(float);
    }

    if (vel_flg) {
      Vmag=(float *) bptr;
      bptr+=grid->num*sizeof(float);
      Vazm=(float *) bptr;
      bptr+=grid->num*sizeof(float);
    }
    for (i=0;i<grid->num;i++) {
      gmlat[i]=grid->lat[i];
      if (mlt_flg) {
        mltp[i]=grid->lon[i]/15.0+map->mlt.av;
        if (mltp[i]>24.0) mltp[i]-=24.0;  
      } else gmlon[i]=grid->lon[i];
      Enorth[i]=-grid->ex[i];
      Eeast[i]=grid->ey[i];

      if (pot_flg) pot[i]=grid->mag[i];
      if (vel_flg) {
        if (noigrf) { /* use diople value for B */
          theta = (90.-hemi*grid->lat[i])*PI/180.0;
          bmag = bpolar*(1.0 - 3.0 * Altitude/Re)*
                 sqrt(3.0*(cos(theta)*cos(theta))+1.0)/2.0;
        } else {
          bmag = -calc_bmag(hemi*grid->lat[i],grid->lon[i],decyear,old_aacgm);
        }
          
        vx=grid->ey[i]/bmag;
        vy=-grid->ex[i]/bmag;
        Vmag[i]=sqrt(vx*vx+vy*vy);
        if (map->hemisphere>=0) Vazm[i]=atan2(vy,-vx)*180.0/PI;
        else Vazm[i]=atan2(vy,vx)*180.0/PI;
      }
    }
      
    npnt[0]=grid->nlon;
    npnt[1]=grid->nlat;

    TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    eyr=yr;
    emo=mo;
    edy=dy;
    ehr=hr;
    emt=mt;
    esc=sc;
    TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    syr=yr;
    smo=mo;
    sdy=dy;
    shr=hr;
    smt=mt;
    ssc=sc;
    mlt=map->mlt.av;

    data=DataMapMake();

    DataMapAddScalar(data,"start.year",DATASHORT,&syr);
    DataMapAddScalar(data,"start.month",DATASHORT,&smo);
    DataMapAddScalar(data,"start.day",DATASHORT,&sdy);
    DataMapAddScalar(data,"start.hour",DATASHORT,&shr);
    DataMapAddScalar(data,"start.minute",DATASHORT,&smt);
    DataMapAddScalar(data,"start.second",DATADOUBLE,&ssc);

    DataMapAddScalar(data,"end.year",DATASHORT,&eyr);
    DataMapAddScalar(data,"end.month",DATASHORT,&emo);
    DataMapAddScalar(data,"end.day",DATASHORT,&edy);
    DataMapAddScalar(data,"end.hour",DATASHORT,&ehr);
    DataMapAddScalar(data,"end.minute",DATASHORT,&emt);
    DataMapAddScalar(data,"end.second",DATADOUBLE,&esc);
    DataMapAddScalar(data,"MLT",DATAFLOAT,&mlt);
      
    DataMapAddArray(data,"vector.mlat",DATAFLOAT,2,npnt,gmlat);
    
    if (mltp !=NULL)
      DataMapAddArray(data,"vector.mlt",DATAFLOAT,2,npnt,mltp);
    else DataMapAddArray(data,"vector.mlon",DATAFLOAT,2,npnt,gmlon);
   
    DataMapAddArray(data,"vector.E.north",DATAFLOAT,2,npnt,Enorth); 
    DataMapAddArray(data,"vector.E.east",DATAFLOAT,2,npnt,Eeast);

    if (pot !=NULL) 
      DataMapAddArray(data,"vector.potential",DATAFLOAT,2,npnt,pot);
    if (Vmag !=NULL) 
      DataMapAddArray(data,"vector.V.mag",DATAFLOAT,2,npnt,Vmag);
    if (Vazm !=NULL) 
      DataMapAddArray(data,"vector.V.azm",DATAFLOAT,2,npnt,Vazm);
    DataMapAddArray(data,"vector.data",DATAINT,2,npnt,count);
    DataMapFwrite(stdout,data);
    
    DataMapFree(data);
    free(buf);
       
    fnum++;
    if (old) for (i=0;i<step;i++) status=OldCnvMapFread(grdfp,map,grd);
    else for (i=0;i<step;i++) status=CnvMapFread(grdfp,map,grd);
    if ((etime !=-1) && (grd->ed_time>=etime)) break; 
  } while (status !=-1);
  
  if (grdfp !=stdin) fclose(grdfp);
  return 0;  
}

