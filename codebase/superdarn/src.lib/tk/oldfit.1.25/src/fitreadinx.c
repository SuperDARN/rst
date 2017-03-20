/* fitreadinx.c
   ============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



/* This routine reads in and decodes a record from a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
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




int OldFitReadInx(struct OldFitFp *ptr,int32 *buffer,int recno) {
  int i,status=0;
  if (ptr->inxfp !=-1) {
     if (recno !=-1)  /* jump to this record */
       lseek(ptr->inxfp,(recno+1)*ptr->inx_recl,SEEK_SET);
     for (i=0;(i<4) && (status==0);i++) 
     status=ConvertReadInt(ptr->inxfp,&buffer[i]);
     return status;
  }
  return -1;
}


