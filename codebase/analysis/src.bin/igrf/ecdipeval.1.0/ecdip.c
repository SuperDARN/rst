/* ecdip.c
   =======
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
#include "igrflib.h"


struct OptionData opt;

int rst_opterr(char *txt) {
    fprintf(stderr,"Option not recognized: %s\n",txt);
    fprintf(stderr,"Please try: ecdipeval --help\n");
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
  double out[3],omlt=0.0;
  char *fmt=NULL;
  char *dfmt="%f %f\n";
  char *fname=NULL;
  FILE *fp;

  unsigned char option=0;
  unsigned char help=0;
  unsigned char version=0;
  unsigned char mag=0;
  unsigned char mlt=0;
  int c;

  char *tmetxt=NULL;
  char *dtetxt=NULL;
  double Re=6371.0;
  double dval=-1;
  double tval=-1;
  int yr,mo,dy,hr,mt,sc,dno;
  double sec;

  char txt[256];

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"i",'x',&mag);                 /* Input is in magnetic rather than geographic coords */
  OptionAdd(&opt,"mlt",'x',&mlt);               /* Longitude is in MLT rather than magnetic */
  OptionAdd(&opt,"lon",'d',&ilon);              /* Longitude [deg] */
  OptionAdd(&opt,"lat",'d',&ilat);              /* Latitude [deg] */
  OptionAdd(&opt,"alt",'d',&alt);               /* Altitude [km] */
  OptionAdd(&opt,"re",'d',&Re);                 /* Earth radius [km] */
  OptionAdd(&opt,"fmt",'t',&fmt);
  OptionAdd(&opt,"f",'t',&fname);
  OptionAdd(&opt,"t",'t',&tmetxt);              /* Time for eccentric dipole transformation */
  OptionAdd(&opt,"d",'t',&dtetxt);              /* Date for eccentric dipole transformation */

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

  if (fmt==NULL) fmt=dfmt;

  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  if (dval !=-1) {
    if (tval !=-1) {
      tval+=dval;
      TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sec);
      IGRF_SetDateTime(yr,mo,dy,hr,mt,0);
    } else {
      tval=dval;
      TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sec);
      IGRF_SetDateTime(yr,mo,dy,0,0,0);
    }
  } else {
    fprintf(stderr,"\nDate must be set, using today's date\n\n");
    IGRF_SetNow();
    IGRF_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sc,&dno);
    IGRF_SetDateTime(yr,mo,dy,0,0,0);
  }

  if (fname==NULL) {
    if (mlt) {
      if (mag) omlt = inv_ecdip_mlt(yr,mo,dy,hr,mt,sc,ilon);
      else     omlt = ecdip_mlt(yr,mo,dy,hr,mt,sc,ilon);
      out[0] = ilat;
      out[1] = omlt;
    } else {
      if (mag) ecdip2geod(ilat,ilon,alt+Re,out);
      else     geod2ecdip(ilat,ilon,alt,out);
    }
    fprintf(stdout,fmt,out[0],out[1]);
  } else {
    if (strcmp(fname,"-")==0) fp=stdin;
    else fp=fopen(fname,"r");
    if (fp==NULL) exit(1);
    while(fgets(txt,255,fp) !=NULL) {
      for (c=0;(txt[c] !=0) && (c<256);c++) 
        if (txt[c]=='#') break;
      if ((c<256) && (txt[c]=='#')) continue;
      if (sscanf(txt,"%lf %lf %lf\n",
          &ilat,&ilon,&alt) !=3) continue;
      if (mlt) {
        if (mag) omlt = inv_ecdip_mlt(yr,mo,dy,hr,mt,sc,ilon);
        else     omlt = ecdip_mlt(yr,mo,dy,hr,mt,sc,ilon);
        out[0] = ilat;
        out[1] = omlt;
      } else {
        if (mag) ecdip2geod(ilat,ilon,alt+Re,out);
        else     geod2ecdip(ilat,ilon,alt,out);
      }
      fprintf(stdout,fmt,out[0],out[1]);
    }
    if (fp !=stdin) fclose(fp);
  }
  return 0;
}
