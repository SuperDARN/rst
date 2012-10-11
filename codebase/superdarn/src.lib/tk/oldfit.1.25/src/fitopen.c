/* fitopen.c
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
#include "fitdata.h"

#include "oldfitread.h"

#include "fitversions.h"
#include "fitstr.h"



struct OldFitFp *OldFitOpenFd(int fitfp,int inxfp) {

  
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,ORIG_PULSE_PAT_LEN,2,2*ORIG_LAG_TAB_LEN,1,
                ORIG_COMBF_SIZE,4,3,
		2,2*ORIG_MAX_RANGE,1,ORIG_MAX_RANGE,0,0};
  int status=0;
  union fit_out r;
  struct OldFitFp *ptr=NULL;
  int16 tmp,i,j;
  int32 inx_buf[4];
  struct radar_parms *prms=NULL;
  char *tmpbuf;
  ptr=malloc(sizeof(struct OldFitFp));
  if (ptr==NULL) return NULL;
 
  ptr->fitfp=-1;
  ptr->inxfp=-1;
  ptr->ctime=-1;
  ptr->stime=-1;
  ptr->etime=-1;
  ptr->fitfp=fitfp;
  ptr->inxfp=inxfp;
 
  fstat(ptr->fitfp,&ptr->fstat);
  fstat(ptr->inxfp,&ptr->istat);

 
  
  ConvertReadShort(ptr->fitfp,&tmp);
  ptr->fit_recl=tmp;
  ConvertReadShort(ptr->fitfp,&tmp);
  ptr->inx_recl=tmp;
  
  tmpbuf=malloc(ptr->fit_recl);
  if (tmpbuf !=NULL) { /* get the header information */
     memset(tmpbuf,0,ptr->fit_recl);
     status=(read(ptr->fitfp,tmpbuf,ptr->fit_recl) !=ptr->fit_recl);
     /* now decode the header information */
     if (status==0) {
        for (i=0;(tmpbuf[i] !='\n') && (tmpbuf[i] !=0) && (i<80);i++) 
          ptr->header[i]=tmpbuf[i];
        ptr->header[i]=0;
        j=i+1;
        for (i=0;(tmpbuf[j+i] !='\n') && (tmpbuf[j+i] !=0) && (i<32);i++) 
          ptr->date[i]=tmpbuf[j+i];
        ptr->date[i]=0;
        j+=i+1;
        for (i=0;(tmpbuf[j+i] !=0) && (i<256);i++) 
          ptr->extra[i]=tmpbuf[j+i];
        ptr->extra[i]=0;
         
        /* okay extra the version information - look for the '.' */
        for (i=0;(ptr->header[i] !='.') && (ptr->header[i] !=0);i++);
        if (ptr->header[i] !=0) { 
           tmpbuf[i]=0; /* work backwards for the major number */
           for (j=0;(ptr->header[i-1-j] !=' ') && ((i-1-j)>0);j++)
             tmpbuf[i-1-j]=ptr->header[i-1-j];
           ptr->major_rev=atoi(tmpbuf+i-j);
           for (j=0;(ptr->header[i+1+j] !=' ') && 
               (ptr->header[i+1+j] !=0);i++) tmpbuf[j]=ptr->header[i+1+j]; 
           tmpbuf[j]=0;
           ptr->minor_rev=atoi(tmpbuf);
           
        }
     }
     free(tmpbuf);
  }
    
  lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);

  /* read the first parameter block se we can determine the year */
  status=(read(ptr->fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out));
  if (status==0) {
    ConvertBlock( (unsigned char *) &r,r1_pat);
    status=(r.r1.rrn !=0);
    prms=(struct radar_parms *) &(r.r1.plist[0]);
  }
  lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);

  if (status !=0) {
    if (ptr->fitfp !=-1) close(ptr->fitfp);
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    free(ptr);
    return NULL; 
  }

  ptr->ctime=TimeYMDHMSToEpoch(prms->YEAR,prms->MONTH,prms->DAY,
			prms->HOUR,prms->MINUT,prms->SEC);

  ptr->stime=ptr->ctime;  
  


  if (OldFitReadInx(ptr,inx_buf,-1)==0) {

    if (inx_buf[0] > inx_buf[1]) prms->YEAR++;
    /* generate the end time of the file */
 
    ptr->etime=TimeYMDHMSToEpoch(prms->YEAR,1,1,0,0,0)
                +inx_buf[1];   
    ptr->inx_srec=inx_buf[2]-2;
    ptr->inx_erec=inx_buf[3];     
  } else { /* corrupt index file */
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    ptr->inxfp=-1;
  } 
  ptr->fitread=OldFitReadCurrent;
  return ptr;        
}

struct OldFitFp *OldFitOpen(char *fitfile,char *inxfile) {

  int fitfp=-1,inxfp=-1;
  fitfp=open(fitfile,O_RDONLY);
  if (fitfp==-1) return NULL;
  if (inxfile !=NULL) inxfp=open(inxfile,O_RDONLY);
  return OldFitOpenFd(fitfp,inxfp);
}
