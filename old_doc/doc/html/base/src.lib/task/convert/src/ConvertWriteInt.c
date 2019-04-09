/* ConvertWriteInt.c
   =================
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
  int32 val[8]={1,2,5,-5,6,2,18,-8};
  int fd;
  fd=open(FNAME,O_WRONLY | O_CREAT);
   
  for (i=0;i<8;i++) ConvertWriteInt(fd,val[i]);
  close(fd);
  
  return 0;
}
   

