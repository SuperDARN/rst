/* ConvertWriteFloat.c
   ===================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "rconvert.h"

#define FNAME "test.dat"

int main(int argc,char *argv[]) {
  int i;
  float val[8]={0.1,0.2,0.5,-0.5,0.6,1.2,1.8,-0.8};
  int fd;
  fd=open(FNAME,O_WRONLY | O_CREAT);
   
  for (i=0;i<8;i++) ConvertWriteFloat(fd,val[i]);
  close(fd);
  
  return 0;
}
   

