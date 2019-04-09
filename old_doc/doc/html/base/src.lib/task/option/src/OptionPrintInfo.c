/* OptionPrintInfo.c
   =================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "option.h"


struct OptionData opt;
int arg=0;

char *hlpstr[]={"OptionPrintInfo - Test of the print info function\n",
              "OptionPrintInfo -help\n",
              "OptionPrintInfo\n",
              0};

int main(int argc,char *argv[]) {

  unsigned char help=0;

  OptionAdd(&opt,"-help",'x',&help); 
 

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  OptionFree(&opt);

  if (help) {
    OptionPrintInfo(stderr,hlpstr);
    exit(0);
  }

 
 
  return 0;
}
   


