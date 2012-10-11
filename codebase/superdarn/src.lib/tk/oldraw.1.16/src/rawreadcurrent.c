/* rawreadcurrent.c
   ================
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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



int32 OldRawDcmpr(unsigned char *word) {
  /* decompress value */
  int count;
  int32 value;
  
  
  count=word[0] & 0x0f;
  value=(word[1] & 0x7f)<<8 | (word[0] & 0xf0); 
  if (count==0) value=value<<1;
  else {
     value=value | 0x8000;
     value=value<<count;
  }
  if ((word[1] & 0x80) !=0) value=-value; 
  return (value);  
}

int OldRawReadCurrent(struct OldRawFp *fp,struct RadarParm *prm,
                      struct RawData *raw) { 

   /* read raw data block from file */


   int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
   int i;
   int16 range;
   int j;
   int16 prev_range;
   int xcf_data;
   unsigned int stat;
   int16 num_byte;
   int32 rec_num=0;
   unsigned char *inbuf;
   unsigned char *inbuf_ptr;
   struct rawdata oldraw;
   struct rawdata *rptr;
   int c,d;

   float acfd[2*ORIG_MAX_RANGE*ORIG_LAG_TAB_LEN];
   float xcfd[2*ORIG_MAX_RANGE*ORIG_LAG_TAB_LEN];
   int16 lag[2*ORIG_LAG_TAB_LEN]; 

   xcf_data = 0;
   prev_range = -1;


 
   rptr=&oldraw;
   inbuf=malloc(sizeof(struct rawdata));

   /* zero out the raw data buffer */
   memset(rptr,0,sizeof(struct rawdata));
   fp->rlen=0;
   do {
     if (ConvertReadShort(fp->rawfp,&num_byte) !=0) {
       free(inbuf);
       return -1;
     }
     fp->rlen+=num_byte;
     fp->ptr+=num_byte;
     num_byte = num_byte - 2;
     stat = read(fp->rawfp,inbuf,num_byte);
     if(stat != num_byte) {
       free(inbuf);
       return -1; 
     }
     inbuf_ptr=inbuf;
     ConvertToInt(inbuf_ptr,&rec_num);
   } while (rec_num==0); 
   inbuf_ptr = inbuf_ptr + 12;  /* skip rec_num + rawwrite */  
   num_byte = num_byte - 12;
  
   /* zero out the raw data buffer */

   /* copy radar_parms */   
   ConvertBlock(inbuf_ptr,radar_parms_pat);
   memcpy((void *) &(rptr->PARMS),inbuf_ptr,sizeof(struct radar_parms)); 
   inbuf_ptr = inbuf_ptr + sizeof(struct radar_parms);
   num_byte = num_byte - sizeof(struct radar_parms);

   /* copy the pulse pattern */
  
   for (i=0;i<rptr->PARMS.MPPUL;i++) {
      ConvertToShort(inbuf_ptr,&rptr->PULSE_PATTERN[i]);
      inbuf_ptr+=sizeof(int16);
      num_byte-=sizeof(int16);
   }

   /* copy the lag table */

    for(j=0;j < 2; ++j)
      for(i=0; i < rptr->PARMS.MPLGS; ++i) {
         ConvertToShort(inbuf_ptr,&rptr->LAG_TABLE[j][i]);
         inbuf_ptr = inbuf_ptr + sizeof(int16);
         num_byte = num_byte - sizeof(int16);
      } 
	  

   /* copy comment buffer */
   memcpy(rptr->COMBF,inbuf_ptr,ORIG_COMBF_SIZE);
   inbuf_ptr = inbuf_ptr + ORIG_COMBF_SIZE;
   num_byte = num_byte - ORIG_COMBF_SIZE;

   /* decompress and copy the lag-0 powers */
   for(i=0 ; i < rptr->PARMS.NRANG ; ++i) {
      rptr->pwr0[i] = OldRawDcmpr(inbuf_ptr);
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);
   }

   /* decompress and copy acfs */
   while ( num_byte > 0 ) {
      ConvertToShort(inbuf_ptr,&range);
      --range;
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);

      if((range <= prev_range) && (rptr->PARMS.XCF))
		   xcf_data = 1;

      for(i = 0; i < rptr->PARMS.MPLGS ; ++i) {   
         for(j=0 ; j < 2; ++j) {
	    if (xcf_data) rptr->xcfd[range][i][j] = OldRawDcmpr(inbuf_ptr);
	    else rptr->acfd[range][i][j] = OldRawDcmpr(inbuf_ptr);
            inbuf_ptr = inbuf_ptr + sizeof(int16);
            num_byte = num_byte - sizeof(int16);
         }
      }
		
      prev_range = range;

   } 
  fp->ctime=TimeYMDHMSToEpoch(rptr->PARMS.YEAR,
			rptr->PARMS.MONTH,
			rptr->PARMS.DAY,
			rptr->PARMS.HOUR,
			rptr->PARMS.MINUT,
			rptr->PARMS.SEC);
  free(inbuf);  


  prm->revision.major=oldraw.PARMS.REV.MAJOR;
  prm->revision.minor=oldraw.PARMS.REV.MINOR;
  prm->origin.code=-1;
  prm->origin.time=NULL;
  prm->origin.command=NULL;
  prm->cp=oldraw.PARMS.CP;
  prm->stid=oldraw.PARMS.ST_ID;
  prm->time.yr=oldraw.PARMS.YEAR;
  prm->time.mo=oldraw.PARMS.MONTH;
  prm->time.dy=oldraw.PARMS.DAY;
  prm->time.hr=oldraw.PARMS.HOUR;
  prm->time.mt=oldraw.PARMS.MINUT;
  prm->time.sc=oldraw.PARMS.SEC;
  prm->time.us=0;

  prm->txpow=oldraw.PARMS.TXPOW;
  prm->nave=oldraw.PARMS.NAVE;
  prm->atten=oldraw.PARMS.ATTEN;
  prm->lagfr=oldraw.PARMS.LAGFR;
  prm->smsep=oldraw.PARMS.SMSEP;
  prm->ercod=oldraw.PARMS.ERCOD;
  prm->stat.agc=oldraw.PARMS.AGC_STAT;
  prm->stat.lopwr=oldraw.PARMS.LOPWR_STAT;
  prm->noise.search=oldraw.PARMS.NOISE;
  prm->noise.mean=oldraw.PARMS.NOISE_MEAN;
      
  prm->channel=oldraw.PARMS.CHANNEL;
  prm->bmnum=oldraw.PARMS.BMNUM;
  prm->bmazm=-1;
  prm->scan=oldraw.PARMS.SCAN;
   
  prm->offset=oldraw.PARMS.usr_resL1; 
  prm->rxrise=oldraw.PARMS.RXRISE;
  prm->intt.sc=oldraw.PARMS.INTT;
  prm->intt.us=0;
  prm->txpl=oldraw.PARMS.TXPL;
    
  prm->mpinc=oldraw.PARMS.MPINC;
  prm->mppul=oldraw.PARMS.MPPUL;
  prm->mplgs=oldraw.PARMS.MPLGS;
  prm->nrang=oldraw.PARMS.NRANG;
  prm->frang=oldraw.PARMS.FRANG;
  prm->rsep=oldraw.PARMS.RSEP;
  prm->xcf=oldraw.PARMS.XCF; 
  prm->tfreq=oldraw.PARMS.TFREQ;
  prm->mxpwr=oldraw.PARMS.MXPWR;
  prm->lvmax=oldraw.PARMS.LVMAX;   

  for (c=0;c<=prm->mplgs;c++) {
    lag[c*2]=oldraw.LAG_TABLE[0][c];
    lag[c*2+1]=oldraw.LAG_TABLE[1][c];
  }  

  RadarParmSetPulse(prm,prm->mppul,oldraw.PULSE_PATTERN);
  RadarParmSetLag(prm,prm->mplgs,lag);
    
  RadarParmSetCombf(prm,oldraw.COMBF);

  raw->revision.major=fp->major_rev;
  raw->revision.minor=fp->minor_rev;

  for (c=0;c<prm->nrang;c++) {
    for (d=0;d<prm->mplgs;d++) {
      acfd[2*(c*prm->mplgs+d)]=oldraw.acfd[c][d][0];     
      acfd[2*(c*prm->mplgs+d)+1]=oldraw.acfd[c][d][1];
      xcfd[2*(c*prm->mplgs+d)]=oldraw.xcfd[c][d][0];     
      xcfd[2*(c*prm->mplgs+d)+1]=oldraw.xcfd[c][d][1];
    }
  }

  RawSetPwr(raw,prm->nrang,oldraw.pwr0,0,NULL);
  RawSetACF(raw,prm->nrang,prm->mplgs,acfd,0,NULL);
  if (prm->xcf !=0) RawSetXCF(raw,prm->nrang,prm->mplgs,xcfd,0,NULL);

  return 0;
}

