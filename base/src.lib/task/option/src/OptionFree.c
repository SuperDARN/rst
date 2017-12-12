/* OptionFree.c
   ============
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "option.h"


struct OptionData opt;
int arg=0;

int main(int argc,char *argv[]) {

  unsigned char flag=0;

  OptionAdd(&opt,"flag",'x',&flag); 
 

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  OptionFree(&opt);

  if (flag) fprintf(stdout,"flag set\n");
  else fprintf(stdout,"flag not set\n");
 
 
  return 0;
}
   


