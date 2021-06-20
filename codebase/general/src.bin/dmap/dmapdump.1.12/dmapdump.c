/* dmapdump.c
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "hlpstr.h"




struct OptionData opt;
int arg=0;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: dmapdump --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  char **tmp;
  unsigned char dflg=0;
  unsigned char zflg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;


  FILE *fp=NULL;
  gzFile zfp=0;
  int c;
  int x,n;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"z",'x',&zflg);

  OptionAdd(&opt,"d",'x',&dflg);

  
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

  if (arg !=argc) {
    if (zflg) {
      zfp=gzopen(argv[arg],"r");
      if (zfp==0) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    } else {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    }  
  } else {
    if (zflg) zfp=gzdopen(fileno(stdin),"r");
    else fp=stdin; 
  }
    
  while(1) {
    if (zflg) ptr=DataMapReadZ(zfp);
    else ptr=DataMapFread(fp);

    if (ptr==NULL) break;

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
        case DATALONG:
        fprintf(stdout,"\tlong");
        break;
        case DATAUCHAR:
        fprintf(stdout,"\tunsigned char");
        break;
        case DATAUSHORT:
        fprintf(stdout,"\tunsigned short");
        break;
        case DATAUINT:
        fprintf(stdout,"\tunsigned int");
        break;
        case DATAULONG:
        fprintf(stdout,"\tunsigned long");
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
        case DATALONG:
        fprintf(stdout,"%lld",*(s->data.lptr));
        break;
        case DATAUCHAR:
	fprintf(stdout,"%u",*(s->data.ucptr));
        break;
        case DATAUSHORT:
        fprintf(stdout,"%u",*(s->data.usptr));
        break;
        case DATAUINT:
        fprintf(stdout,"%u",*(s->data.uiptr));
        break;
        case DATAULONG:
        fprintf(stdout,"%llu",*(s->data.ulptr));
        break;
        case DATAFLOAT:
        fprintf(stdout,"%f",*(s->data.fptr));
        break;
        case DATADOUBLE:
        fprintf(stdout,"%lf",*(s->data.dptr));
        break;
        case DATASTRING:
	tmp=(char **) s->data.vptr;
        if (*tmp !=NULL) fprintf(stdout,"%c%s%c",'"',*tmp,'"');
        else fprintf(stdout,"%c%c",'"','"');
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
      if (dflg) {
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
            fprintf(stdout,"%d",a->data.iptr[x]);
            break;
            case DATAFLOAT:
            fprintf(stdout,"%f",a->data.fptr[x]);
            break;
            case DATADOUBLE:
            fprintf(stdout,"%lf",a->data.dptr[x]);
            break;	    
            case DATASTRING:
            tmp=(char **) a->data.vptr;
            if (tmp[x] !=NULL) fprintf(stdout,"%c%s%c",'"',tmp[x],'"');
	    else fprintf(stdout,"%c%c",'"','"');
            break;
	    
          }
        }  
        fprintf(stdout,"\n");
      } else fprintf(stdout,"\n");
    }  
    DataMapFree(ptr);

  }
  if (zflg) gzclose(zfp);
  else fclose(fp);
  return 0;  


}
