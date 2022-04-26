/* OptionProcessFile.c
   ===================
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:



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
   


