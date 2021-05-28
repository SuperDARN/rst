/* raw_read_current.c
   ==================
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
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "lmt.h"
#include "raw.h"
#include "raw_read.h"



int32 dcmpr(unsigned char *word) {
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

int raw_read_current(struct rawfp *fp,struct rawdata *raw_data) {

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

   xcf_data = 0;
   prev_range = -1;

   inbuf=malloc(sizeof(struct rawdata));

   /* zero out the raw data buffer */
   memset(raw_data,0,sizeof(struct rawdata));
   fp->rlen=0;
   do {
     if (ConvertReadShort(fp->rawfp,&num_byte) !=0 || num_byte <= 0) {
       if (num_byte < 0){
           fprintf(stderr,"WARNING : raw_read_current : raw_read_current : num_byte < 0 in record header, potentially corrupted file.\n");
       }
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
   memcpy((void *) &(raw_data->PARMS),inbuf_ptr,sizeof(struct radar_parms));
   inbuf_ptr = inbuf_ptr + sizeof(struct radar_parms);
   num_byte = num_byte - sizeof(struct radar_parms);

   /* copy the pulse pattern */

   for (i=0;i<raw_data->PARMS.MPPUL;i++) {
      ConvertToShort(inbuf_ptr,&raw_data->PULSE_PATTERN[i]);
      inbuf_ptr+=sizeof(int16);
      num_byte-=sizeof(int16);
   }

   /* copy the lag table */

    for(j=0;j < 2; ++j)
      for(i=0; i < raw_data->PARMS.MPLGS; ++i) {
         ConvertToShort(inbuf_ptr,&raw_data->LAG_TABLE[j][i]);
         inbuf_ptr = inbuf_ptr + sizeof(int16);
         num_byte = num_byte - sizeof(int16);
      }

   /* copy comment buffer */
   memcpy(raw_data->COMBF,inbuf_ptr,ORIG_COMBF_SIZE);
   inbuf_ptr = inbuf_ptr + ORIG_COMBF_SIZE;
   num_byte = num_byte - ORIG_COMBF_SIZE;

   /* decompress and copy the lag-0 powers */
   for(i=0 ; i < raw_data->PARMS.NRANG ; ++i) {
      raw_data->pwr0[i] = dcmpr(inbuf_ptr);
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);
   }

   /* decompress and copy acfs */
   while ( num_byte > 0 ) {
      ConvertToShort(inbuf_ptr,&range);
      --range;
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);

      if((range <= prev_range) && (raw_data->PARMS.XCF))
		   xcf_data = 1;

      for(i = 0; i < raw_data->PARMS.MPLGS ; ++i) {
         for(j=0 ; j < 2; ++j) {
	    if (xcf_data) raw_data->xcfd[range][i][j] = dcmpr(inbuf_ptr);
	    else raw_data->acfd[range][i][j] = dcmpr(inbuf_ptr);
            inbuf_ptr = inbuf_ptr + sizeof(int16);
            num_byte = num_byte - sizeof(int16);
         }
      }

      prev_range = range;

   }
  fp->ctime=TimeYMDHMSToEpoch(raw_data->PARMS.YEAR,
			raw_data->PARMS.MONTH,
			raw_data->PARMS.DAY,
			raw_data->PARMS.HOUR,
			raw_data->PARMS.MINUT,
			raw_data->PARMS.SEC);
  free(inbuf);
  return 0;
}

