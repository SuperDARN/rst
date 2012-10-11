/* ConvertReadShort.c
   ==================
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
  int16 val;
  int fd;  

  fd=open(FNAME,O_RDONLY);
   
  if (fd==-1) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  
  while (ConvertReadShort(fd,&val)==0) {
    fprintf(stdout,"%d\n",val);
  }   
  close(fd);
  
  return 0;
}
   


