/* RCDFMajority.c
   ==============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <cdf.h>
#include "rcdf.h"


int main(int argc,char *argv[]) {

  CDFid id;
  CDFstatus status;
  int majority;

  fprintf(stdout,"RCDFMajority\n");
  if (argc==1) {
    fprintf(stderr,"No filename.\n");
    exit(-1);
  } 

  status=CDFopen(argv[1],&id);
  if (status !=CDF_OK) {
     fprintf(stderr,"Could not open cdf file.\n");
     exit(-1);
  }

  majority=RCDFMajority(id);

  CDFclose(id);
  
  fprintf(stdout,"Majority:%d\n",majority);

  return 0;
}
