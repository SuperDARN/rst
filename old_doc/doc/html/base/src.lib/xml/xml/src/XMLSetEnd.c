/* XMLSetEnd.c
   ===========
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"

int end(char *name,char *buf,
          int sze,void *data) {

  FILE *fp=NULL;
  if (data !=NULL) fp=(FILE *) data;
  else fp=stderr;
  fprintf(fp,"\nEnd Tag:%s\n",name);
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

  XMLSetEnd(xmldata,end,outp);
 
  while(fgets(lbuf,255,inp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) return -1;
  }
  XMLFree(xmldata);

  return 0;

}
