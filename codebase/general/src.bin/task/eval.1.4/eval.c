/* eval.c
   ====== */

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"

#include "errstr.h"
#include "hlpstr.h"

#include "reval.h"

#include "rmath.h"

struct OptionData opt;
int arg=0;




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
  


char *varlist[]={
  "PI",0
};


void decode_format(char *txt) {
  int i=0;
  int j=0;
  for (i=0;txt[i] !=0;i++) {
    if (txt[i] !='\\') {
      if (j !=i) txt[j]=txt[i];
      j++;
      continue;
    }
    if (txt[i+1]==0) break;
    if (txt[i+1]=='n') txt[j]='\n';
    if (txt[i+1]=='t') txt[j]='\t';
    if (txt[i+1]=='\"') txt[j]='\"';
    if (txt[i+1]=='\\') txt[j]='\\';
    j++;
    i++;
  }
  txt[j]=0;
}

int decode_value(char *ptr,double *value,void *data) {
  
  int i; 
  double *store;
   
  store=(double *)data;

  if ((isdigit(ptr[0])) || (ptr[0]=='.')) {
     *value=atof(ptr);
     return 0;
   }
 
  for (i=0;varlist[i] !=0;i++) if (strcmp(ptr,varlist[i])==0) break;
  *value=store[i];
  return 0;
}


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

int main(int argc,char *argv[]) {
  unsigned char help=0;
  unsigned char option=0;

  int s;
  double store[2];
  double val;
  char *cast=NULL;
  char *format=NULL;
  char *dformat="%lg\n";
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"c",'t',&cast);
  OptionAdd(&opt,"f",'t',&format);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if (arg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(0);
  }
  if (format==NULL) format=dformat;
  else decode_format(format);
  store[0]=PI;
  s=Eval(argv[arg],&val,decode_value,store,decode_function,store);
  if (cast==NULL) fprintf(stdout,format,val);
  else if (cast[0]=='c') fprintf(stdout,format,(unsigned char) val);
  else if (cast[0]=='u') fprintf(stdout,format,(unsigned int) val);
  else if (cast[0]=='i') fprintf(stdout,format,(int) val);
  else if (cast[0]=='d') fprintf(stdout,format,val);
  return 0;
} 







