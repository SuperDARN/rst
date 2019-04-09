/* XMLSetStart.c
   =============
   Author: R.J.Barnes
*/


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

  while(fgets(lbuf,255,inp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) return -1;
  }
  XMLFree(xmldata);

  return 0;

}
