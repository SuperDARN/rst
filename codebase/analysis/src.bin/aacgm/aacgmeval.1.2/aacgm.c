/* aacgm.c
   =======
   Author: R.J.Barnes
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
#include "errstr.h"
#include "hlpstr.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"


struct OptionData opt;

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

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: aacgmeval --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int arg;
 
  double ilon=0.0,ilat=0.0,alt=0.0;
  double olon=0.0,olat=0.0,r;
  char *fmt=NULL;
  char *dfmt="%f %f\n";
  char *fname=NULL;
  FILE *fp;

  unsigned char option=0;
  unsigned char help=0;
  unsigned char version=0;
  unsigned char flag=0;
  int c;

  int old_aacgm=0;
  char *tmetxt=NULL;
  char *dtetxt=NULL;
  double dval=-1;
  double tval=-1;
  int yr,mo,dy,hr,mt,sc,dno;
  double sec;

  char txt[256];
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"i",'x',&flag);
  OptionAdd(&opt,"lon",'d',&ilon);
  OptionAdd(&opt,"lat",'d',&ilat);
  OptionAdd(&opt,"alt",'d',&alt);
  OptionAdd(&opt,"fmt",'t',&fmt);
  OptionAdd(&opt,"f",'t',&fname);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);   /* Use old AACGM coefficicents rather than v2 */
  OptionAdd(&opt,"t",'t',&tmetxt);              /* Time for AACGM_v2 transformation */
  OptionAdd(&opt,"d",'t',&dtetxt);              /* Date for AACGM_v2 transformation */

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

  if (old_aacgm==0) {
    if (tmetxt !=NULL) tval=strtime(tmetxt);
    if (dtetxt !=NULL) dval=strdate(dtetxt);

    if (dval !=-1) {
      if (tval !=-1) {
        tval+=dval;
        TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sec);
        AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,0);
      } else {
        tval=dval;
        TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sec);
        AACGM_v2_SetDateTime(yr,mo,dy,0,0,0);
      }
    } else {
      fprintf(stderr,"\nDate must be set for AACGM_v2, using today's date\n\n");
      AACGM_v2_SetNow();
      AACGM_v2_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sc,&dno);
      AACGM_v2_SetDateTime(yr,mo,dy,0,0,0);
    }
  }

  if (fname==NULL) {
    if (old_aacgm) AACGMConvert(ilat,ilon,alt,&olat,&olon,&r,flag);
    else AACGM_v2_Convert(ilat,ilon,alt,&olat,&olon,&r,flag);
    fprintf(stdout,fmt,olat,olon);    
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
      if (old_aacgm) AACGMConvert(ilat,ilon,alt,&olat,&olon,&r,flag);
      else AACGM_v2_Convert(ilat,ilon,alt,&olat,&olon,&r,flag);
      fprintf(stdout,fmt,olat,olon);    
    }
    if (fp !=stdin) fclose(fp);
  }
  return 0;
}
