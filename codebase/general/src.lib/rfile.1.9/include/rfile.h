/* rfile.h
   ==========
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




#ifndef _RFILE_H
#define _RFILE_H

#define LMAX 4096

#define FIELD_TXT 0
#define FIELD_CHAR 1
#define FIELD_DOUBLE 2
#define FIELD_FLOAT 3

#define FILE_STEP 64

struct RfileIndex {
  double st_time,ed_time;
  int num;
  struct {
    double *start;
    double *end;
  } time;
  int *offset;
};


struct RfileData {
  int type;
  union {
    char *tval;
    char cval;
    float fval;
    double dval;
    int32 ival;
    int16 sval;
    int32 lval;
  } data;
};


typedef int (*RfileDecoder)(char *,char *,char *,double,double,int,int,int,
                            struct RfileData *,void *ptr);


int RfileRead(FILE *fp,int num,RfileDecoder *decode,void **ptr);
struct RfileIndex *RfileLoadIndex(FILE *fp);

int RfileSeek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,double sc,
              struct RfileIndex *inx,
              double *aval);



int RfileReadRaw(FILE *fp,int num,RfileDecoder *decode,void **ptr);



#endif





