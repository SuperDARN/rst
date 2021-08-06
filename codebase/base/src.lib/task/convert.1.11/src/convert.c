/* convert.c
   =========
   Author: R.J.Barnes
(c) 2011 JHU/APL & Others - Please Consult LICENSE.datamap.txt for more information.                                                                 

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
#include <sys/types.h>
#include <fcntl.h>
#include <zlib.h>
#include <unistd.h>
#include "rtypes.h"



int ConvertBitOrder() {
  int32 test;
  test=1;
  return *((unsigned char *) &test);
}
  

void ConvertToLong(unsigned char *cnv,int64 *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val;
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) out[i]=cnv[7-i];
  else for (i=0;i<8;i++) out[i]=cnv[i];
}

void ConvertToInt(unsigned char *cnv,int32 *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) out[i]=cnv[3-i];
  else for (i=0;i<4;i++) out[i]=cnv[i];
}

void ConvertToShort(unsigned char *cnv,int16 *val) {
  short tmp;
  unsigned char *out;
  int i;
  out=(unsigned char *) &tmp;
  if (ConvertBitOrder()==0) for (i=0;i<2;i++) out[i]=cnv[1-i];
  else for (i=0;i<2;i++) out[i]=cnv[i];
  *val=tmp;
}


void ConvertToULong(unsigned char *cnv,uint64 *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val;
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) out[i]=cnv[7-i];
  else for (i=0;i<8;i++) out[i]=cnv[i];
}

void ConvertToUInt(unsigned char *cnv,uint32 *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) out[i]=cnv[3-i];
  else for (i=0;i<4;i++) out[i]=cnv[i];
}

void ConvertToUShort(unsigned char *cnv,uint16 *val) {
  short tmp;
  unsigned char *out;
  int i;
  out=(unsigned char *) &tmp;
  if (ConvertBitOrder()==0) for (i=0;i<2;i++) out[i]=cnv[1-i];
  else for (i=0;i<2;i++) out[i]=cnv[i];
  *val=tmp;
}

void ConvertToDouble(unsigned char *cnv,double *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val; 
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) out[i]=cnv[7-i];
  else for (i=0;i<8;i++) out[i]=cnv[i];  
}

void ConvertToFloat(unsigned char *cnv,float *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) out[i]=cnv[3-i];
  else for (i=0;i<4;i++) out[i]=cnv[i];
}

void ConvertFromFloat(float val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) cnv[3-i]=in[i];
  else for (i=0;i<4;i++) cnv[i]=in[i];
}

void ConvertFromDouble(double val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) cnv[7-i]=in[i];
  else for (i=0;i<8;i++) cnv[i]=in[i];
}


void ConvertFromLong(int64 val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) cnv[7-i]=in[i];
  else for (i=0;i<8;i++) cnv[i]=in[i];
}


void ConvertFromInt(int32 val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) cnv[3-i]=in[i];
  else for (i=0;i<4;i++) cnv[i]=in[i];
}

void ConvertFromShort(int16 val,unsigned char *cnv) {
  short tmp;
  unsigned char *in;
  int i;
  tmp=val;
  in=(unsigned char *) &tmp;
  if (ConvertBitOrder()==0) for (i=0;i<2;i++) cnv[1-i]=in[i];
  else for (i=0;i<2;i++) cnv[i]=in[i];
}


void ConvertFromULong(uint64 val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) cnv[7-i]=in[i];
  else for (i=0;i<8;i++) cnv[i]=in[i];
}


void ConvertFromUInt(uint32 val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) cnv[3-i]=in[i];
  else for (i=0;i<4;i++) cnv[i]=in[i];
}

void ConvertFromUShort(uint16 val,unsigned char *cnv) {
  short tmp;
  unsigned char *in;
  int i;
  tmp=val;
  in=(unsigned char *) &tmp;
  if (ConvertBitOrder()==0) for (i=0;i<2;i++) cnv[1-i]=in[i];
  else for (i=0;i<2;i++) cnv[i]=in[i];
}



void ConvertBlock(unsigned char *cnv,int *pattern) {
  int i,j,k;
  int bflg;
  unsigned char *ptr;
  unsigned char tmp[8];
  bflg=ConvertBitOrder();
  if (bflg==1) return;
  ptr=cnv;
  for (i=0;pattern[i] !=0;i+=2) {
    if (pattern[i]==1) ptr+=pattern[i+1]; /* character array so ignore */
    else for (j=0;j<pattern[i+1];j++) {
      for (k=0;k<pattern[i];k++) tmp[pattern[i]-1-k]=ptr[k];
      for (k=0;k<pattern[i];k++) ptr[k]=tmp[k];
      ptr+=pattern[i];
    }   
  }
}


int ConvertFreadLong(FILE *fp,int64 *val) {
  unsigned char tmp[8];
  if (fread(tmp,8,1,fp) !=1) return -1;
  ConvertToLong(tmp,val);
  return 0;
}

