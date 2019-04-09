/* ConvertFromFloat.c
   ==================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {
  int i;
  float val;
  unsigned char buf[4];
  
  val=0.3;
  ConvertFromFloat(val,buf);
  
  fprintf(stdout,"val=%g\n",val);
  fprintf(stdout,"buf=");
  for (i=0;i<4;i++) fprintf(stdout,"%.2x",buf[i]);
  fprintf(stdout,"\n");
  return 0;
}
   

