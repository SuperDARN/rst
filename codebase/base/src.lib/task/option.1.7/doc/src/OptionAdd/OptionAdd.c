/* OptionAdd.c
   ===========
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "option.h"


struct OptionData opt;
int arg=0;

int main(int argc,char *argv[]) {

  unsigned char flag=0;
  char *text=NULL;

  OptionAdd(&opt,"flag",'x',&flag); 
  OptionAdd(&opt,"text",'t',&text);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  OptionFree(&opt);

  if (flag) fprintf(stdout,"flag set\n");
  else fprintf(stdout,"flag not set\n");
  if (text !=NULL) fprintf(stdout,"%s\n",text);
  
  return 0;
}
   