int ConvertFreadInt(FILE *fp,int32 *val) {
  unsigned char tmp[4];
  if (fread(tmp,4,1,fp) !=1) return -1;
  ConvertToInt(tmp,val);
  return 0;
}

int ConvertFreadShort(FILE *fp,int16 *val) {
  unsigned char tmp[2];
  if (fread(tmp,2,1,fp) !=1) return -1;
  ConvertToShort(tmp,val);
  return 0;
}

int ConvertFreadULong(FILE *fp,uint64 *val) {
  unsigned char tmp[8];
  if (fread(tmp,8,1,fp) !=1) return -1;
  ConvertToULong(tmp,val);
  return 0;
}

int ConvertFreadUInt(FILE *fp,uint32 *val) {
  unsigned char tmp[4];
  if (fread(tmp,4,1,fp) !=1) return -1;
  ConvertToUInt(tmp,val);
  return 0;
}

int ConvertFreadUShort(FILE *fp,uint16 *val) {
  unsigned char tmp[2];
  if (fread(tmp,2,1,fp) !=1) return -1;
  ConvertToUShort(tmp,val);
  return 0;
}

int ConvertFreadDouble(FILE *fp,double *val) {
  unsigned char tmp[8];
  if (fread(tmp,8,1,fp) !=1) return -1;
  ConvertToDouble(tmp,val);
  return 0;
}

int ConvertFreadFloat(FILE *fp,float *val) {
  unsigned char tmp[4];
  if (fread(tmp,4,1,fp) !=1) return -1;
  ConvertToFloat(tmp,val);
  return 0;
}

int ConvertFwriteFloat(FILE *fp,float val) {
  unsigned char tmp[4];
  ConvertFromFloat(val,tmp);
  if (fwrite(tmp,4,1,fp) !=1) return -1;
  return 0;
}

int ConvertFwriteDouble(FILE *fp,double val) {
  unsigned char tmp[8];
  ConvertFromDouble(val,tmp);
  if (fwrite(tmp,8,1,fp) !=1) return -1;
  return 0;
}


int ConvertFwriteLong(FILE *fp,int64 val) {
  unsigned char tmp[8];
  ConvertFromLong(val,tmp);
  if (fwrite(tmp,8,1,fp) !=1) return -1;
  return 0;
}


int ConvertFwriteInt(FILE *fp,int32 val) {
  unsigned char tmp[4];
  ConvertFromInt(val,tmp);
  if (fwrite(tmp,4,1,fp) !=1) return -1;
  return 0;
}

int ConvertFwriteShort(FILE *fp,int16 val) {
  unsigned char tmp[2];
  ConvertFromShort(val,tmp);
  if (fwrite(tmp,2,1,fp) !=1) return -1;
  return 0;
}

int ConvertFwriteULong(FILE *fp,uint64 val) {
  unsigned char tmp[8];
  ConvertFromULong(val,tmp);
  if (fwrite(tmp,8,1,fp) !=1) return -1;
  return 0;
}


int ConvertFwriteUInt(FILE *fp,uint32 val) {
  unsigned char tmp[4];
  ConvertFromUInt(val,tmp);
  if (fwrite(tmp,4,1,fp) !=1) return -1;
  return 0;
}

int ConvertFwriteUShort(FILE *fp,uint16 val) {
  unsigned char tmp[2];
  ConvertFromUShort(val,tmp);
  if (fwrite(tmp,2,1,fp) !=1) return -1;
  return 0;
}

int ConvertReadLong(int fp,int64 *val) {
  unsigned char tmp[8];
  int s=0,o=0,l=8;
  while (o<8) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToLong(tmp,val);
  return 0;
}

int ConvertReadInt(int fp,int32 *val) {
  unsigned char tmp[4];
  int s=0,o=0,l=4;
  while (o<4) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToInt(tmp,val);
  return 0;
}

int ConvertReadShort(int fp,int16 *val) {
  unsigned char tmp[2];
  int s=0,o=0,l=2;
  while (o<2) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToShort(tmp,val);
  return 0;
}




int ConvertReadULong(int fp,uint64 *val) {
  unsigned char tmp[8];
  int s=0,o=0,l=8;
  while (o<8) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToULong(tmp,val);
  return 0;
}

int ConvertReadUInt(int fp,uint32 *val) {
  unsigned char tmp[4];
  int s=0,o=0,l=4;
  while (o<4) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToUInt(tmp,val);
  return 0;
}

int ConvertReadUShort(int fp,uint16 *val) {
  unsigned char tmp[2];
  int s=0,o=0,l=2;
  while (o<2) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToUShort(tmp,val);
  return 0;
}


int ConvertReadDouble(int fp,double *val) {
  unsigned char tmp[8];
  int s=0,o=0,l=8;
  while (o<8) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToDouble(tmp,val);
  return 0;
}

int ConvertReadFloat(int fp,float *val) {
  unsigned char tmp[4];
  
  int s=0,o=0,l=4;
  while (o<4) {
    s=read(fp,tmp+o,l);
    o=o+s;
    l=l-s;
    if (s==0) return -1;
    if (s==-1) return -1;
  }
  ConvertToFloat(tmp,val);
  return 0;
}


