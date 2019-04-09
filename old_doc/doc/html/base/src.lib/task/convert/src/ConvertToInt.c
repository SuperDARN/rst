/* ConvertToInt.c
   ==============
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {

  int i;
  int32 val;
  unsigned char buf[4]={0xff,0xff,0xe0,0x3f};
   
  ConvertToInt(buf,&val);
  fprintf(stdout,"buf=");
  for (i=0;i<4;i++) fprintf(stdout,"%.2x",buf[i]);
  fprintf(stdout,"\n");
  fprintf(stdout,"val=%d\n",val);
  return 0;
}
   

