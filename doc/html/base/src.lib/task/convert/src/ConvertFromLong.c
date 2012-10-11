/* ConvertFromLong.c
   =================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {
  int i;
  int64 val;
  unsigned char buf[8];
  
  val=1023;
  ConvertFromLong(val,buf);
  
  fprintf(stdout,"val=%lld\n",val);
  fprintf(stdout,"buf=");
  for (i=0;i<8;i++) fprintf(stdout,"%.2x",buf[i]);
  fprintf(stdout,"\n");
  return 0;
}
   

