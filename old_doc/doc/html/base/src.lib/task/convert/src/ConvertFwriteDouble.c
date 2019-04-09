/* ConvertFwriteDouble.c
   =====================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

#define FNAME "test.dat"

int main(int argc,char *argv[]) {
  int i;
  double val[8]={0.1,0.2,0.5,-0.5,0.6,1.2,1.8,-0.8};
  FILE *fp;  
  fp=fopen(FNAME,"w");
   
  for (i=0;i<8;i++) ConvertFwriteDouble(fp,val[i]);
  fclose(fp);
  
  return 0;
}
   

