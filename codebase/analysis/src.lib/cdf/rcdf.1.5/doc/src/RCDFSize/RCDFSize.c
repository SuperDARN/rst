/* RCDFSize.c
   ==========
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <cdf.h>
#include "rcdf.h"


int main(int argc,char *argv[]) {

  fprintf(stdout,"RCDFSize\n");
  
  fprintf(stdout,"CDF_BYTE:%d\n",RCDFSize(CDF_BYTE));
  fprintf(stdout,"CDF_INT2:%d\n",RCDFSize(CDF_INT2));
  fprintf(stdout,"CDF_REAL4:%d\n",RCDFSize(CDF_REAL4));
  fprintf(stdout,"CDF_FLOAT:%d\n",RCDFSize(CDF_FLOAT));


  return 0;
}
