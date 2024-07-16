/* grid_filter.c
   ============= 
   Author: R.J.Barnes
*/

/*
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
#include <math.h>
#include <sys/types.h>
#include "option.h"
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "hlpstr.h"


struct OptionData opt;
struct GridData *igrd;
struct GridData *ogrd;

struct CpidTable {
  int num;
  int *start;
  int *end;
};

struct CpidTable *CpidLoadTable(FILE *fp);
int filter_grid(struct GridData *out, struct GridData *in,
                struct CpidTable *cpid,double minrng, double maxrng);


int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: grid_filter --help\n");
  return(-1);
}


int main(int argc,char *argv[]) {

  FILE *fp;
  char *envstr;
  char cpid_filepath[250];

  int old=0;

  int arg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  unsigned char cpid=0;
  struct CpidTable *ctable=NULL;

  double minrng=0;
  double maxrng=10000;

  int yr,mo,dy,hr,mt;
  double sc;

  igrd=GridMake();
  ogrd=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"old",'x',&old);

  OptionAdd(&opt,"cpid",'x',&cpid);
  OptionAdd(&opt,"minrng",'d',&minrng);
  OptionAdd(&opt,"maxrng",'d',&maxrng);

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

  if (cpid) {
    envstr = getenv("RSTPATH");
    if (envstr == NULL) {
      fprintf(stderr,"Environment variable RSTPATH must be defined.\n");
      exit(-1);
    }
    sprintf(cpid_filepath,"%s/tables/superdarn/cpid/cpid.dat",envstr);
    fp = fopen(cpid_filepath, "r");
    if (fp != NULL) {
      ctable = CpidLoadTable(fp);
      fclose(fp);
      if (ctable == NULL) {
        fprintf(stderr,"Error loading CPID table %s\n",cpid_filepath);
        exit(-1);
      }
    }
  }

  if (arg !=argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(1);
    }
  } else fp=stdin;

  if (old) {
    while (OldGridFread(fp,igrd) !=-1) {

      filter_grid(ogrd,igrd,ctable,minrng,maxrng);

      if (vb) {
        TimeEpochToYMDHMS(ogrd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d  %d > %d pts (%.2f%%)\n",
                        yr,mo,dy,hr,mt,(int) sc,igrd->vcnum,ogrd->vcnum,
                        (float)ogrd->vcnum/igrd->vcnum*100);
      }

      OldGridFwrite(stdout,ogrd);
    }
  } else {
    while (GridFread(fp,igrd) !=-1) {

      filter_grid(ogrd,igrd,ctable,minrng,maxrng);

      if (vb) {
        TimeEpochToYMDHMS(ogrd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d  %d > %d pts (%.2f%%)\n",
                        yr,mo,dy,hr,mt,(int) sc,igrd->vcnum,ogrd->vcnum,
                        (float)ogrd->vcnum/igrd->vcnum*100);
      }

      GridFwrite(stdout,ogrd);
    }
  }

  if (fp !=stdin) fclose(fp);

  return 0;
}


struct CpidTable *CpidLoadTable(FILE *fp) {

  char line[1024];
  char *tkn;
  int i;
  int s,e,status;
  struct CpidTable *ptr;
  ptr=malloc(sizeof(struct CpidTable));
  if (ptr==NULL) return NULL;

  ptr->num = 0;
  ptr->start = NULL;
  ptr->end = NULL;

  while (fgets(line,1024,fp) !=0) {
    for (i=0; (line[i] != 0) &&
              ((line[i] == ' ') || (line[i] == '\n')); i++);

    if ((line[i]==0) || (line[i]=='#')) continue;

    tkn = line+i;
    status = sscanf(tkn, "%d %d", &s, &e);
    if (status == 2) {
      if (ptr->start == NULL) ptr->start = malloc(sizeof(int));
      else                    ptr->start = realloc(ptr->start, sizeof(int)*(ptr->num+1));

      if (ptr->end == NULL) ptr->end = malloc(sizeof(int));
      else                  ptr->end = realloc(ptr->end, sizeof(int)*(ptr->num+1));

      ptr->start[ptr->num] = s;
      ptr->end[ptr->num] = e;
      ptr->num++;
    }
  }

  return ptr;
}


int filter_grid(struct GridData *out, struct GridData *in,
                struct CpidTable *ctable, double minrng, double maxrng) {

  int i,j=0;
  int vcnt=0;
  int status=0;
  int cpid=0;

  out->st_time = in->st_time;
  out->ed_time = in->ed_time;
  out->stnum = in->stnum;
  out->xtd = in->xtd;

  /* Copy station info */
  if (in->stnum > 0) {
    if (out->sdata == NULL) out->sdata = malloc(sizeof(struct GridSVec)*in->stnum);
    else                    out->sdata = realloc(out->sdata,sizeof(struct GridSVec)*in->stnum);
    memcpy(out->sdata,in->sdata,sizeof(struct GridSVec)*in->stnum);
  } else if (out->sdata != NULL) {
    free(out->sdata);
    out->sdata=NULL;
  }

  /* Search through all grid vectors in record to see which meet criteria */
  for (i=0; i<in->vcnum; i++) {
    if (in->data[i].srng >= minrng && in->data[i].srng < maxrng) {

      /* Apply CPID criteria if selected */
      if (ctable != NULL) {
        status = 0;

        /* Get CPID of grid vector from station info */
        for (j=0; j<in->stnum; j++) {
          if (in->data[i].st_id == in->sdata[j].st_id) {
            cpid = abs(in->sdata[j].prog_id);
            break;
          }
        }

        /* Check if CPID is valid */
        for (j=0; j<ctable->num; j++) {
          if ((ctable->start[j] <= cpid) && (cpid <= ctable->end[j])) {
            status = 1;
            break;
          }
        }
        if (status == 0) continue;
      }

      /* All criteria are met, so keep grid vector */
      if (out->data == NULL) out->data = malloc(sizeof(struct GridGVec));
      else                   out->data = realloc(out->data,sizeof(struct GridGVec)*(vcnt+1));

      memcpy(&out->data[vcnt],&in->data[i],sizeof(struct GridGVec));

      vcnt++;
    }
  }

  out->vcnum = vcnt;

  /* Update number of grid vectors in station info */
  for (i=0; i<out->stnum; i++) out->sdata[i].npnt = 0;

  for (i=0; i<out->vcnum; i++) {
    for (j=0; j<out->stnum; j++) {
      if (out->data[i].st_id == out->sdata[j].st_id) {
        out->sdata[j].npnt++;
        break;
      }
    }
  }

  return 1;
}

