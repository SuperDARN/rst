/* rawopen.c
   =========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "lmt.h"
#include "raw.h"
#include "oldrawread.h"
#include "rawversions.h"



struct rawrechdr { /* this is the header for the output record */
    unsigned char size[2];
    unsigned char number[4];
    char stamp[8];
};


struct OldRawFp *OldRawOpenFd(int rawfd,int inxfd) {

  int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};

  unsigned char *inbuf=NULL;
  int16 num_byte;
  int32 rec_num;
  int stat;
  struct radar_parms *prm;
  struct OldRawFp *ptr=NULL;
  int status=0;
  int j;

  inbuf=malloc(sizeof(struct rawdata));
  if (inbuf==NULL) return NULL;

  ptr=malloc(sizeof(struct OldRawFp));
  
  if (ptr==NULL) return NULL;

  ptr->rawfp=rawfd;
  ptr->stime=-1;
  ptr->ctime=-1;
  ptr->frec=0;
  ptr->rlen=0;
  ptr->ptr=0;

  fstat(ptr->rawfp,&ptr->rstat);

  if (ConvertReadShort(ptr->rawfp,&num_byte) !=0) {
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

  if (rec_num==0) { /* decode header to get the threshold */
    char tmp[256];
    char *str;
    strcpy(tmp,(char *) (inbuf+sizeof(struct rawrechdr)-sizeof(int16)+1));
    ptr->thr=3;
    str=strtok(tmp," ");
    if (str==NULL) status=-1;
    if ((status==0) && (strcmp(str,"version") !=0)) status=-1;
    if (status==0) str=strtok(NULL," ");
    if (str==NULL) status=-1;
    for (j=0;str[j] !=0;j++) if (str[j]=='.') break;
    if (str[j]==0) {
      ptr->major_rev=atoi(str);
    } else {
      str[j]=0;
      ptr->major_rev=atoi(str);
      ptr->minor_rev=atoi(str+j+1);
    }
    if (status==0) str=strtok(NULL," ");
    if (str==NULL) status=-1;
    if ((status==0) && (strcmp(str,"threshold") !=0)) status=-1;
    if (status==0) str=strtok(NULL," ");
    if (str==NULL) status=-1;
    if (status==0) ptr->thr=atoi(str);
  }

  if (rec_num !=0) { /* not the header so rewind the file */
    lseek(ptr->rawfp,0L,SEEK_SET);
    ptr->rlen=0;
  }

  /* read the first record so that we can determine the start time of 
     the file */

  
 if (ConvertReadShort(ptr->rawfp,&num_byte) !=0) {
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

  ptr->ctime=ptr->stime;

  /* rewind to the first record */

  lseek(ptr->rawfp,ptr->frec,SEEK_SET);

  ptr->rawread=OldRawReadCurrent;

  free(inbuf);
  return ptr;
}

struct OldRawFp *OldRawOpen(char *rawfile,char *inxfile) {
  int rawfd=0;
  int inxfd=0;

  rawfd=open(rawfile,O_RDONLY);
  if (rawfd==-1) return NULL;
  
  return OldRawOpenFd(rawfd,inxfd);
}
