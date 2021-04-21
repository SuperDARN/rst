/* smrread.c
   =========
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
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"




int SmrFreadData(FILE *fp,int ngood,struct RadarParm *prm,
                 struct FitData *fit) {

  int sptr;
   
  /* read in the fitacf data from the summary file*/

  int n;
  int *index=NULL; /* array to store range indices*/
   
  sptr=ftell(fp);
  index=malloc(sizeof(int)*ngood);
  if (index==NULL) return -1;

  FitSetRng(fit,prm->nrang);

  for(n=0;n<ngood;n++) 
   if (fscanf(fp,"%d",&index[n]) !=1) {
      free(index);
      return -1;
    }
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&fit->rng[index[n]].p_l) !=1) {
      free(index);
      return -1;  
    }
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&fit->rng[index[n]].v) !=1) {
      free(index);
      return -1;
    }
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&fit->rng[index[n]].w_l) !=1) {
     free(index);
      return -1;
    }
  for(n=0;n<ngood;n++) {
    fit->rng[index[n]].qflg=1;
    if ((fit->rng[index[n]].v<20) && 
       (fit->rng[index[n]].w_l<20)) fit->rng[index[n]].gsct=1;
  }
  free(index);
  return ftell(fp)-sptr;
} 
  
int SmrFread(FILE *fp,struct RadarParm *prm,struct FitData *fit,int beam) {
  int c;
  int sptr; 
  int blen;
   
  int year,mo,dy,hr,mt,sc;
  int rec_time; 
  int ngood;
  int atten,frang,rsep;
  int tfreq;
  int noise; 
  int bmnum;
  int nrang;
  int st_id=0;
  int cpid=0; 
  int channel=0;
  int status;
  char line[1024];
  char *tok;

  /* skip beginning of line */
  sptr=ftell(fp);
  while((c=fgetc(fp)) != '=') if (c==EOF) return -1;

  /* read the line and then decode */

  fgets(line,1024,fp);
  tok=strtok(line," ");
  if (tok==NULL) return -1;
  year=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  rec_time=atoi(tok);
   
  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  st_id=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  bmnum=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  tfreq=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  noise=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  ngood=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  atten=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  nrang=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  frang=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  rsep=atoi(tok);


  tok=strtok(NULL," ");
  if (tok !=NULL) cpid=atoi(tok);
  if ((tok !=NULL) && ((tok=strtok(NULL," ")) !=NULL)) channel=atoi(tok);
   
  TimeYrsecToYMDHMS(rec_time,year,&mo,&dy,&hr,&mt,&sc);
  if (bmnum==beam) prm->scan=1;
  else prm->scan=0;
  prm->nrang=nrang;
  prm->stid=st_id;
  prm->atten=atten/10;
  prm->frang=frang;
  prm->rsep=rsep;     
  prm->bmnum=bmnum;
  prm->tfreq=tfreq;
  prm->noise.search=noise;
  prm->time.yr=year;
  prm->time.mo=mo;
  prm->time.dy=dy;
  prm->time.hr=hr;
  prm->time.mt=mt;
  prm->time.sc=sc;
  prm->time.us=0;
  prm->cp=cpid;
  prm->channel=channel;
  blen=ftell(fp)-sptr;
  status=SmrFreadData(fp,ngood,prm,fit);
  if (status ==-1) return -1;
  return blen+status;
}  




