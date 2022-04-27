/* XMLCallStart.c
   ==============
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
  int a;
  FILE *fp=NULL;

  if (data !=NULL) fp=(FILE *) data;
  else fp=stderr;

  fprintf(fp,"\nStart Tag:%s\n",name);

  for (a=0;a<atnum;a++) { 
    fprintf(fp,"%s",atname[a]);
    if (atval[a] !=0) fprintf(fp,"=%s\n",atval[a]);
    else fprintf(fp,"\n");
  } 
  return 0;
}

int end(char *name,char *buf,
          int sze,void *data) {

  FILE *fp=NULL;
  if (data !=NULL) fp=(FILE *) data;
  else fp=stderr;
  fprintf(fp,"\nEnd Tag:%s\n",name);
  return 0;
}


int text(char *buf,
          int sze,void *data) {

  FILE *fp=NULL;
  if (data !=NULL) fp=(FILE *) data;
  else fp=stderr;
  fprintf(fp,"Body:");
  fwrite(buf,sze,1,fp);
  return 0;
}



int main(int argc,char *argv[]) {
 
  FILE *outp=stdout;
  char lbuf[256];

  char *atname[]={"width","height"};
  char *atval[]={"100","200"};

  struct XMLdata *xmldata;
 


  xmldata=XMLMake();

  XMLSetStart(xmldata,start,outp);
  XMLSetEnd(xmldata,end,outp);
  XMLSetText(xmldata,text,outp);

  sprintf(lbuf,"dummy contents.");

  XMLCallStart(xmldata,"table",2,atname,atval,"table",strlen("table"));

  XMLDecode(xmldata,lbuf,strlen(lbuf));

  XMLCallEnd(xmldata,"table","/table",strlen("/table"));

  XMLFree(xmldata);

  return 0;

}
