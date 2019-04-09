/* ConvertBlock.c
   ==============
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {
  int i;
  
  
  struct {
    double x,y,z;
    int16 flg;
    int32 data[4];
    float angle;
  } dat;
  
  int pattern[]={8,3,2,1,4,4,4,1,0,0};
  
  unsigned char *str;
  
  dat.x=0.5;
  dat.y=0.2;
  dat.z=0.3;
  dat.flg=1023;
  dat.data[0]=18000;
  dat.data[1]=32000;
  dat.data[2]=64000;
  dat.data[3]=12000;
  dat.angle=45.5;
   
  str=(unsigned char *) &dat;
 
 
  ConvertBlock(str,pattern);
  fprintf(stdout,"buf=");
  for (i=0;i<sizeof(dat);i++) fprintf(stdout,"%.2x",str[i]);
  fprintf(stdout,"\n");
  
  return 0;
}
   









