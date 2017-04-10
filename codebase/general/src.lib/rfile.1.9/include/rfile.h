/* rfile.h
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
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





