/* igrf.c
   ======
   Author: E.G.Thomas
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "option.h"
#include "rtime.h"
#include "rmath.h"
#include "errstr.h"
#include "hlpstr.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"
#include "igrflib.h"


struct OptionData opt;

int opterr(char *txt) {
    fprintf(stderr,"Option not recognized: %s\n",txt);
    return(-1);
}

/**
 * Converts an input date from YYYYMMDD format to an epoch time in number of
 * seconds since 00:00 UT on January 1, 1970.
 **/
double strdate(char *text) {

    double tme;
    int val;
    int yr,mo,dy;

    /* Calculate day, month, and year from YYYYMMDD format date */
    val=atoi(text);
    dy=val % 100;
    mo=(val / 100) % 100;
    yr=(val / 10000);

    /* If only 2-digit year provided then assume it was pre-2000 */
    if (yr<1970) yr+=1900;

    /* Calculate epoch time of input year, month, and day */
    tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

    /* Return epoch time in number of seconds since 00:00UT on January 1, 1970 */
    return tme;

}


/**
 * Converts an input time from HHMM format to number of seconds.
 **/
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

int main(int argc,char *argv[]) {
  int arg;
 
  double ilon=0.0,ilat=0.0,alt=0.0;
  double rtp[3],brtp[3],bxyz[3];
  double bmag;
  double glat,glon,r;
  char *fmt=NULL;
  char *dfmt="%f %f %f %f\n";
  char *fname=NULL;
  FILE *fp;

  unsigned char option=0;
  unsigned char help=0;
  unsigned char version=0;
  unsigned char mag=0;
  unsigned char xyz=0;
  int c;

  int old_aacgm=0;
  char *tmetxt=NULL;
  char *dtetxt=NULL;
  double Re=-1;
  double dval=-1;
  double tval=-1;
  int yr,mo,dy,hr,mt,sc,dno;
  double sec;

  char txt[256];

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"m",'x',&mag);                 /* Input is in magnetic rather than geographic coords */
  OptionAdd(&opt,"lon",'d',&ilon);              /* Longitude [deg] */
  OptionAdd(&opt,"lat",'d',&ilat);              /* Latitude [deg] */
  OptionAdd(&opt,"alt",'d',&alt);               /* Altitude [km] */
  OptionAdd(&opt,"re",'d',&Re);                 /* Earth radius [km] */
  OptionAdd(&opt,"xyz",'x',&xyz);                /* Output in Cartesian rather than North/East/Down coords */
  OptionAdd(&opt,"fmt",'t',&fmt);
  OptionAdd(&opt,"f",'t',&fname);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);   /* Use old AACGM coefficicents rather than v2 */
  OptionAdd(&opt,"t",'t',&tmetxt);              /* Time for IGRF calculation */
  OptionAdd(&opt,"d",'t',&dtetxt);              /* Date for IGRF calculation */

  arg=OptionProcess(1,argc,argv,&opt,opterr);

  if (arg==-1) {
    exit(0);
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

  if (Re!=-1) Re*=1000.0;
  else Re=6371.0*1000.0;

  if (fmt==NULL) fmt=dfmt;

  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  if (dval !=-1) {
    if (tval !=-1) {
      tval+=dval;
      TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sec);
      IGRF_SetDateTime(yr,mo,dy,hr,mt,0);
      if (mag && !old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,0);
    } else {
      tval=dval;
      TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sec);
      IGRF_SetDateTime(yr,mo,dy,0,0,0);
      if (mag && !old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,0,0,0);
    }
  } else {
    fprintf(stderr,"\nDate must be set, using today's date\n\n");
    IGRF_SetNow();
    IGRF_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sc,&dno);
    IGRF_SetDateTime(yr,mo,dy,0,0,0);
    if (mag && !old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,0,0,0);
  }

  if (fname==NULL) {
    if (mag) {
      if (old_aacgm) AACGMConvert(ilat,ilon,alt,&glat,&glon,&r,1);
      else AACGM_v2_Convert(ilat,ilon,alt,&glat,&glon,&r,1);
    } else {
      glat = ilat;
      glon = ilon;
    }
    rtp[0] = (Re+alt*1000.0)/Re;
    rtp[1] = (90.-glat)*PI/180.;
    rtp[2] = glon*PI/180.;
    IGRF_compute(rtp,brtp);
    bspcar(rtp[1],rtp[2],brtp,bxyz);
    bmag = sqrt(bxyz[0]*bxyz[0]+bxyz[1]*bxyz[1]+bxyz[2]*bxyz[2]);
    if (xyz) fprintf(stdout,fmt,bxyz[0],bxyz[1],bxyz[2],bmag);
    else fprintf(stdout,fmt,-brtp[1],brtp[2],-brtp[0],bmag);
  } else {
    if (strcmp(fname,"-")==0) fp=stdin;
    else fp=fopen(fname,"r");
    if (fp==NULL) exit(1);
    while(fgets(txt,255,fp) !=NULL) {
      for (c=0;(txt[c] !=0) && (c<256);c++) 
      if (txt[c]=='#') break;
      if (txt[c]=='#') continue;
      if (sscanf(txt,"%lf %lf %lf\n",
          &ilat,&ilon,&alt) !=3) continue;
      if (mag) {
        if (old_aacgm) AACGMConvert(ilat,ilon,alt,&glat,&glon,&r,1);
        else AACGM_v2_Convert(ilat,ilon,alt,&glat,&glon,&r,1);
      } else {
        glat = ilat;
        glon = ilon;
      }
      rtp[0] = (Re+alt*1000.0)/Re;
      rtp[1] = (90.-glat)*PI/180.;
      rtp[2] = glon*PI/180.;
      IGRF_compute(rtp,brtp);
      bspcar(rtp[1],rtp[2],brtp,bxyz);
      bmag = sqrt(bxyz[0]*bxyz[0]+bxyz[1]*bxyz[1]+bxyz[2]*bxyz[2]);
      if (xyz) fprintf(stdout,fmt,bxyz[0],bxyz[1],bxyz[2],bmag);
      else fprintf(stdout,fmt,-brtp[1],brtp[2],-brtp[0],bmag);
      fprintf(stdout,fmt,-brtp[1],brtp[2],-brtp[0],bmag);
    }
    if (fp !=stdin) fclose(fp);
  }
  return 0;
}
