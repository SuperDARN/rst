/* dmaptoskeleton.c
   ================
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "hlpstr.h"
#include "errstr.h"




#define STRINGSIZE 256

int snum=0;
struct DataMapScalar **sptr=NULL;
char **cdfsname;

int anum=0;
struct DataMapArray **aptr=NULL;
char **cdfaname;

int32 *rptr=NULL;
int32 *roff=NULL;
int rnum=0; 

struct OptionData opt;
int arg=0;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: dmaptoskeleton --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int status=0;
  struct DataMap *ptr;
  struct DataMapScalar *sx=NULL,*sy=NULL;
  struct DataMapArray *ax=NULL,*ay=NULL;

  char buf[1024];
  unsigned char vbflg=0;
  unsigned char zflg=0;

  FILE *fp=NULL;
  gzFile zfp=0;
  int c,i;
  int x=0,n;
  int tab;

  char *cdfname=NULL;
  char *cdfdef={"dmapcdf"};
  int block=0;
  int rnum=0,znum=0;
  int nelm=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vbflg);
  OptionAdd(&opt,"z",'x',&zflg);

  if (argc>1) {
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
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

  if (cdfname==NULL) cdfname=cdfdef;

  while (1) {

    if (zflg) ptr=DataMapReadZ(zfp);
    else ptr=DataMapFread(fp);

    if (ptr==NULL) break;

    for (c=0;c<ptr->snum;c++) {
      sx=ptr->scl[c];
      for (n=0;n<snum;n++) {
        sy=sptr[n];
        if (strcmp(sx->name,sy->name) !=0) continue;
        if (sx->type !=sy->type) continue;
        break;
      }
      if (n==snum) {
        /* copy the scalar */
        if (sptr==NULL) sptr=malloc(sizeof(struct DataMapScalar *));
        else {
          struct DataMapScalar **tmp;
          tmp=realloc(sptr,(snum+1)*sizeof(struct DataMapScalar *));
          if (tmp==NULL) break;
          sptr=tmp;
        }
        sptr[snum]=DataMapMakeScalar(sx->name,0,sx->type,NULL);
        snum++;
      }
    }
    if (c !=ptr->snum) {
      status=-1;
      break;
    }
    for (c=0;c<ptr->anum;c++) {
      ax=ptr->arr[c];
      for (n=0;n<anum;n++) {
        ay=aptr[n];
        if (strcmp(ax->name,ay->name) !=0) continue;
        if (ax->type != ay->type) continue;
        if (ax->dim != ay->dim) continue;
        break;
      }
      if (n !=anum) {
        /* check the ranges */
        for (i=0;i<ax->dim;i++)
          if (ax->rng[i]>rptr[roff[n]+i]) rptr[roff[n]+i]=ax->rng[i];
      }

      if (n==anum) {
        /* copy the array */
        if (roff==NULL) roff=malloc(sizeof(int32));
        else {
          int32 *tmp;
          tmp=realloc(roff,(anum+1)*sizeof(int32));
          if (tmp==NULL) break;
          roff=tmp;
        }
        roff[anum]=rnum;
        if (rptr==NULL) rptr=malloc(sizeof(int32)*ax->dim);
        else {
          int32 *tmp;
          tmp=realloc(rptr,(rnum+ax->dim)*sizeof(int32));
          if (tmp==NULL) break;
            rptr=tmp;
        }
        for (i=0;i<ax->dim;i++) rptr[rnum+i]=ax->rng[i];
        rnum+=ax->dim;

        if (aptr==NULL) aptr=malloc(sizeof(struct DataMapArray *));
        else {
          struct DataMapArray **tmp;
          tmp=realloc(aptr,(anum+1)*sizeof(struct DataMapArray *));
          if (tmp==NULL) break;
          aptr=tmp;
        }
        aptr[anum]=DataMapMakeArray(ax->name,0,ax->type,ax->dim,NULL,NULL);
        anum++;

      }

    }
    if (c !=ptr->anum) {
       status=-1;
       break;
    }
    DataMapFree(ptr);
    block++;
  }
  if (zflg) gzclose(zfp);
  else fclose(fp);

  if (status==-1) {
    fprintf(stderr,"Error processing file.\n");
    exit(-1);
  }

  for (n=0;n<anum;n++) { 
     ax=aptr[n];
     ax->rng=rptr+roff[n];
     for (c=0;c<ax->dim;c++) if (ax->rng[c]==0) break; /* bad data */
     if (c !=ax->dim) ax->dim=0;
  }

  cdfsname=malloc(sizeof(char *)*snum);
  if (cdfsname==NULL) {
    fprintf(stderr,"Could not allocate scalar name table.\n");
    exit(-1);
  }

  cdfaname=malloc(sizeof(char *)*anum);
  if (cdfaname==NULL) {
    fprintf(stderr,"Could not allocate array name table.\n");
    exit(-1);
  }

  for (n=0;n<snum;n++) { 
    sx=sptr[n];
    cdfsname[n]=malloc(strlen(sx->name)+1);
    for (c=0;sx->name[c] !=0;c++) 
       if (isalnum(sx->name[c])) cdfsname[n][c]=sx->name[c];
    else cdfsname[n][c]='_';
    cdfsname[n][c]=0;
    x=0;
    for (c=0;c<n;c++) if (strncmp(cdfsname[c],cdfsname[n],
                          strlen(cdfsname[n]))==0) x++;; 
    if (x !=0) {
      char *tmp;
      char txt[4];
      x++;
      sprintf(txt,"%.3d",x);
      tmp=realloc(cdfsname[n],strlen(cdfsname[n])+4);
      if (tmp==NULL) break;
      cdfsname[n]=tmp;
      strcat(cdfsname[n],txt);
    }
  }

  if (n !=snum) {
    fprintf(stderr,"Error generating names.\n");
    exit(-1);
  }

  for (n=0;n<anum;n++) { 
    ax=aptr[n];
    cdfaname[n]=NULL;
    if (ax->dim==0) continue;
    cdfaname[n]=malloc(strlen(ax->name)+1);
    for (c=0;ax->name[c] !=0;c++) 
      if (isalnum(ax->name[c])) cdfaname[n][c]=ax->name[c];
    else cdfaname[n][c]='_';
    cdfaname[n][c]=0;
    for (c=0;c<n;c++) if (strncmp(cdfaname[c],cdfaname[n],
                          strlen(cdfaname[n]))==0) x++;
    if (x !=0) {
      char *tmp;
      char txt[4];
      x++;
      sprintf(txt,"%.3d",x);
      tmp=realloc(cdfaname[n],strlen(cdfaname[n])+4);
      if (tmp==NULL) break;
      cdfaname[n]=tmp;
      strcat(cdfaname[n],txt);
    }

  }

  if (n !=anum) {
    fprintf(stderr,"Error generating names.\n");
    exit(-1);
  }

  rnum=snum;
  znum=anum;

  fp=fopen(argv[2],"w");
  fprintf(fp,"#header\n");
  fprintf(fp,"                            CDF NAME: %s\n",cdfname);
  fprintf(fp,"                       DATA ENCODING: NETWORK\n");
  fprintf(fp,"                            MAJORITY: ROW\n");
  fprintf(fp,"                              FORMAT: SINGLE\n\n");
  fprintf(fp,
     "! Variables  G.Attributes  V.Attributes  Records  Dims  Sizes\n");
  fprintf(fp,
     "! ---------  ------------  ------------  -------  ----  -----\n");
  fprintf(fp,
     "  %d/%d           0             0         %d/z    0\n",
     rnum,znum,block);

  fprintf(fp,"\n#GLOBALattributes\n");
  fprintf(fp,"\n#VARIABLEattributes\n");

  fprintf(fp,"\n#variables\n\n");
  for (n=0;n<snum;n++) {
    sx=sptr[n];
    fprintf(fp,"! Variable          Data      Number    Record   Dimension\n");
    fprintf(fp,"! Name              Type     Elements  Variance  Variances\n");
    fprintf(fp,"! --------          ----     --------  --------  ---------\n");
    fprintf(fp,"\n");
    sprintf(buf,"  %c%s%c",'"',cdfsname[n],'"');
    fprintf(fp,"%s",buf);
    tab=18-strlen(buf);
    if (tab>0) for (c=0;c<tab;c++) fprintf(fp," ");
    else fprintf(fp," ");
    switch (sx->type) {
    case DATACHAR:
      fprintf(fp,"CDF_CHAR  ");
      break;
    case DATASHORT:
      fprintf(fp,"CDF_INT2  ");
      break;
    case DATAINT:
      fprintf(fp,"CDF_INT4  ");
      break;
    case DATAFLOAT:
      fprintf(fp,"CDF_FLOAT ");
      break;
    case DATADOUBLE:
      fprintf(fp,"CDF_DOUBLE");
      break;
    case DATASTRING:
      fprintf(fp,"CDF_CHAR  ");
      break;

    }
    nelm=1;
    if (sx->type==DATASTRING) nelm=STRINGSIZE;
    fprintf(fp,"    %d         T          ",nelm);
    fprintf(fp,"\n\n");
    fprintf(fp,"! Attribute         Data     Value\n");
    fprintf(fp,"! Name              Type\n");
    fprintf(fp,"! --------          ----     --------\n");
    fprintf(fp,"\n.\n\n");

  }
  fprintf(fp,"\n#zVariables\n\n");

  for (n=0;n<anum;n++) {
    ax=aptr[n];

    fprintf(fp,"! Variable          Data      Number");
    fprintf(fp,"                 Record   Dimension\n");
    fprintf(fp,"! Name              Type     Elements  Dims");
    fprintf(fp,"  Sizes  Variance  Variances\n");
    fprintf(fp, "! --------          ----     --------  ----");
    fprintf(fp,"  -----  --------  ---------\n");
    fprintf(fp,"\n");
    sprintf(buf,"  %c%s%c",'"',cdfaname[n],'"');
    fprintf(fp,"%s",buf);
    tab=18-strlen(buf);
    if (tab>0) for (c=0;c<tab;c++) fprintf(fp," ");
    else fprintf(fp," ");
    switch (ax->type) {
    case DATACHAR:
      fprintf(fp,"CDF_CHAR  ");
      break;
    case DATASHORT:
      fprintf(fp,"CDF_INT2  ");
      break;
    case DATAINT:
      fprintf(fp,"CDF_INT4  ");
      break;
    case DATAFLOAT:
      fprintf(fp,"CDF_FLOAT ");
      break;
    case DATADOUBLE:
      fprintf(fp,"CDF_DOUBLE");
      break;
    case DATASTRING:
      fprintf(fp,"CDF_CHAR  ");
      break;

    }

    nelm=1;
    if (ax->type==DATASTRING) nelm=STRINGSIZE;
    fprintf(fp,"    %d         %d     ",nelm,ax->dim);
    for (c=0;c<ax->dim;c++) fprintf(fp,"%d ",ax->rng[c]);
    fprintf(fp,"    T     ");
    for (c=0;c<ax->dim;c++) fprintf(fp,"T ");
    fprintf(fp,"\n\n");
    fprintf(fp,"! Attribute         Data     Value\n");
    fprintf(fp,"! Name              Type\n");
    fprintf(fp,"! --------          ----     --------\n");
    fprintf(fp,"\n.\n\n");

  }

  fprintf(fp,"#end\n\n");
  fclose(fp);

  fp=fopen(argv[3],"w");
  for (n=0;n<snum;n++) {
    fprintf(fp,"\t");
    sx=sptr[n];
    switch(sx->type) {
      case DATACHAR:
        fprintf(fp,"char 0");
        break;
      case DATASHORT:
        fprintf(fp,"short 0");
        break;
      case DATAINT:
        fprintf(fp,"int 0");
        break;
      case DATAFLOAT:
        fprintf(fp,"float 0");
        break;
      case DATADOUBLE:
        fprintf(fp,"double 0");
        break;
      case DATASTRING:
        fprintf(fp,"string 0"); 
    }
    fprintf(fp," %c%s%c=%s;\n",'"',sx->name,'"',cdfsname[n]);
  }

  for (n=0;n<anum;n++) {
    fprintf(fp,"\t");
    ax=aptr[n];
    if (ax->dim==0) continue;
    switch(ax->type) {
      case DATACHAR:
        fprintf(fp,"char");
        break;
      case DATASHORT:
        fprintf(fp,"short");
        break;
      case DATAINT:
        fprintf(fp,"int");
        break;
      case DATAFLOAT:
        fprintf(fp,"float");
        break;
      case DATADOUBLE:
        fprintf(fp,"double");
        break;
      case DATASTRING:
        fprintf(fp,"string"); 
    }
    fprintf(fp," %d",ax->dim); 
    fprintf(fp," %c%s%c=%s;\n",'"',ax->name,'"',cdfaname[n]);
  }
  fclose(fp);
  return 0;
}
