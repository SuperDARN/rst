/* smrwrite.c
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"




int SmrHeaderFwrite(FILE * fp,char *version,char *text,char *rname) {
  time_t ctime;
  int status;

  /* first, clear the header buffer */

  fprintf(fp, "sd_summary %s\n",version);
  fprintf(fp, "%s\n",text);

  ctime = time((time_t *) 0);
  status = fprintf(fp, "%s %s\n", rname, asctime(gmtime(&ctime)));
  if (status==-1) return -1;
  return 0;
}


int SmrFwrite(FILE *fp,struct RadarParm *prm,struct FitData *fit,int pwr) {
  int rec_time;
  int status;
  int i,n,ngood=0;
  int *rng;

  rng=malloc(sizeof(int)*prm->nrang);
  for (i=0; i < prm->nrang; ++i) 
    if ((fit->rng[i].qflg == 1) && (fit->rng[i].p_0 >= pwr)) {
      rng[i]=1;
      ++ngood;
     } else rng[i] = 0;
		
  
	
  /* now print the time stamp and header info */
			
  rec_time = TimeYMDHMSToYrsec(prm->time.yr,prm->time.mo,prm->time.dy,
                               prm->time.hr,prm->time.mt,prm->time.sc);
  status = fprintf(fp,
     "time = %4hd %d %2hd %2hd %6hd %d %hd %2hd %3hd %4hd %3hd %3hd %3hd\n",
				  prm->time.yr,
				  rec_time,
				  prm->stid,
				  prm->bmnum,
				  prm->tfreq,
				  (int) prm->noise.search,
				  ngood,
				  prm->atten*10,
				  prm->nrang,
				  prm->frang,
				  prm->rsep,prm->cp,
                                  prm->channel);
  if (status <0) {
    free(rng);
    return -1;
  }
 			
   /* now put out the list of good ranges */
   n = 0;
   for (i = 0; i < prm->nrang; ++i) if (rng[i] == 1) {
    
     status = fprintf(fp, "%4d", i);
     if (status < 0 ){
        free(rng);
        return -1;
     }		
	 ++n;
     if (n >= 25) {
	   fprintf(fp, "\n");
	   n = 0;
	 }
   }
   if (n != 0) fprintf(fp, "\n");
		
   for (i=0; i < prm->nrang; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%8.1f", fit->rng[i].p_l);
    if (status<0) { 
      free(rng);
      return -1;
    }
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");

  for (i=0; i < prm->nrang; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%8.1f", fit->rng[i].v);
    if (status<0) {
      free(rng);
      return -1;
    }
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");

  for (i=0; i < prm->nrang; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%7.0f", fit->rng[i].w_l);
    if (status<0) {
      free(rng);
     return -1;
    }
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");
  free(rng);
  return 0;
}

 
