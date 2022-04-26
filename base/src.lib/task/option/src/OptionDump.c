/* OptionDump.c
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
   


