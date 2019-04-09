/* OptionProcessFile.c
   ===================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "option.h"

struct OptionData opt;
struct OptionFile *optf=NULL;
int arg=0;

int main(int argc,char *argv[]) {
  FILE *fp;

  unsigned char flag=0;
  int ival=0;
  float fval=0.0;
  char *text=NULL;

  char *cfgname=NULL;

  OptionAdd(&opt,"flag",'x',&flag); 
  OptionAdd(&opt,"ival",'i',&ival);
  OptionAdd(&opt,"fval",'f',&fval);
  OptionAdd(&opt,"text",'t',&text);
  OptionAdd(&opt,"cf",'t',&cfgname);
  
  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (cfgname !=NULL) {
    fp=fopen(cfgname,"r");
    if (fp !=NULL) {
      optf=OptionProcessFile(fp);
      fclose(fp);
    }
    if (optf !=NULL) {
      arg=OptionProcess(0,optf->argc,optf->argv,&opt,NULL);
      OptionFreeFile(optf);
    }
  }

  OptionFree(&opt);

  if (flag) fprintf(stdout,"flag set\n");
  else fprintf(stdout,"flag not set\n");
  fprintf(stdout,"ival=%d\n",ival);
  fprintf(stdout,"fval=%g\n",fval);
  if (text !=NULL) fprintf(stdout,"text=%s\n",text);
 
  return 0;
}
   


