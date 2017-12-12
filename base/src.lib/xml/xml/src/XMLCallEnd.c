/* XMLCallEnd.c
   ============
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

  struct XMLdata *xmldata;
 


  xmldata=XMLMake();

  XMLSetStart(xmldata,start,outp);
  XMLSetEnd(xmldata,end,outp);
  XMLSetText(xmldata,text,outp);

  sprintf(lbuf,"dummy contents.");

  XMLCallStart(xmldata,"table",0,NULL,NULL,"table",strlen("table"));

  XMLDecode(xmldata,lbuf,strlen(lbuf));

  XMLCallEnd(xmldata,"table","/table",strlen("/table"));

  XMLFree(xmldata);

  return 0;

}
