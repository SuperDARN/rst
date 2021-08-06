/* XMLMake.c
   =========
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
#include <string.h>
#include "rxml.h"

int start(char *name,int atnum,char **atname,char **atval,char *buf,
          int sze,void *data) {
  FILE *fp=NULL;

  if (data !=NULL) fp=(FILE *) data;
  else fp=stderr;

  fprintf(fp,"<%s>",name);


  return 0;
}

int end(char *name,char *buf,
          int sze,void *data) {

  FILE *fp=NULL;
  if (data !=NULL) fp=(FILE *) data;
  else fp=stderr;
    fprintf(fp,"</%s>",name);

  return 0;
}


int text(char *buf,
          int sze,void *data) {

  FILE *fp=NULL;
  if (data !=NULL) fp=(FILE *) data;
  else fp=stderr;

  fwrite(buf,sze,1,fp);
  return 0;
}



int main(int argc,char *argv[]) {
 
  FILE *inp;
  FILE *outp=stdout;
  char lbuf[256];
  int s;

  struct XMLdata *xmldata;
 
  if (argc>1) {
    inp=fopen(argv[1],"r");
    if (inp==NULL) {
      fprintf(stderr,"File not found.\n");
    }
  } else inp=stdin;

  xmldata=XMLMake();

  XMLSetStart(xmldata,start,outp);
  XMLSetEnd(xmldata,end,outp);
  XMLSetText(xmldata,text,outp);

  while(fgets(lbuf,255,inp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) return -1;
  }
  XMLFree(xmldata);

  return 0;

}
