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

  int n=0;
  struct OptionText  *text=NULL;

  OptionAdd(&opt,"t",'a',&text);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  OptionFree(&opt);
  if (text !=NULL) for (n=0;n<text->num;n++) fprintf(stdout,"%s\n",text->txt[n]);

  
  return 0;
}
   


