/* cfitopen.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "cfitdata.h"
#include "cfitread.h"





struct CFitfp *CFitOpen(char *name) {
  struct CFitfp *fptr=NULL;

  fptr=malloc(sizeof(struct CFitfp));
  fptr->fp=gzdopen(open(name,O_RDONLY),"r");
  if (fptr->fp==NULL) {
    free(fptr);
    return NULL;
  }
  /* 
  fstat(fptr->fp,&cstat);
  fptr->fsze=cstat.st_size;
  */

  fptr->ctime=-1;
  fptr->blen=0;
  fptr->fptr=0;
  fptr->fsze=0;
  fptr->fbuf=NULL;

  /*  
  fptr->fbuf=malloc(fptr->fsze);
  
  if (fptr->fbuf !=NULL) {
    if (gzread(fptr->fp,fptr->fbuf,fptr->fsze) !=fptr->fsze) {
      gzclose(fptr->fp);  
      free(fptr->fbuf);
      free(fptr);
      return NULL;
    }
    gzclose(fptr->fp);
    fptr->fp=-1;
  }
  */

  return fptr;
} 



