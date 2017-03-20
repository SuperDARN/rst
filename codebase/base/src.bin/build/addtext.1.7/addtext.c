/* addtext.c
   =========
   Author R.J.Barnes
*/

/* 
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hlpstr.h"

char buffer[4096];
char text_buffer[4096];
char dstr[64]={"*License*"};
char *rtext={"text"};

void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,str[i]);
}


int instr(char *body,char *str) {

  int i=0;
  int j=0;
  while (body[i] !=0) {
    if (body[i]==str[j]) j++;
    else j=0;
    if (str[j]==0) break;
    i++;
  }
  if (body[i]==0) return -1;
  return i-j+1;
}

int main(int argc,char *argv[]) {
  int tlen=0;
  char *tstr=NULL;
  char *rstr=NULL;
  char *prefix;

  FILE *fp;
  char *texts=NULL;
  int i,j,c;

  for (c=1;c<argc;c++) {

    if (strcmp(argv[c],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    } if (strcmp(argv[c],"-t")==0) {
      tstr=argv[c+1];
      c++;
    } else if (strcmp(argv[c],"-r")==0) {
      rstr=argv[c+1];
      c++;
    } else break;
  }
  
  if (tstr==NULL) {
    tstr=dstr;
    for (i=0;dstr[i] !=0;i++) if (dstr[i]=='*') dstr[i]='$'; 
  }
  if (rstr==NULL) rstr=rtext;
  

  if (c<argc) texts=argv[c];

  tlen=strlen(tstr);  

  while(fgets(buffer,4096,stdin) !=NULL) {
  

    if ((i=instr(buffer,tstr)) !=-1) { 
      buffer[i]=0;
      prefix=buffer;

      if (instr(prefix,"/*") !=-1) {
        fprintf(stdout,"%s\n",prefix);
        for (j=0;prefix[j] !=0;j++) if (prefix[j]=='/') prefix[j]=' ';
      }
      if (instr(prefix,"(*") !=-1) {
        fprintf(stdout,"%s\n",prefix);
        for (j=0;prefix[j] !=0;j++) if (prefix[j]=='(') prefix[j]=' ';
      }

      if (texts !=NULL) {
        fp=fopen(texts,"r");
        while(fgets(text_buffer,4096,fp) !=NULL) {           
          fprintf(stdout,"%s%s",prefix,text_buffer);
        }
        fclose(fp);
        fprintf(stdout,"%s%s",prefix,buffer+i+tlen);
      } else {
        fprintf(stdout,"%s%s%s",prefix,rstr,buffer+i+tlen);
      }
      continue;
    }
    fprintf(stdout,"%s",buffer);
  }
  return 0;
}
