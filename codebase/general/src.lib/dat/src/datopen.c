/* 
 Copyright (c) 2021 University of Saskatchewan
 Author: Marina Schmidt
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 Copied code from raw_close.c in cmpraw modified for dat files


 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 Modifications:
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include <errno.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "lmt.h"
#include "dmap.h"
#include "dat.h"
#include "datread.h"
#include "dat_versions.h"

/* DatOpen
 * opens the file pointer, allocates dat structure memory, and
 * sets the datread to the function DatReadCurrent.
 * Params:
 *  char * datfile - datfile name
 *  char * inxfile - indexfile name, not required
 *  returns:
 *      Datfp - dat file pointer
 *      If NULL then an error occured. 
 */
struct Datfp *DatOpen(char *datfile, char *inxfile) {

  int DatParms_pat[]={1, 2, 2, 17, 4, 2, 2, 14, 4, 4, 2, 4, 0, 0};

  unsigned char *inbuf=NULL;
  int16 num_byte;
  int32 rec_num;
  int stat;
  struct DatParms *prm;
  struct Datfp *ptr=NULL;

  inbuf=malloc(sizeof(struct DatData));
  if (inbuf==NULL) {
    fprintf(stderr, "Error: Malloc returned %d on allocating memory for inbuf, %s\n", errno, strerror(errno));
    return NULL;
  }

  ptr=malloc(sizeof(struct Datfp));
  
  if (ptr==NULL) {
      free(inbuf);
      fprintf(stderr, "Error: Malloc returned %d on allocating memory for inbuf, %s\n", errno, strerror(errno));
      return NULL;
  }

  ptr->datfp=open(datfile,O_RDONLY);
  ptr->stime=-1;
  ptr->ctime=-1;
  ptr->frec=0;
  ptr->rlen=0;
  ptr->ptr=0;
  if (ptr->datfp==-1) {
    free(ptr);
    free(inbuf);
    return NULL;
  }
  fstat(ptr->datfp,&ptr->rstat);

  if (ConvertReadShort(ptr->datfp,&num_byte) !=0 || num_byte <= 0) {
    if (num_byte < 0){
        fprintf(stderr,"WARNING : DatOpen : *DatOpen : num_byte < 0 in record header, potentially corrupted file.\n");
    }
    close(ptr->datfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->datfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->datfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }
  ConvertToInt(inbuf,&rec_num);

  ptr->frec=num_byte+2;
  ptr->rlen=num_byte+2;
  ptr->ptr=num_byte+2;

  if (rec_num !=0) { /* not the header so rewind the file */
    lseek(ptr->datfp,0L,SEEK_SET);
    ptr->rlen=0;
  }

  /* read the first record so that we can determine the start time of
     the file */
 if (ConvertReadShort(ptr->datfp,&num_byte) !=0 || num_byte <= 0) {
    close(ptr->datfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->datfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->datfp);
    free(ptr);
    free(inbuf);
    fprintf(stderr, "Error: read returned %d while readig dat file, %s\n", errno, strerror(errno));
    return NULL;
  }

  ConvertToInt(inbuf,&rec_num);

  /* now decode the parameter block */
  ConvertBlock(inbuf+12,DatParms_pat);
  prm=(struct DatParms *) (inbuf+12);

  ptr->stime=TimeYMDHMSToEpoch(prm->YEAR,prm->MONTH,prm->DAY,
	  prm->HOUR,prm->MINUT,prm->SEC);

  /* rewind to the first record */
  lseek(ptr->datfp,ptr->frec,SEEK_SET);

  ptr->datread=DatReadCurrent;

  free(inbuf);
  return ptr;
}

