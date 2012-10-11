/* OptionDump.c
   ============
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "option.h"


struct OptionData opt;
int arg=0;

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char flag=0;
  int ival;
  char *tval=NULL;

  OptionAdd(&opt,"help",'x',&help);
  OptionAdd(&opt,"flag",'x',&flag); 
  OptionAdd(&opt,"int",'i',&ival); 
  OptionAdd(&opt,"text",'s',&tval); 


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  OptionFree(&opt);

  if (flag) fprintf(stdout,"flag set\n");
  else fprintf(stdout,"flag not set\n");
 
 
  return 0;
}
   


