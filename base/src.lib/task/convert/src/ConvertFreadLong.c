/* ConvertFreadLong.c
   ==================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

#define FNAME "test.dat"

int main(int argc,char *argv[]) {
  int64 val;
  FILE *fp;  
  fp=fopen(FNAME,"r");
   
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  
  while (ConvertFreadLong(fp,&val)==0) {
    fprintf(stdout,"%lld\n",val);
  }   
  fclose(fp);
  
  return 0;
}
   


