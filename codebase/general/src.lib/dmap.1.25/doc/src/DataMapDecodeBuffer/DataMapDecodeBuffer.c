/* DataMapRead.c
   ==============
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "rtypes.h"
#include "dmap.h"
#include "connex.h"


int main(int argc,char *argv[]) {

  char *host;
  int port;

  int sock;
  int flag,status,size;
  unsigned char *buffer=NULL;
 
  struct DataMap *ptr=NULL;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  char **tmp;

 
  int c;
  int x,n;

  if (argc<3) { 
    fprintf(stderr,"host and port must be specified.'n");
    exit(-1);
  }

  host=argv[1];
  port=atoi(argv[2]);
 
  sock=ConnexOpen(host,port); 
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.'n");
    exit(-1);
  }
  do {
   status=ConnexRead(1,&sock,&buffer,&size,&flag,NULL);
   if (status==-1) break;
   if (flag !=1) continue;
   if (size==0) continue;

   ptr=DataMapDecodeBuffer(buffer,size);
   if (ptr==NULL) fprintf(stderr,"Not a recognized message.\n");

    fprintf(stdout,"scalars:\n");
    for (c=0;c<ptr->snum;c++) {
      s=ptr->scl[c];
      switch (s->type) {
        case DATACHAR:
        fprintf(stdout,"\tchar");
        break;
        case DATASHORT:
        fprintf(stdout,"\tshort");
        break;
        case DATAINT:
        fprintf(stdout,"\tint");
        break;
        case DATAFLOAT:
        fprintf(stdout,"\tfloat");
        break;
        case DATADOUBLE:
        fprintf(stdout,"\tdouble");
        break;
        case DATASTRING:
        fprintf(stdout,"\tstring");
        break;
      }
      fprintf(stdout,"\t%c%s%c",'"',s->name,'"');
      fprintf(stdout," = ");
      switch (s->type) {
        case DATACHAR:
        fprintf(stdout,"%d",*(s->data.cptr));
        break;
        case DATASHORT:
        fprintf(stdout,"%d",*(s->data.sptr));
        break;
        case DATAINT:
        fprintf(stdout,"%d",*(s->data.lptr));
        break;
        case DATAFLOAT:
        fprintf(stdout,"%g",*(s->data.fptr));
        break;
        case DATADOUBLE:
        fprintf(stdout,"%g",*(s->data.dptr));
        break;
        case DATASTRING:
	tmp=(char **) s->data.vptr;
        fprintf(stdout,"%c%s%c",'"',*tmp,'"');
        break;
      }
      fprintf(stdout,"\n");
    }
    fprintf(stdout,"arrays:\n");
    for (c=0;c<ptr->anum;c++) {
      a=ptr->arr[c];
      switch (a->type) {
        case DATACHAR:
        fprintf(stdout,"\tchar");
        break;
        case DATASHORT:
        fprintf(stdout,"\tshort");
        break;
        case DATAINT:
        fprintf(stdout,"\tint");
        break;
        case DATAFLOAT:
        fprintf(stdout,"\tfloat");
        break;
        case DATADOUBLE:
        fprintf(stdout,"\tdouble");
        break;
        case DATASTRING:
        fprintf(stdout,"\tstring");
        break;
      }
      fprintf(stdout,"\t%c%s%c",'"',a->name,'"');
      fprintf(stdout," ");
      for (x=0;x<a->dim;x++) fprintf(stdout,"[%d]",a->rng[a->dim-1-x]);
    
      fprintf(stdout,"=");
      n=1;
      for (x=0;x<a->dim;x++) n=a->rng[x]*n;
      for (x=0;x<n;x++) {
        if (x % a->rng[0]==0) fprintf(stdout,"\n\t\t");
        else if (x !=0) fprintf(stdout,",\t");
        switch (a->type) {
        case DATACHAR:
          fprintf(stdout,"%d",a->data.cptr[x]);
          break;
        case DATASHORT:
          fprintf(stdout,"%d",a->data.sptr[x]);
          break;
        case DATAINT:
          fprintf(stdout,"%d",a->data.lptr[x]);
          break;
        case DATAFLOAT:
          fprintf(stdout,"%g",a->data.fptr[x]);
          break;
        case DATADOUBLE:
          fprintf(stdout,"%g",a->data.dptr[x]);
          break;	    
        case DATASTRING:
          tmp=(char **) a->data.vptr;
          fprintf(stdout,"%c%s%c",'"',tmp[x],'"');
          break;
	    
        }  
        fprintf(stdout,"\n");
      } 
    }  
    DataMapFree(ptr);

  } while (1);
  return 0;

}
