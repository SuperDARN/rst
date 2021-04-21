/* idlsave.c
   ========= 
   Author R.J.Barnes
*/

/*
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "dmap.h"
#include "idlsave.h"

#define START_MARKER 0
#define COMMON_VARIABLE 1
#define VARIABLE 2 
#define SYSTEM_VARIABLE 3
#define END_MARKER 6
#define TIMESTAMP 10
#define COMPILED 12
#define IDENTIFICATION 13
#define VERISON 14
#define HEAP_HEADER 15
#define HEAP_DATA 16
#define PROMOTE64 17
#define NOTICE 19

#define UNDEFINED_TYP 0
#define BYTE_TYP 1
#define INT_TYP 2
#define LONG_TYP 3 
#define FLOAT_TYP 4 
#define DOUBLE_TYP 5
#define COMPLEX_TYP 6
#define STRING_TYP 7
#define STRUCT_TYP 8 
#define DCOMPLEX_TYP 9 
#define PTR_TYP 10
#define OBJ_TYP 11 
#define UINT_TYP 12 
#define ULONG_TYP 13
#define LONG64_TYP 14
#define ULONG64_TYP 15

struct IDLComplex {
  float x;
  float y;
};

struct IDLDComplex {
  double x;
  double y;
};

struct IDLByte {
  int32 len;
  char *buf;
};

struct IDLString {
  char *txt;
  int32 len;
};

struct RHDR {
  uint32 nextrec0;
  uint32 nextrec1;
  uint32 pad;
};

struct RHDR64 {
  uint64 nextrec;
  uint32 pad[2];
};


union IDLVar {
  struct IDLByte b;
  struct IDLString s;
  int16 i;
  int32 l;
  int32 p;
  float f;
  double d;
  struct IDLComplex c;
  struct IDLDComplex dc;
  uint16 ui;
  uint32 ul;
  int64 l64;
  uint64 ul64;
};

struct IDLFile *IDLOpen(char *fname,char zflg) {
  struct IDLFile *fp=NULL;
  fp=malloc(sizeof(struct IDLFile));
  fp->zflg=zflg;
  if (zflg) {
    fp->fp.z=gzopen(fname,"r");
    if (fp->fp.z==NULL) {
      free(fp);
      return NULL;
    }
  } else {
    fp->fp.p=fopen(fname,"r");
    if (fp->fp.p==NULL) {
      free(fp);
      return NULL;
    }
  }
  return fp;
}

struct IDLFile *IDLFdopen(int fd,char zflg) {
  struct IDLFile *fp=NULL;
  fp=malloc(sizeof(struct IDLFile));
  fp->zflg=zflg;
  if (zflg) {
    fp->fp.z=gzdopen(fd,"r");
    if (fp->fp.z==NULL) {
      free(fp);
      return NULL;
    }
  } else {
    fp->fp.p=fdopen(fd,"r");
    if (fp->fp.p==NULL) {
      free(fp);
      return NULL;
    }
  }
  return fp;
}

void IDLClose(struct IDLFile *fp) {
  if (fp==NULL) return;
  if (fp->zflg) gzclose(fp->fp.z); 
  else fclose(fp->fp.p);
  free(fp);
}

int IDLRead(struct IDLFile *fp,char * buf,int len) {
  int s;
  if (fp->zflg) s=gzread(fp->fp.z,buf,len);
  else s=fread(buf,1,len,fp->fp.p);
  if (s !=len) return -1;
  return s;
}

int IDLSeek(struct IDLFile *fp,int offset,int whence) {
  if (fp->zflg) return gzseek(fp->fp.z,offset,whence);
  return fseek(fp->fp.p,offset,whence); 
}

void IDLFreeVariable(int32 typecode,union IDLVar *val) {
  switch (typecode) {
  case BYTE_TYP:
    if (val->b.buf !=NULL) free (val->b.buf);
    break;
  case STRING_TYP:
    if (val->s.txt !=NULL) free(val->s.txt);
  }
}

uint32 IDLConvert32(uint32 inp) {
  int i;
  uint32 out;
  unsigned char *ip,*op;
  ip=(unsigned char *) &inp;
  op=(unsigned char *) &out;
  if (ConvertBitOrder() !=0) for (i=0;i<4;i++) op[i]=ip[3-i];
  else out=inp;
  return out;
}

uint32 IDLConvert64(uint64 inp) {
  int i;
  uint64 out;
  unsigned char *ip,*op;
  ip=(unsigned char *) &inp;
  op=(unsigned char *) &out;
  if (ConvertBitOrder() !=0) for (i=0;i<8;i++) op[i]=ip[7-i];
  else out=inp;
  return out;
}



int IDLRead_PTR(struct IDLFile *fp,int32 *val) {
  int s;
  uint32 tmp; 
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  *val=(int32) IDLConvert32(tmp); 
  return 0;
}

int IDLRead_BYTE(struct IDLFile *fp,struct IDLByte *val) {
  int s;
  char *tptr;
  div_t dv;
  int32 pad;
  uint32 tmp;
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  val->len=(int32) IDLConvert32(tmp);
  dv=div(val->len,4);
  pad=(dv.rem !=0) ? (4-dv.rem) :0;
  if (val->len==0) {
    free(val->buf);
    return 0;
  }
  tptr=realloc(val->buf,val->len); 
  if (tptr==NULL) return -1;
  val->buf=tptr; 
  s=IDLRead(fp,val->buf,val->len);
  s=IDLSeek(fp,pad,SEEK_CUR); 
 
  return s;
}

int IDLRead_STRING(struct IDLFile *fp,struct IDLString *val) {
  int s;
  char *tptr;
  div_t dv;
  int32 pad;
  uint32 tmp;
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  val->len=(int32) IDLConvert32(tmp);
  if (val->len==0) {
    if (val->txt !=NULL) free(val->txt);
    val->txt=NULL;
    return s;
  }
  dv=div(val->len,4);
  pad=(dv.rem !=0) ? (4-dv.rem) :0;
  tptr=realloc(val->txt,val->len+1);
  if (tptr==NULL) return -1;
  val->txt=tptr; 
  s=IDLRead(fp,val->txt,val->len);
  val->txt[val->len]=0;
  s=IDLSeek(fp,pad,SEEK_CUR); 
  return s;
}


int IDLRead_INT(struct IDLFile *fp,int16 *val) {
  int s;
  uint32 tmp; 
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  *val=(int16) IDLConvert32(tmp); 
  return 0;
}

int IDLRead_UINT(struct IDLFile *fp,uint16 *val) {
  int s;
  uint32 tmp; 
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  *val=(uint16) IDLConvert32(tmp); 
  return 0;
}

int IDLRead_LONG(struct IDLFile *fp,int32 *val) {
  int s;
  uint32 tmp; 
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  *val=(int32) IDLConvert32(tmp); 
  return 0;
}

int IDLRead_ULONG(struct IDLFile *fp,uint32 *val) {
  int s;
  uint32 tmp; 
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  *val=(uint32) IDLConvert32(tmp); 
  return 0;
}



int IDLRead_LONG64(struct IDLFile *fp,int64 *val) {
  int s;
  uint64 tmp; 
  s=IDLRead(fp, (char *) &tmp,8);
  if (s==-1) return -1;
  *val=(int64) IDLConvert64(tmp); 
  return 0;
}

int IDLRead_ULONG64(struct IDLFile *fp,uint64 *val) {
  int s;
  uint64 tmp; 
  s=IDLRead(fp, (char *) &tmp,8);
  if (s==-1) return -1;
  *val=(uint64) IDLConvert64(tmp); 
  return 0;
}

int IDLRead_FLOAT(struct IDLFile *fp,float *val ) {
  int s;
  uint32 tmp;
  s=IDLRead(fp, (char *) &tmp,4);
  if (s==-1) return -1;
  *((uint32 *) val)=IDLConvert32(tmp); 
  return 0; 
}

int IDLRead_DOUBLE(struct IDLFile *fp,double *val ) {
  int s;
  uint64 tmp;
  s=IDLRead(fp, (char *) &tmp,8);
  if (s==-1) return -1;
  *((uint64 *) val)=IDLConvert64(tmp); 
  return 0; 
}

int IDLRead_COMPLEX(struct IDLFile *fp,struct IDLComplex *val ) {
  int s;
  s=IDLRead_FLOAT(fp,&val->x);
  if (s==-1) return -1;
  s=IDLRead_FLOAT(fp,&val->y);
  return s;
}

int IDLRead_DCOMPLEX(struct IDLFile *fp,struct IDLDComplex *val ) {
  int s;
  s=IDLRead_DOUBLE(fp,&val->x);
  if (s==-1) return -1;
  s=IDLRead_DOUBLE(fp,&val->y);
  return s;
}

int IDLReadData(struct IDLFile *fp,int32 typecode,union IDLVar *val) {
  int s=0;
  int32 tmp;
  switch (typecode) {
  case BYTE_TYP:
    s=IDLRead_BYTE(fp,&val->b);
    break;
  case INT_TYP:
    s=IDLRead_INT(fp,&val->i);
    break;
  case LONG_TYP:
    s=IDLRead_LONG(fp,&val->l);
    break;
  case FLOAT_TYP:
    s=IDLRead_FLOAT(fp,&val->f);
    break;
  case DOUBLE_TYP:
   s=IDLRead_DOUBLE(fp,&val->d);
    break;
  case COMPLEX_TYP:
    s=IDLRead_COMPLEX(fp,&val->c);
    break;
  case STRING_TYP:
    s=IDLRead_LONG(fp,&tmp);
    if (tmp==0) {
      val->s.len=0;
      if (val->s.txt !=NULL) free(val->s.txt);
      val->s.txt=NULL;
      return s;
    }
    s=IDLRead_STRING(fp,&val->s);
    break;
  case STRUCT_TYP:
    break;
  case DCOMPLEX_TYP:
    s=IDLRead_DCOMPLEX(fp,&val->dc);
    break;
  case PTR_TYP:
    s=IDLRead_PTR(fp,&val->p);
    break;
  case OBJ_TYP:
    break;
  case UINT_TYP:
    s=IDLRead_UINT(fp,&val->ui);
    break;
  case ULONG_TYP:
    s=IDLRead_ULONG(fp,&val->ul);
    break;
  case LONG64_TYP:
    s=IDLRead_LONG64(fp,&val->l64);
    break;
  case ULONG64_TYP:
    s=IDLRead_ULONG64(fp,&val->ul64);
    break;
  }
  return s;
}

int IDLAddScalar(struct IDLFile *fp,
                 struct DataMap *dmap,struct IDLString *vname,
                 int32 typecode) {
  union IDLVar val;
  int32 lpad;
  char *tmp=NULL;
  int n,s;

  memset(&val,0,sizeof(union IDLVar));

  s=IDLRead_LONG(fp,&lpad);
  if (s==-1) return -1; 
  if (lpad !=0x07) return -1;

  s=IDLReadData(fp,typecode,&val);    
  if (s==-1) return -1;

  switch (typecode) {
  case BYTE_TYP:
    if (val.b.len==1) 
      DataMapStoreScalar(dmap,vname->txt,DATACHAR,&val.b.buf[0]);
    break;
  case INT_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATASHORT,&val.i);
    break;
  case LONG_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATAINT,&val.l);
    break;
  case FLOAT_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATAFLOAT,&val.f);
    break;
  case DOUBLE_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATADOUBLE,&val.d);
    break;
  case COMPLEX_TYP:
    tmp=malloc(strlen(vname->txt)+3);
    for (n=0;n<2;n++) {
      strcpy(tmp,vname->txt);
      if (n==0) {
        strcat(tmp,".x");
        DataMapStoreScalar(dmap,tmp,DATAFLOAT,&val.c.x);
      } else {
        strcat(tmp,".y");
        DataMapStoreScalar(dmap,tmp,DATAFLOAT,&val.c.y);
      }
    }
    free(tmp);
    break;
  case STRING_TYP:
    if (val.s.len !=0) {
      tmp=malloc(val.s.len+1);
      memcpy(tmp,val.s.txt,val.s.len+1);   
    } else tmp=NULL; 
    DataMapStoreScalar(dmap,vname->txt,DATASTRING,&tmp);
    break;
  case STRUCT_TYP:
    /* not implemented */
    break;
  case DCOMPLEX_TYP:
    tmp=malloc(strlen(vname->txt)+3);
    for (n=0;n<2;n++) {
      strcpy(tmp,vname->txt);
      if (n==0) {
        strcat(tmp,".x");
        DataMapStoreScalar(dmap,tmp,DATADOUBLE,&val.dc.x);
      } else {
        strcat(tmp,".y");
        DataMapStoreScalar(dmap,tmp,DATADOUBLE,&val.dc.y);
      }
    }
    free(tmp);
    break;
  case PTR_TYP:
    /* not implemented */
    break;
  case OBJ_TYP:
    /* not implemented */
    break;
  case UINT_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATAUSHORT,&val.ui);
    break;
  case ULONG_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATAINT,&val.ul);
    break;
  case LONG64_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATALONG,&val.l64);
    break;
  case ULONG64_TYP:
    DataMapStoreScalar(dmap,vname->txt,DATAULONG,&val.ul64);
    break;
  }
  IDLFreeVariable(typecode,&val);
  return 0; 
}

