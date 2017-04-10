/* dmaptoncdf.c
   ============
   Author: R.J.Barnes
*/


/*
   See license.txt
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
#include "netcdf.h"
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "hlpstr.h"
#include "errstr.h"





int snum=0;
struct DataMapScalar **sptr=NULL;
char **cdfsname;

int anum=0;
struct DataMapArray **aptr=NULL;
char **cdfaname;

struct OptionData opt;
int arg=0;


int loadmap(FILE *fp) {
  char buf[256];
  int off=0;
  int c,x,y,t,dim;

  do {
    off=0;
    do {
      c=fgetc(fp);
      if (c==-1) break;
      if (c==';') break;
      buf[off]=c;
      off++;
      if (off>255) c=-1;
    } while (c !=-1);
    if (c==-1) break;
    buf[off]=0;

    x=0;
    while((!isalnum(buf[x])) && (x<off)) x++;
    y=x;
    while((isalnum(buf[y])) && (y<off)) y++;
    t=0;
    if (strncmp(buf+x,"char",4)==0) t=DATACHAR;
    if (strncmp(buf+x,"short",5)==0) t=DATASHORT;
    if (strncmp(buf+x,"long",4)==0) t=DATAINT;
    if (strncmp(buf+x,"float",5)==0) t=DATAFLOAT;
    if (strncmp(buf+x,"double",6)==0) t=DATADOUBLE;
    if (strncmp(buf+x,"string",6)==0) t=DATASTRING;
    x=y;
    while ((!isalnum(buf[x])) && (x<off)) x++;
    y=x;
    while ((isalnum(buf[y])) && (y<off)) y++;
    buf[y]=0;
    dim=atoi(buf+x);
    x=y;
    while ((buf[x] !='"') && (x<off)) x++;
    y=x+1;
    while ((buf[y] !='"') && (y<off)) y++;
    buf[y]=0;
    if (dim==0) {
      if (sptr==NULL) sptr=malloc(sizeof(struct DataMapScalar *));
      else {
        struct DataMapScalar **tmp;
        tmp=realloc(sptr,(snum+1)*sizeof(struct DataMapScalar *));
        if (tmp==NULL) break;
        sptr=tmp;
      }
      sptr[snum]=DataMapMakeScalar(buf+x+1,0,t,NULL);
      if (cdfsname==NULL) cdfsname=malloc(sizeof(char *));
      else {
        char **tmp;
        tmp=realloc(cdfsname,(snum+1)*sizeof(char *));
        if (tmp==NULL) break;
        cdfsname=tmp;
      }  
      snum++;
    } else {
      if (aptr==NULL) aptr=malloc(sizeof(struct DataMapArray *));
      else {
        struct DataMapArray **tmp;
        tmp=realloc(aptr,(anum+1)*sizeof(struct DataMapArray *));
        if (tmp==NULL) break;
        aptr=tmp;
      }
      if (cdfaname==NULL) cdfaname=malloc(sizeof(char *));
      else {
        char **tmp;
        tmp=realloc(cdfaname,(snum+1)*sizeof(char *));
        if (tmp==NULL) break;
        cdfaname=tmp;
      }  
      aptr[anum]=DataMapMakeArray(buf+x+1,0,t,dim,NULL,NULL);
      anum++;
    }
 
    x=y+1;
    while ((buf[x] !='=') && (x<off)) x++;
    while ((!isalnum(buf[x])) && (x<off)) x++;
    y=x;
    while ((buf[y] !=';') && (y<off)) y++;
    buf[y]=0;
    if (dim==0) {
      cdfsname[snum-1]=malloc(y-x+1);
      strcpy(cdfsname[snum-1],buf+x);
    } else {
      cdfaname[anum-1]=malloc(y-x+1);
      strcpy(cdfaname[anum-1],buf+x);
    }
  } while (c !=-1);
  return 0;
}



int main(int argc,char *argv[]) {
  struct DataMap *ptr;
  struct DataMapScalar *sx,*sy;
  struct DataMapArray *ax,*ay;
  size_t index[256];
  size_t start[256];
  size_t count[256];

  int s;
  unsigned char vbflg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char zflg=0;

  FILE *fp=NULL;
  gzFile zfp=0;

  FILE *mapfp;
  int n,c,x;
  int ncid;
  int block=0;
 
  int varid;
  
  int strsze;
  char **strptr;
  char *tmpbuf=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vbflg);
  OptionAdd(&opt,"z",'x',&zflg);


  if (argc>1) {
    arg=OptionProcess(1,argc,argv,&opt,NULL); 
    if (help==1) {
      OptionPrintInfo(stdout,hlpstr);
      exit(0);
    }
    if (option==1) {
      OptionDump(stdout,&opt);
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


  /* load the map */

  mapfp=fopen(argv[arg+1],"r");
  loadmap(mapfp);
  fclose(mapfp);

 

  s=nc_open(argv[arg+2],NC_WRITE,&ncid);
  if (s !=NC_NOERR) {
    fprintf(stderr,"Error opening CDF file.\n");
    exit(-1);
  }


   


  block=0;
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
      if (n !=snum) { /* mapped variable */
        s=nc_inq_varid(ncid,cdfsname[n],&varid);
        if (s !=NC_NOERR) {
          fprintf(stderr,"Error accessing CDF file.\n");
          exit(-1);
        }
        index[0]=block;
        switch (sx->type) {
        case DATACHAR:
          s=nc_put_var1_text(ncid,varid,index,sx->data.cptr);
          break;
        case DATASHORT:
          s=nc_put_var1_short(ncid,varid,index,sx->data.sptr);
          break;
        case DATAINT:
          s=nc_put_var1_long(ncid,varid,index,(long *) sx->data.iptr);
          break;
        case DATAFLOAT:
          s=nc_put_var1_float(ncid,varid,index,sx->data.fptr);
          break;
        case DATADOUBLE:
          s=nc_put_var1_double(ncid,varid,index,sx->data.dptr);
          break;
        case DATASTRING:
          start[0]=block;
          start[1]=0;
          count[0]=1;
          count[1]=strlen(*((char **) sx->data.vptr))+1;
          s=nc_put_vara_text(ncid,varid,start,count,
                             *((char **) sx->data.vptr));
          break;
	}
        if (s !=NC_NOERR) {
          fprintf(stderr,"Error writing CDF file (%d).\n",s);
          exit(-1);
        }
       
      }
    }

    for (c=0;c<ptr->anum;c++) {
      ax=ptr->arr[c];
      for (n=0;n<anum;n++) {
        ay=aptr[n];
      
        if (strcmp(ax->name,ay->name) !=0) continue;
        if (ax->type !=ay->type) continue;
        if (ax->dim !=ay->dim) continue;
        break;
      }
      if (n !=anum) { /* mapped variable */
      
        s=nc_inq_varid(ncid,cdfaname[n],&varid);
        if (s !=NC_NOERR) {
          fprintf(stderr,"Error accessing CDF file.\n");
          exit(-1);
        }
        start[0]=block;
        count[0]=1;
        n=1;
        for (x=0;x<ax->dim;x++) {
          start[1+x]=0;
          count[1+x]=ax->rng[x];
          n=n*ax->rng[x];
	}

        if (ax->type==DATASTRING) {
          int ndims;
          int dimids[NC_MAX_VAR_DIMS];
          size_t dimlen;
          s=nc_inq_varndims(ncid,varid,&ndims);
          if (s !=NC_NOERR) {
            fprintf(stderr,"Error accessing CDF file.\n");
            exit(-1);
          }
          s=nc_inq_vardimid(ncid,varid,dimids);
          if (s !=NC_NOERR) {
            fprintf(stderr,"Error accessing CDF file.\n");
            exit(-1);
          }
          if (ndims-2!=ax->dim) {
            fprintf(stderr,"Error matching dimensions.\n");
            exit(-1);
	  }
          
          s=nc_inq_dimlen(ncid,dimids[ndims-1],&dimlen);
          if (s !=NC_NOERR) {
            fprintf(stderr,"Error accessing CDF file.\n");
            exit(-1);
          }
          strsze=dimlen;
          tmpbuf=malloc(n*strsze);
          if (tmpbuf==NULL) {
            fprintf(stderr,"Failed to allocate buffer.\n");
            exit(-1);
	  }
          memset(tmpbuf,0,n*strsze);
          start[1+ax->dim]=0;
          count[1+ax->dim]=strsze;
          strptr=(char **) ax->data.vptr;
          for (x=0;x<n;x++) strncpy(tmpbuf+x*strsze,strptr[x],strsze);
	}               

        switch (ax->type) { 
        case DATACHAR:
           s=nc_put_vara_text(ncid,varid,start,count,ax->data.cptr);
           break;
        case DATASHORT:
           s=nc_put_vara_short(ncid,varid,start,count,ax->data.sptr);
           break;
        case DATAINT:
	  s=nc_put_vara_long(ncid,varid,start,count,(long *) ax->data.iptr);
           break;
        case DATAFLOAT:
           s=nc_put_vara_float(ncid,varid,start,count,ax->data.fptr);
           break;
        case DATADOUBLE:
           s=nc_put_vara_double(ncid,varid,start,count,ax->data.dptr);
           break;
        case DATASTRING:
           s=nc_put_vara_text(ncid,varid,start,count,tmpbuf);
	   break;
	}
        if (tmpbuf !=NULL) {
	  free(tmpbuf);
          tmpbuf=NULL;
	}

        if (s !=NC_NOERR) {
          fprintf(stderr,"Error writing CDF file (%d).\n",s);
          exit(-1);
        }
 
      }
    }
  

    DataMapFree(ptr);
    block++;
  }
  nc_close(ncid);
  if (zflg) gzclose(zfp);
  else fclose(fp);
  return 0;
}
