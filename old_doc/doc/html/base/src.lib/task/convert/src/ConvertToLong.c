/* ConvertToLong.c
   ===============
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {

  int i;
  int64 val;
  unsigned char buf[8]={0xff,0xff,0xc0,0x2f,0x1f,0x7f,0xe0,0x3f};
   
  ConvertToLong(buf,&val);
  fprintf(stdout,"buf=");
  for (i=0;i<8;i++) fprintf(stdout,"%.2x",buf[i]);
  fprintf(stdout,"\n");
  fprintf(stdout,"val=%lld\n",val);
  return 0;
}
   