int IDLAddArray(struct IDLFile *fp,
                struct DataMap *dmap,struct IDLString *vname,int32 typecode) {
  char *tmp=NULL;
  char *csuf[2]={".x",".y"};
  void *ptr[2]={NULL,NULL};
  int n,k,s;
  union IDLVar val;

  int32 lpad,nbytes,nelements,ndims,nmax;
  int32 *dims=NULL;

  memset(&val,0,sizeof(union IDLVar));

  s=IDLRead_LONG(fp,&lpad);
  if (lpad !=0x08) return -1;
  s=IDLRead_LONG(fp,&lpad);
  if (s==-1) return -1;
  s=IDLRead_LONG(fp,&nbytes);
  if (s==-1) return -1;
  s=IDLRead_LONG(fp,&nelements);
  if (s==-1) return -1;
  s=IDLRead_LONG(fp,&ndims);
  if (s==-1) return -1;
  s=IDLRead_LONG(fp,&lpad);
  if (s==-1) return -1;
  s=IDLRead_LONG(fp,&lpad);
  if (s==-1) return -1;
  s=IDLRead_LONG(fp,&nmax);
  if (s==-1) return -1;

  dims=malloc(sizeof(int32)*ndims);

  if (dims==NULL) return -1;

  for (n=0;n<nmax;n++) {
    if (n<ndims) s=IDLRead_LONG(fp,&dims[n]);
    else s=IDLRead_LONG(fp,&lpad);
    if (s==-1) break;
  }
  if (n !=nmax) {
    free(dims);
    return -1;
  }

  s=IDLRead_LONG(fp,&lpad);
  if (lpad !=0x07) {
    free(dims);
    return -1;
  } 
 
  switch (typecode) {
  case BYTE_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATACHAR,ndims,dims,NULL);
    break;
  case INT_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATASHORT,ndims,dims,NULL);
    break;
  case LONG_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATAINT,ndims,dims,NULL);
    break;
  case FLOAT_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATAFLOAT,ndims,dims,NULL);
    break;
  case DOUBLE_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATADOUBLE,ndims,dims,NULL);
    break;
  case COMPLEX_TYP:
    tmp=malloc(strlen(vname->txt)+3);
    for (k=0;k<2;k++) {
      strcpy(tmp,vname->txt);
      strcat(tmp,csuf[k]);
      ptr[k]=DataMapStoreArray(dmap,tmp,DATAFLOAT,ndims,dims,NULL);    
    }
    free(tmp);
    break;
  case STRING_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATASTRING,ndims,dims,NULL);
    break;
  case STRUCT_TYP:
    /* not implemented */
    break;
  case DCOMPLEX_TYP:
    tmp=malloc(strlen(vname->txt)+3);
    for (k=0;k<2;k++) {
      strcpy(tmp,vname->txt);
      strcat(tmp,csuf[k]);
       ptr[k]=DataMapStoreArray(dmap,tmp,DATADOUBLE,ndims,dims,NULL);
    } 
    free(tmp);
    break;
  case PTR_TYP:
    /* not implemented */
    break;
  case OBJ_TYP:
    /* not implemented */
    break;
  case UINT_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATAUSHORT,ndims,dims,NULL);
    break;
  case ULONG_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATAUINT,ndims,dims,NULL);
    break;
  case LONG64_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATALONG,ndims,dims,NULL);
    break;
  case ULONG64_TYP:
    ptr[0]=DataMapStoreArray(dmap,vname->txt,DATAULONG,ndims,dims,NULL);
    break;  
  }

  if (typecode==BYTE_TYP) {
    s=IDLReadData(fp,typecode,&val);
    if (s==-1) {
      free (dims);
      return -1;
    }
    for (n=0;n<nelements;n++) ((char *) ptr)[n]=val.b.buf[n];
  } else {
    for (n=0;n<nelements;n++) {
      s=IDLReadData(fp,typecode,&val);
      if (s==-1) break;
      switch (typecode) {
      case INT_TYP:
	((int16 *) ptr[0])[n]=val.i;
        break;
      case LONG_TYP:
	((int32 *) ptr[0])[n]=val.l;
        break;
      case FLOAT_TYP:
	((float *) ptr[0])[n]=val.f;
        break;
      case DOUBLE_TYP:
	((double *) ptr[0])[n]=val.d;
        break;
      case COMPLEX_TYP:
	((float *) ptr[0])[n]=val.c.x;
	((float *) ptr[1])[n]=val.c.y;  
        break;
      case STRING_TYP:
        ((char **) ptr[0])[n]=NULL;
	if (val.s.len !=0) {
          tmp=malloc(val.s.len+1);
          memcpy(tmp,val.s.txt,val.s.len+1);
          ((char **) ptr[0])[n]=tmp;
	}
        break;
      case STRUCT_TYP:
        break;
      case DCOMPLEX_TYP:   
        ((double *) ptr[0])[n]=val.c.x;
	((double *) ptr[1])[n]=val.c.y;
        break;
      case PTR_TYP:
        /* not implemented */
        break;
      case OBJ_TYP:
        /* not implemented */
        break;
      case UINT_TYP:
	((uint16 *) ptr[0])[n]=val.ui;
        break;
      case ULONG_TYP:
	((uint32 *) ptr[0])[n]=val.ul;
        break;
      case LONG64_TYP:
	((int64 *) ptr[0])[n]=val.l64;
	break;
      case ULONG64_TYP:
        ((uint64 *) ptr[0])[n]=val.ul64;
        break;
      }
    } 
    if (n !=nelements) {
      free(dims);
      IDLFreeVariable(typecode,&val);
      return -1;
    }
  }
  free(dims);
  IDLFreeVariable(typecode,&val);
  return 0; 
}

