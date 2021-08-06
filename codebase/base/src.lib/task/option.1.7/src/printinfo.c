/* printinfo.c
   ===========
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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
      fprintf(fp,"%s",str[i]);
      continue;
    }
    strncpy(buffer,str[i],4095);
    buffer[j]=0;
    fprintf(fp,"%s",buffer);
    for (k=0;k<(maxtab-j);k++) buffer[k]=' ';
    buffer[k]=0;
    fprintf(fp,"%s",buffer);
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
          fprintf(fp,"%s",buffer+m);
          l=maxtab;
        }
        buffer[m]=0; 
        fprintf(fp,"%s",buffer);
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
    fprintf(fp,"%s",buffer);
  }
}


