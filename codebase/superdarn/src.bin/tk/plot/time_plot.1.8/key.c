/* key.c
   =====
   Author: R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "key.h"



int load_key(FILE *fp,struct key *key) {

  int i,j=0;
  unsigned int colval;
  int num=0;
  char line[1024];
  char *tkn;
  while (fgets(line,1024,fp) !=0) {
  
    for (i=0; (line[i] !=0) && 
              ((line[i] ==' ') || (line[i]=='\n'));i++);

   
    if ((line[i]==0) || (line[i]=='#')) continue;
    tkn=line+i;
    if (num==0) {
      sscanf(tkn,"%d",&num);
      if (num !=0) {
        key->a=malloc(num);
        key->r=malloc(num);
        key->g=malloc(num);  
        key->b=malloc(num);
        memset(key->a,0,num);
        memset(key->r,0,num);
        memset(key->g,0,num);
        memset(key->b,0,num);
        key->max=num;
        j=0;
      }
    } else if (j<num) {
      sscanf(tkn,"%x",&colval);
      if (strlen(tkn)>7) key->a[j]=(colval>>24) & 0xff;
      else key->a[j]=0xff;
      key->r[j]=(colval>>16) & 0xff;
      key->g[j]=(colval>>8) & 0xff;
      key->b[j]=colval & 0xff;
      j++;
    }
  }
  return num;
}