struct DataMap *IDLReadSave(struct IDLFile *fp) {
  int s,c;
  struct DataMap *dmap=NULL;

  char tmp[256];  
  char sig[4]={'S','R',0,4};

  int p64flg=0;  
  int32 recid;
  uint64 fnext;
  struct RHDR64 hdr64;
  struct RHDR hdr;
  struct IDLString vname={NULL,0};
  int32 typecode=0; 
  int32 varflags=0;

  if (fp==NULL) return NULL;

  s=IDLRead(fp,tmp,4);
  for (c=0;(c<4) && (tmp[c]==sig[c]);c++);
  if (c !=4) return NULL;

  dmap=DataMapMake(); 

  while(1) {
    s=IDLRead_LONG(fp,&recid);
    if (s==-1) break;
    fnext=0;
    switch (recid) {
    case PROMOTE64:
      s=IDLRead(fp, (char *) &hdr64,sizeof(struct RHDR64));
      if (s==-1) break;
      fnext=IDLConvert64(hdr64.nextrec);        
      p64flg=1;
      break;
    default:
      if (p64flg==0) {
        s=IDLRead(fp, (char *) &hdr,sizeof(struct RHDR));
        if (s==-1) break;
        fnext=IDLConvert32(hdr.nextrec0);    
      } else {
        s=IDLRead(fp, (char *) &hdr64,sizeof(struct RHDR64));
        if (s==-1) break;
        fnext=IDLConvert64(hdr64.nextrec);        
      }
      break;
    }  
    if (fnext==0) break;

    switch (recid) {
    case VARIABLE :
      s=IDLRead_STRING(fp,&vname);
      if (s==-1) break;
      s=IDLRead_LONG(fp,&typecode); 
      if (s==-1) break;
      s=IDLRead_LONG(fp,&varflags);
      if (s==-1) break;

      if ((varflags & 0x04)==0) s=IDLAddScalar(fp,dmap,&vname,typecode);
      else if ((varflags & 0x4) !=0) 
        s=IDLAddArray(fp,dmap,&vname,typecode);
      if (s==-1) break; 	
    }
    s=IDLSeek(fp,(long) fnext,SEEK_SET);
    if (s==-1) break;
  }
  return dmap;
}