int ConvertWriteFloat(int fp,float val) {
  unsigned char tmp[4];
  ConvertFromFloat(val,tmp);
  if (write(fp,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteDouble(int fp,double val) {
  unsigned char tmp[8];
  ConvertFromDouble(val,tmp);
  if (write(fp,tmp,8) !=8) return -1;
  return 0;
}


int ConvertWriteLong(int fp,int64 val) {
  unsigned char tmp[8];
  ConvertFromLong(val,tmp);
  if (write(fp,tmp,8) !=8) return -1;
  return 0;
}


int ConvertWriteInt(int fp,int32 val) {
  unsigned char tmp[4];
  ConvertFromInt(val,tmp);
  if (write(fp,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteShort(int fp,int16 val) {
  unsigned char tmp[2];
  ConvertFromShort(val,tmp);
  if (write(fp,tmp,2) !=2) return -1;
  return 0;
}



int ConvertWriteULong(int fp,uint64 val) {
  unsigned char tmp[8];
  ConvertFromULong(val,tmp);
  if (write(fp,tmp,8) !=8) return -1;
  return 0;
}


int ConvertWriteUInt(int fp,uint32 val) {
  unsigned char tmp[4];
  ConvertFromUInt(val,tmp);
  if (write(fp,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteUShort(int fp,uint16 val) {
  unsigned char tmp[2];
  ConvertFromUShort(val,tmp);
  if (write(fp,tmp,2) !=2) return -1;
  return 0;
}

int ConvertReadLongZ(gzFile file,int64 *val) {
  unsigned char tmp[8];
  if (gzread(file,tmp,8) !=8) return -1;
  ConvertToLong(tmp,val);
  return 0;
}

int ConvertReadIntZ(gzFile file,int32 *val) {
  unsigned char tmp[4];
  if (gzread(file,tmp,4) !=4) return -1;
  ConvertToInt(tmp,val);
  return 0;
}

int ConvertReadShortZ(gzFile file,int16 *val) {
  unsigned char tmp[2];
  if (gzread(file,tmp,2) !=2) return -1;
  ConvertToShort(tmp,val);
  return 0;
}


int ConvertReadULongZ(gzFile file,uint64 *val) {
  unsigned char tmp[8];
  if (gzread(file,tmp,8) !=8) return -1;
  ConvertToULong(tmp,val);
  return 0;
}

int ConvertReadUIntZ(gzFile file,uint32 *val) {
  unsigned char tmp[4];
  if (gzread(file,tmp,4) !=4) return -1;
  ConvertToUInt(tmp,val);
  return 0;
}

int ConvertReadUShortZ(gzFile file,uint16 *val) {
  unsigned char tmp[2];
  if (gzread(file,tmp,2) !=2) return -1;
  ConvertToUShort(tmp,val);
  return 0;
}

int ConvertReadDoubleZ(gzFile file,double *val) {
  unsigned char tmp[8];
  if (gzread(file,tmp,8) !=8) return -1;
  ConvertToDouble(tmp,val);
  return 0;
}

int ConvertReadFloatZ(gzFile file,float *val) {
  unsigned char tmp[4];
  if (gzread(file,tmp,4) !=4) return -1;
  ConvertToFloat(tmp,val);
  return 0;
}

int ConvertWriteFloatZ(gzFile file,float val) {
  unsigned char tmp[4];
  ConvertFromFloat(val,tmp);
  if (gzwrite(file,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteDoubleZ(gzFile file,double val) {
  unsigned char tmp[8];
  ConvertFromDouble(val,tmp);
  if (gzwrite(file,tmp,8) !=8) return -1;
  return 0;
}


int ConvertWriteLongZ(gzFile file,int64 val) {
  unsigned char tmp[8];
  ConvertFromLong(val,tmp);
  if (gzwrite(file,tmp,8) !=8) return -1;
  return 0;
}


int ConvertWriteIntZ(gzFile file,int32 val) {
  unsigned char tmp[4];
  ConvertFromInt(val,tmp);
  if (gzwrite(file,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteShortZ(gzFile file,int16 val) {
  unsigned char tmp[2];
  ConvertFromShort(val,tmp);
  if (gzwrite(file,tmp,2) !=2) return -1;
  return 0;
}

int ConvertWriteULongZ(gzFile file,uint64 val) {
  unsigned char tmp[8];
  ConvertFromULong(val,tmp);
  if (gzwrite(file,tmp,8) !=8) return -1;
  return 0;
}

int ConvertWriteUIntZ(gzFile file,uint32 val) {
  unsigned char tmp[4];
  ConvertFromUInt(val,tmp);
  if (gzwrite(file,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteUShortZ(gzFile file,uint16 val) {
  unsigned char tmp[2];
  ConvertFromUShort(val,tmp);
  if (gzwrite(file,tmp,2) !=2) return -1;
  return 0;
}
