/* OptionProcess.c
   ===============
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "option.h"


struct OptionData opt;
int arg=0;

int opterr(char *txt) {
  fprintf(stderr,"Could not recognize option:%s\n",txt);
  return 0;
}

int main(int argc,char *argv[]) {

  unsigned char flag=0;
  int ival=0;
  float fval=0.0;
  char *text=NULL;


  OptionAdd(&opt,"flag",'x',&flag); 
  OptionAdd(&opt,"ival",'i',&ival);
  OptionAdd(&opt,"fval",'f',&fval);
  OptionAdd(&opt,"text",'t',&text);
  

  arg=OptionProcess(1,argc,argv,&opt,opterr);

  OptionFree(&opt);

  if (flag) fprintf(stdout,"flag set\n");
  else fprintf(stdout,"flag not set\n");
  fprintf(stdout,"ival=%d\n",ival);
  fprintf(stdout,"fval=%g\n",fval);
  if (text !=NULL) fprintf(stdout,"text=%s\n",text);

  if (arg !=argc) fprintf(stdout,"final argument:%s\n",argv[arg]);

 
  return 0;
}
   


