/* raw_open.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "lmt.h"
#include "raw.h"
#include "raw_read.h"
#include "raw_versions.h"



struct rawfp *raw_open(char *rawfile,char *inxfile) {

  int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};

  unsigned char *inbuf=NULL;
  int16 num_byte;
  int32 rec_num;
  int stat;
  struct radar_parms *prm;
  struct rawfp *ptr=NULL;

  inbuf=malloc(sizeof(struct rawdata));
  if (inbuf==NULL) return NULL;

  ptr=malloc(sizeof(struct rawfp));
  
  if (ptr==NULL) {
    free(inbuf);
    return NULL;
  }

  ptr->rawfp=open(rawfile,O_RDONLY);
  ptr->stime=-1;
  ptr->ctime=-1;
  ptr->frec=0;
  ptr->rlen=0;
  ptr->ptr=0;
  if (ptr->rawfp==-1) {
    free(ptr);
    free(inbuf);
    return NULL;
  }
  fstat(ptr->rawfp,&ptr->rstat);

  if (ConvertReadShort(ptr->rawfp,&num_byte) !=0 || num_byte <= 0) {
    if (num_byte < 0){
        fprintf(stderr,"WARNING : raw_open : *raw_open : num_byte < 0 in record header, potentially corrupted file.\n");
    }
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->rawfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }
  ConvertToInt(inbuf,&rec_num);

  ptr->frec=num_byte+2;
  ptr->rlen=num_byte+2;
  ptr->ptr=num_byte+2;

  if (rec_num !=0) { /* not the header so rewind the file */
    lseek(ptr->rawfp,0L,SEEK_SET);
    ptr->rlen=0;
  }

  /* read the first record so that we can determine the start time of
     the file */


 if (ConvertReadShort(ptr->rawfp,&num_byte) !=0 || num_byte <= 0) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->rawfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  ConvertToInt(inbuf,&rec_num);

  /* now decode the parameter block */

  ConvertBlock(inbuf+12,radar_parms_pat);
  prm=(struct radar_parms *) (inbuf+12);

  ptr->stime=TimeYMDHMSToEpoch(prm->YEAR,prm->MONTH,prm->DAY,
	  prm->HOUR,prm->MINUT,prm->SEC);

  /* rewind to the first record */

  lseek(ptr->rawfp,ptr->frec,SEEK_SET);

  ptr->rawread=raw_read_current;

  free(inbuf);
  return ptr;
}

