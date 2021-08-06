/* fit_open.c
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

/* opens a fit file */

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
#include "lmt.h"
#include "raw.h"
#include "fit.h"

#include "fit_read.h"
#include "fit_read_inx.h"
#include "fit_str.h"
#include "fit_versions.h"




struct fitfp *fit_open(char *fitfile,char *inxfile) {

  
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,PULSE_PAT_LEN,2,2*LAG_TAB_LEN,1,ORIG_COMBF_SIZE,4,3,
		2,2*ORIG_MAX_RANGE,1,ORIG_MAX_RANGE,0,0};
  int status=0;
  union fit_out r;
  struct fitfp *ptr=NULL;
  int16 tmp,i,j;
  int32 inx_buf[4];
  struct radar_parms *prms=NULL;
  char *tmpbuf;
  ptr=malloc(sizeof(struct fitfp));
  if (ptr==NULL) return NULL;
 
  ptr->fitfp=-1;
  ptr->inxfp=-1;
  ptr->ibuf=NULL;
  ptr->fbuf=NULL;
  ptr->iptr=0;
  ptr->fptr=0;
  ptr->ctime=-1;
  ptr->stime=-1;
  ptr->etime=-1;
  ptr->fitfp=open(fitfile,O_RDONLY);
  if (ptr->fitfp==-1) {
    free(ptr);
    return NULL;
  }

  fstat(ptr->fitfp,&ptr->fstat);
  

  /* buffering disabled as it is actually slower to load into memory!  
  ptr->fbuf=malloc(ptr->fstat.st_size);
  if (ptr->fbuf !=NULL) {
    if (read(ptr->fitfp,ptr->fbuf,
             ptr->fstat.st_size) !=ptr->fstat.st_size) {
       close(ptr->fitfp);
       free(ptr->fbuf);
       free(ptr);
       return NULL;
    }
    close(ptr->fitfp);
    ptr->fitfp=-1;
  }
  */
  
  if (inxfile !=NULL) { /* open the index file */
    ptr->inxfp=open(inxfile,O_RDONLY);
    if (ptr->inxfp !=-1) { 
      fstat(ptr->inxfp,&ptr->istat);
      ptr->ibuf=malloc(ptr->istat.st_size);
      if ((ptr->ibuf !=NULL) &&
          (read(ptr->inxfp,ptr->ibuf,ptr->istat.st_size) 
           !=ptr->istat.st_size)) free(ptr->ibuf);
       close(ptr->inxfp);
       ptr->inxfp=-1;
    }
  } 

  if (ptr->fitfp !=-1) {
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
  } else {
    ConvertToShort(ptr->fbuf,&tmp);
    ptr->fit_recl=tmp;
    ConvertToShort(ptr->fbuf+2,&tmp);
    ptr->inx_recl=tmp;
    ptr->fptr=ptr->fit_recl;
    memcpy( (unsigned char *) &r,
            ptr->fbuf+ptr->fit_recl,sizeof(union fit_out));
    ConvertBlock( (unsigned char *) &r,r1_pat);
    status=(r.r1.rrn !=0);
    prms=(struct radar_parms *) &(r.r1.plist[0]);
  }

  if (status !=0) {
    if (ptr->fitfp !=-1) close(ptr->fitfp);
    if (ptr->fbuf !=NULL) free(ptr->fbuf);
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    if (ptr->ibuf !=NULL) free(ptr->ibuf);
    free(ptr);
    return NULL; 
  }

  ptr->ctime=TimeYMDHMSToEpoch(prms->YEAR,prms->MONTH,prms->DAY,
			prms->HOUR,prms->MINUT,prms->SEC);

  ptr->stime=ptr->ctime;  
  


  if (fit_read_inx(ptr,inx_buf,-1)==0) {

    if (inx_buf[0] > inx_buf[1]) prms->YEAR++;
    /* generate the end time of the file */
 
    ptr->etime=TimeYMDHMSToEpoch(prms->YEAR,1,1,0,0,0)
                +inx_buf[1];   
    ptr->inx_srec=inx_buf[2]-2;
    ptr->inx_erec=inx_buf[3];     
  } else { /* corrupt index file */
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    if (ptr->ibuf !=NULL) free(ptr->ibuf);
    ptr->ibuf=NULL;
    ptr->inxfp=-1;
  } 
  ptr->fitread=fit_read_current;
  return ptr;        
}




