/* ConvertToDouble.c
   ===================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {

   int i;
  double val;
  unsigned char buf[8]={0,0,0,0,0xff,0xff,0xe0,0x3f};
   
  ConvertToDouble(buf,&val);
  fprintf(stdout,"buf=");
  for (i=0;i<8;i++) fprintf(stdout,"%.2x",buf[i]);
  fprintf(stdout,"\n");
  fprintf(stdout,"val=%g\n",val);
  return 0;
}
   

