/* RCDFMaxRecR.c
   =============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <cdf.h>
#include "rcdf.h"


int main(int argc,char *argv[]) {

  CDFid id;
  CDFstatus status;
  int max;

  fprintf(stdout,"RCDFMaxRecR\n");
  if (argc<3) {
    fprintf(stderr,"Filename and variable must be given.\n");
    exit(-1);
  } 

  status=CDFopen(argv[1],&id);
  if (status !=CDF_OK) {
     fprintf(stderr,"Could not open cdf file.\n");
     exit(-1);
  }

  max=RCDFMaxRecR(id,argv[2]);

  CDFclose(id);
  
  fprintf(stdout,"Max. R Rec for %s:%d\n",argv[2],max);

  return 0;
}
