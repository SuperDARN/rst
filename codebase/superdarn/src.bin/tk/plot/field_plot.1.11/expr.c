/* expr.c
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "reval.h"
#include "radar.h" 
#include "scandata.h"




struct edata {
  struct RadarBeam *ptr;
  int rng;
};

char *varlist[]={
  "rng",
  "beam",
  "channel",
  "cpid",
  "scan",
  "nave",
  "noise",
  "freq",
  "atten",
  "gsct",
  "v",
  "v_e",
  "w_l",
  "w_l_e",
  "p_l",
  "p_l_e",
  0
};


char *fnlist[]={
  "abs",
  "acos",
  "acosh",
  "asin",
  "asinh",
  "atan",
  "atan2",
  "atanh",
  "ceil",
  "cos",
  "cosh",
  "exp",
  "floor",
  "int",
  "log",
  "log10",
  "sin",
  "sinh",
  "sqrt",
  "tan",
  "tanh",
  0};
  


int decode_function(char *ptr,int argnum,double *argptr,
                    double *value,void *data) {


  int i; 
 
  for (i=0;fnlist[i] !=0;i++) if (strcmp(ptr,fnlist[i])==0) break;

  switch(i) {
    case 0:
      *value=fabs(argptr[0]);
      break;
    case 1: 
      *value=acos(argptr[0]);
      break;
    case 2: 
      *value=acosh(argptr[0]);
      break;
    case 3: 
      *value=asin(argptr[0]);
      break;
    case 4: 
      *value=asinh(argptr[0]);
      break;
    case 5: 
      *value=atan(argptr[0]);
      break;
    case 6: 
      *value=atan2(argptr[0],argptr[1]);
      break;
    case 7: 
      *value=atanh(argptr[0]);
      break;
   case 8: 
      *value=ceil(argptr[0]);
      break;
   case 9: 
      *value=cos(argptr[0]);
      break;
  case 10: 
      *value=cosh(argptr[0]);
      break;
  case 11: 
      *value=exp(argptr[0]);
      break;
  case 12: 
      *value=floor(argptr[0]);
      break;
  case 13: 
      *value=(int) argptr[0];
      break;
  case 14: 
      *value=log(argptr[0]);
      break;
  case 15: 
      *value=log10(argptr[0]);
      break;
  case 16: 
      *value=sin(argptr[0]);
      break;
  case 17: 
      *value=sinh(argptr[0]);
      break;
  case 18: 
      *value=sqrt(argptr[0]);
      break;
  case 19: 
      *value=tan(argptr[0]);
      break;
  case 20: 
      *value=tanh(argptr[0]);
      break;
      
  }
  return 0;
}

int decode_value(char *ptr,double *value,void *data) {
  
  int i; 
  struct edata *e;
  e=(struct edata *)data;

 
  if ((isdigit(ptr[0])) || (ptr[0]=='.')) {
     *value=atof(ptr);
     return 0;
   }
 
  for (i=0;varlist[i] !=0;i++) if (strcmp(ptr,varlist[i])==0) break;

  switch (i) {
  case 0:
    *value=e->rng;
    break;
  case 1:
    *value=(e->ptr)->bm;
    break;
  case 2:
    *value=(e->ptr)->channel;
    break;
  case 3:
    *value=(e->ptr)->cpid;
    break;
  case 4:
    *value=(e->ptr)->scan;
    break;
  case 5:
    *value=(e->ptr)->nave;
    break;
  case 6:
    *value=(e->ptr)->noise;
    break;
  case 7:
    *value=(e->ptr)->freq;
    break;
  case 8:
    *value=(e->ptr)->atten;
    break;
  case 9:
    *value=(e->ptr)->rng[e->rng].gsct;
    break;
  case 10:
    *value=(e->ptr)->rng[e->rng].v;
    break;
  case 11:
    *value=(e->ptr)->rng[e->rng].v_e;
    break;
  case 12:
    *value=(e->ptr)->rng[e->rng].w_l;
    break;
  case 13:
    *value=(e->ptr)->rng[e->rng].w_l_e;
    break;
  case 14:
    *value=(e->ptr)->rng[e->rng].p_l;
    break;
  case 15:
    *value=(e->ptr)->rng[e->rng].p_l_e;
    break;

  } 
  return 0;
}


double eval_expr(char *expr,
                 struct RadarBeam *ptr,int rng) { 
  int s;
  double value=0;

  struct edata data;

  data.ptr=ptr;
  data.rng=rng;
 
  s=Eval(expr,&value,decode_value,&data,decode_function,&data);
  /* should report an error in here */
  return value;
  

}

