/* ConvertFwriteLong.c
   ==================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

#define FNAME "test.dat"

int main(int argc,char *argv[]) {
  int i;
  int64 val[8]={2,4,4,-5,6,3,10,-8};
  FILE *fp;  
  fp=fopen(FNAME,"w");
   
  for (i=0;i<8;i++) ConvertFwriteLong(fp,val[i]);
  fclose(fp);
  
  return 0;
}
   

