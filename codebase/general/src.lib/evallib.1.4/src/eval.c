/* eval.c
   ====== 
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "reval.h"



char *operator[]={"==","!=","<=",">=",
                  "||","&&",
                  "|","&","^",
                  "+","-","/","*","%",
                  "!","<",">",0};
char *opcode={"enlgoa|&^+-/*%!<>"};



/* NOTE - The postfix stack uses three arrays. 
 * The first identifies the type of object on the stack, 
 * either an operator or an operand.
 * The second contains the numeric ID of the operator
 * or the index of the operand. 
 * The final array contains the actual operands.
 * The index into the operand array is actually redundant
 * as the last entry of the operand array will always
 * correspond to the top operand put on the stack. 
 * Its just a convenient way of referencing things.
 */


int EvalPushPostfix(struct EvalPostFix *ptr,int type,void *data) {
  if (ptr->num==0) {
    ptr->type=malloc(1);
    ptr->object=malloc(sizeof(int));
  } else {
    ptr->type=realloc(ptr->type,ptr->num+1);
    ptr->object=realloc(ptr->object,(ptr->num+1)*sizeof(int));
  }
  ptr->type[ptr->num]=type;
  if (type !=0) ptr->object[ptr->num]=*((int *) data);
  else {
    if (ptr->vnum==0) ptr->value=malloc(sizeof(double));
    else ptr->value=realloc(ptr->value,(ptr->vnum+1)*sizeof(double));
    ptr->value[ptr->vnum]=*( (double *) data);
    ptr->object[ptr->num]=ptr->vnum;
    ptr->vnum++;
  }
  ptr->num++;
  return 0;
}

int EvalPopPostFix(struct EvalPostFix *ptr,int *type,void *data) {
  if (ptr->num==0) return -1;
  ptr->num--;

  *type=ptr->type[ptr->num];
  if (*type !=0) *((int *) data)=ptr->object[ptr->num];
  else {
    *((double *) data)=ptr->value[ptr->object[ptr->num]];
    ptr->vnum--;
    if (ptr->vnum>0) ptr->value=realloc(ptr->value,ptr->vnum*sizeof(double));
    else free(ptr->value);
  }
  if (ptr->num>0) {
    ptr->type=realloc(ptr->type,ptr->num);
    ptr->object=realloc(ptr->object,sizeof(int)*ptr->num);
  } else {
    free(ptr->type);
    free(ptr->object);
  }
  return 0;
}

int EvalPush(struct EvalStack *ptr,unsigned char c) {
  if (ptr->num==0) ptr->buf=malloc(1);
  else ptr->buf=realloc(ptr->buf,ptr->num+1);
  ptr->buf[ptr->num]=c;
  ptr->num++;
  return 0;
}


int EvalPop(struct EvalStack *ptr) {
  int c;
  if (ptr->num==0) return -1;
  ptr->num--;
  c=ptr->buf[ptr->num];
  if (ptr->num>0) ptr->buf=realloc(ptr->buf,ptr->num);
  else free(ptr->buf);
  return c;
}

int EvalTop(struct EvalStack *ptr) {
  if (ptr->num==0) return -1; 
  return ptr->buf[ptr->num-1];
}

int EvalPrecedence(int op) {
  switch (op) {
  case '+': 
  case '-':
    return 1;
    break;
  case '*':
  case '/':
    return 2;
    break;
  default:
    return 3;
  }
}

int EvalOpType(int op) {
  if (op=='$') return 1;
  if (op=='@') return 1;
  if (op=='!') return 1;
  return 2;
}

int EvalFindObject(char *ptr,int *op,int *step) {
  int k=0,j=-1;
  *op=-1;
  *step=1;
  while (ptr[k] !=0) {
    for (j=0;operator[j] !=NULL;j++) 
      if (strncmp(ptr+k,operator[j],strlen(operator[j]))==0) break;
    if (operator[j] !=NULL) break;
    if (ptr[k]=='(') break;
    if (ptr[k]==')') break;
    k++;
  }
  if (operator[j] !=NULL) {
   *op=opcode[j];
   *step=strlen(operator[j]);
  } else *op=ptr[k];
  return k;
}

