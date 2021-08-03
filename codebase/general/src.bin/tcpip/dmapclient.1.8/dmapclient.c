/* ipclient.c
   ==========
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
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "option.h"
#include "rtypes.h"
#include "dmap.h"
#include "connex.h"
#include "errstr.h"
#include "hlpstr.h"



struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: dmapclient --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int arg;
  int sock;
  int remote_port=0;
  char *host;
  int flag,status,size;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char *buffer=NULL;
  unsigned char stream=255;

  char **tmp; 
  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  int c,n,x;

  int sval=255;
  unsigned char dflg=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"d",'x',&dflg);
  OptionAdd(&opt,"s",'i',&sval);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  if (argc-arg<2) { 
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  host=argv[argc-2];
  remote_port=atoi(argv[argc-1]);
  stream=sval;

  sock=ConnexOpen(host,remote_port,NULL); 
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
  status=(write(sock,&stream,sizeof(char)) !=1);
  if (status !=0) {
    fprintf(stderr,"Could not set stream to host.\n");
    exit(-1);
  }
 
  do {
   status=ConnexRead(1,&sock,&buffer,&size,&flag,NULL);
   if (status==-1) break;
   if (flag !=-1) {
     fprintf(stderr,"Message size:%d\n",size);
     if (size==0) continue;
     ptr=DataMapDecodeBuffer(buffer,size);
     if (ptr==NULL) fprintf(stderr,"Not a recognized message.\n");
     else {
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
	   fprintf(stdout,"%d",*(s->data.iptr));
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
	 for (x=0;x<a->dim;x++) fprintf(stdout,"[%d]",a->rng[x]);
	 if (dflg) {
	   fprintf(stdout,"=");
	   n=1;
	   for (x=0;x<a->dim;x++) n=a->rng[x]*n;
	   for (x=0;x<n;x++) {
	     if (x % a->rng[a->dim-1]==0) fprintf(stdout,"\n\t\t");
	     else if (x !=0) fprintf(stdout,",\t");
	     switch (a->type) {
	     case DATACHAR:
	       fprintf(stdout,"%d",a->data.cptr[x]);
	       break;
	     case DATASHORT:
	       fprintf(stdout,"%d",a->data.sptr[x]);
	       break;
	     case DATAINT:
	       fprintf(stdout,"%d",a->data.iptr[x]);
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
	   }
	   fprintf(stdout,"\n");
	 } else fprintf(stdout,"\n");
       }
      DataMapFree(ptr);
     }
   }           
  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;

}
   

 










