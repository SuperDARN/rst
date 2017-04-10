/* reval.h
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _REVAL_H
#define _REVAL_H

struct EvalStack {
  int num;
  unsigned char *buf;
};

struct EvalPostFix {
  int num;
  char *type;
  int  *object;

  int vnum;
  double *value;
};

int Eval(char *sum,double *ptr,
         int (*dvar)(char *ptr,double *val,void *data),
         void *vdata,
         int (*dfun)(char *ptr,int argnum,double *argptr,
                     double *val,void *data),
         void *fdata);

#endif
