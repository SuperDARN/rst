/* fitread.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitread.h"




int OldFitRead(struct OldFitFp *ptr,struct RadarParm *prm,
               struct FitData *fit) {
  if (ptr==NULL) return -1;
  return (ptr->fitread)(ptr,prm,fit);
}





