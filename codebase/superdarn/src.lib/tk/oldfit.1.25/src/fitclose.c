/* fit_close.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

/* This routine reads in and decodes a record from a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitread.h"



void OldFitClose(struct OldFitFp *ptr) {
  if (ptr==NULL) return;
  if (ptr->fitfp !=-1) close(ptr->fitfp);
  if (ptr->inxfp !=-1) close(ptr->inxfp);
  free(ptr);
}



