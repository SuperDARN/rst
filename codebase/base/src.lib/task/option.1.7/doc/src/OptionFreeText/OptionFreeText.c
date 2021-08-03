/* OptionFree.c
   ============
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
   


