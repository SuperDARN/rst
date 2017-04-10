/* printinfo.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>



void OptionPrintInfo(FILE *fp,char *str[]) {
  int i,j,k,l,m,n;
  int maxtab=0;
  char buffer[4096];
  /* first go through looking for tabs */

  for (i=0;str[i] !=NULL;i++) {
    for (j=0;str[i][j] !=0;j++) if (str[i][j]=='\t') break;
    if ((str[i][j] !=0) && (j>maxtab)) maxtab=j;
  } 
  maxtab++;

  /* now output the lines */

  for (i=0;str[i] !=NULL;i++) {
    for (j=0;str[i][j] !=0;j++) if (str[i][j]=='\t') break;
    if (str[i][j]==0) {
      fprintf(fp,str[i]);
      continue;
    }
    strncpy(buffer,str[i],4095);
    buffer[j]=0;
    fprintf(fp,buffer);
    for (k=0;k<(maxtab-j);k++) buffer[k]=' ';
    buffer[k]=0;
    fprintf(fp,buffer);
    j++;
    /* chop the rest of the buffer up into 80 character long lines */
    m=0;
    l=maxtab;
    for (k=j;str[i][k] !=0;k++) {
      buffer[m]=str[i][k];
      m++;
      if (buffer[m-1]==' ') {
        if ((l+m-1)>80) {
	  /* new line */
          fprintf(fp,"\n");
          for (n=0;n<maxtab;n++) buffer[m+n]=' ';
	  buffer[m+n]=0;
          fprintf(fp,buffer+m);
          l=maxtab;
        }
        buffer[m]=0; 
        fprintf(fp,buffer);
        l+=m-1;
        m=0;
      }     
    }
    if ((l+m-1)>80) {
      /* new line */
      fprintf(fp,"\n");
      l=maxtab;
    }
    buffer[m]=0;
    fprintf(fp,buffer);
  }
}