// TODO: function description 
int Eval(char *expression,double *ptr,
         int (*dvar)(char *ptr,double *val,void *data),
         void *vdata,
         int (*dfun)(char *ptr,int argnum,double *argptr,
                     double *val,void *data),

         void *fdata) {

  int i=0,j=0,k=0;
  int c;
  int op;
  double value=0;
  double x,y;
  int type;
  int state=0;
  int step;

  struct EvalStack stack;
  struct EvalPostFix output;
  struct EvalPostFix evaluate;

  unsigned char buf[8];
  char *sum=NULL;

  memset(&stack,0,sizeof(struct EvalStack));
  memset(&output,0,sizeof(struct EvalPostFix));
  memset(&evaluate,0,sizeof(struct EvalPostFix));


  /* pre-parse the sum removing white space */

  // TODO: errno check
  sum=malloc(strlen(expression)+1);
  if (sum == NULL)
  {
      fprintf(stderr, "Error: malloc could not create memory for sum\n");
      return -1;
  }
  while (expression[i] !=0) {
    if (expression[i] !=' ') {
     sum[j]=expression[i];
     j++;
    }
    i++;
  }
  sum[j]=0;
  i=0;

  while (sum[i] !=0) {
   
    k=EvalFindObject(sum+i,&op,&step);
  
    if (k>0) {


      /* we have an operand.
       * This can be either a number, eg 3.413
       * A variable name, eg xmin
       * or a function call cos(....)
       */


      if (op=='(') { /* function call */
	/* find matching bracket and pass substring to 
           function processor */

        int l,m,n,s=1;
        char *argbuf=NULL;
        char *name=NULL;
        double *args=NULL;
        int argnum=0;

        l=k+1;
        while ((sum[i+l] !=0) && (s !=0)) {
          if (sum[i+l]=='(') s++;
          if (sum[i+l]==')') s--;
          l++;
	}
        if ((l-k-1)>0) {
          argbuf=malloc(l-k-1);
          memcpy(argbuf,sum+i+k+1,l-k-2);
          argbuf[l-k-2]=0;
          m=0;n=0;

          while (argbuf[n] !=0) {
            if (argbuf[n]==',') {
               argbuf[n]=0;
               if (args==NULL) args=malloc(sizeof(double));
               else args=realloc(args,sizeof(double)*(argnum+1));
               s=Eval(argbuf+m,&args[argnum],dvar,vdata,dfun,fdata);
               if (s !=0) {
  		  free(sum);
                  free(argbuf);
                  return s;
	       }
               argnum++;
               m=n+1;
            }
            n++; 
	  }
      
  
         if (m !=n) {
            if (args==NULL) args=malloc(sizeof(double));
            else args=realloc(args,sizeof(double)*(argnum+1));
            s=Eval(argbuf+m,&args[argnum],dvar,vdata,dfun,fdata);
            if (s !=0) {
	       free(sum);
               return s;
	    }
            argnum++;
         }
         free(argbuf);
	}
        name=malloc(k+1);
        memcpy(name,sum+i,k);
        name[k]=0;


	s=(*dfun)(name,argnum,args,&value,fdata);
        free(name);
        if (s !=0) {
 	   free(sum);
           return 4;
	}
	k=l;
      } else {
	int s;

        char *name=NULL;
        name=malloc(k+1);
        memcpy(name,sum+i,k);
        name[k]=0;

        s=(*dvar)(name,&value,vdata);
        free(name);
        if (s !=0) {
          free(sum);
          return 3;
	}
      }
      
      EvalPushPostfix(&output,0,&value);
      state=1;
    } else if (op=='(') { 
      EvalPush(&stack,'(');
      state=2;
    } else if (op==')') {

      c=EvalPop(&stack);
      while ((stack.num !=0) && (c !='(')) {
        EvalPushPostfix(&output,EvalOpType(c),&c);
        c=EvalPop(&stack);
      }
      if ((c !='(') && (stack.num==0)) {
        free(sum);
        return 2;
      }
      state=3;
    } else {
      if (state==0) {
        if (op=='+') op='@';
        else if (op=='-') op='$';
      } 
      while ((stack.num !=0) && (EvalTop(&stack) !='(') &&
             (EvalPrecedence(EvalTop(&stack))>=EvalPrecedence(op))) {
	c=EvalPop(&stack);
        EvalPushPostfix(&output,EvalOpType(c),&c);
      }
      EvalPush(&stack,op);
      state=0;
    }
    if (k==0) k+=step;
    i+=k;
  }
 
 

  while (stack.num !=0) {
    c=EvalPop(&stack);
    if (c=='(') {
       free(sum);
       return 2;
    }
    EvalPushPostfix(&output,EvalOpType(c),&c);
  }

  for (i=0;i<output.num;i++) {
    if (output.type[i]==0) EvalPushPostfix(&evaluate,0,
                                        &output.value[output.object[i]]);
   
    else if (output.type[i]==1) {
      EvalPopPostFix(&evaluate,&type,buf);
      x=*((double *) buf);
      switch (output.object[i]) {
        case '@' :
          value=+x;
          break;
        case '$' :
          value=-x;
          break;
        case '!' : 
          value=!x;
        }
        EvalPushPostfix(&evaluate,0,&value);
    } else {
    
      EvalPopPostFix(&evaluate,&type,buf);
   
      x=*((double *) buf);
        
      EvalPopPostFix(&evaluate,&type,buf);
      y=*((double *) buf);
     
      switch (output.object[i]) {
      case '*' :
        value=y*x;
        break;
      case '/' :
        value=y/x;
        break;
      case '%' :
        value=y- (int) (y / x);
        break;
      case '+' :
        value=y+x;
        break;
      case '-' :
        value=y-x;
        break;
      case '<' :
        value=y<x;
        break;
      case '>' :
        value=y>x;
        break;
      case 'e' :
        value=(y==x);
        break;
      case 'n' :
        value=(y!=x);
        break;
      case 'l' :
        value=(y<=x);
        break;
      case 'g' :
        value=(y>=x);
        break;
      case '|' :
        value=(int) y | (int) x;
        break;
      case '&' :
        value=(int) y & (int) x;
        break;
      case '^' :
        value=(int) y ^ (int) x;
        break;
     case 'a' :
        value=(int) y && (int) x;
        break;
      case 'o' :
        value=(int) y || (int) x;
        break;


      }
      EvalPushPostfix(&evaluate,0,&value);
    }
  }
  *ptr=evaluate.value[0];
  free(sum);
  return 0;
}



