/* 
 Copyright (C) 2021 SuperDARN Canada, University of Saskatchewan
 Author: Marina Schmidt
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 Copied code from raw_close.c in cmpraw modified for dat files

 Modified

Disclaimer:

 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include <fcntl.h>
#include "rtypes.h"
#include "rtime.h"
#include "lmt.h"

#include "dmap.h"
#include "dat.h"
#include "datread.h"


/* DatClose
 * Closes the file pointer 
 */
void DatClose(struct Datfp *fp) {
  if (fp==NULL) return;
  if (fp->datfp !=-1) close(fp->datfp);
}
