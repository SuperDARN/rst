/* OptionScanFileArg.c
   ===================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "option.h"

int main(int argc,char *argv[]) {
  FILE *fp;
  char *text=NULL;

  fp=fopen(argv[argc-1],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  while ((text=OptionScanFileArg(fp)) !=NULL) {
    fprintf(stdout,"%s\n",text);
    free(text);
  }
  fclose(fp); 
  return 0;
}
   


