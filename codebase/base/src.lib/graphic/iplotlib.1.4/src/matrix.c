/* matrix.c
   ======== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rfbuffer.h"
#include "iplot.h"

#ifndef PI
  #define PI 3.14159265358979323846
#endif

struct PlotMatrix *PlotMatrixString(char *str) {
  struct PlotMatrix *ptr;
  char *tmp=NULL;
  char *tok=NULL;
  int s=0;

  if (str==NULL) return NULL;

  tmp=malloc(strlen(str)+1);
  if (tmp==NULL) return NULL;
  strcpy(tmp,str);

  ptr=malloc(sizeof(struct PlotMatrix));
  if (ptr==NULL) {
    free(tmp);
    return NULL;
  }
  
  tok=strtok(tmp," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->a=atof(tok);
  if (s==0) tok=strtok(NULL," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->b=atof(tok);
  if (s==0) tok=strtok(NULL," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->c=atof(tok);
  if (s==0) tok=strtok(NULL," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->d=atof(tok);

  free(tmp);
  if (s !=0) {
    free(ptr);
    return NULL;
  }
  return ptr; 
}

int PlotScaleMatrix(struct PlotMatrix *ptr,
                            float xscale,float yscale) {
  
  if (ptr==NULL) return -1;
  ptr->a=ptr->a*xscale;
  ptr->b=ptr->b*yscale;
  ptr->c=ptr->c*xscale;
  ptr->d=ptr->d*yscale;
 return 0;

}
        
int PlotRotateMatrix(struct PlotMatrix *ptr,
                      float angle) {
  float costh,sinth;
  float ma,mb,mc,md;
  if (ptr==NULL) return -1;
  
  costh=cos(PI*angle/180.0);
  sinth=sin(PI*angle/180.0);

  ma=ptr->a*costh-ptr->c*sinth;
  mb=ptr->b*costh-ptr->d*sinth;
  mc=ptr->a*sinth+ptr->c*costh;
  md=ptr->b*sinth+ptr->d*costh;

  ptr->a=ma;
  ptr->b=mb;
  ptr->c=mc;
  ptr->d=md;  
  return 0;

}

int PlotMatrixTransform(struct PlotMatrix *ptr,float ix,float iy,
			    float *ox,float *oy) {
  if (ptr==NULL) return -1;
  *ox=ptr->a*ix+ptr->b*iy;
  *oy=ptr->c*ix+ptr->d*iy;
  return 0;
}

int PlotMatrixMultiply(struct PlotMatrix *x,struct PlotMatrix *y,
                           struct PlotMatrix *z) {
  float a,b,c,d;

  a=x->a*y->a+x->b*y->c;
  b=x->a*y->b+x->b*y->d;
  c=x->c*y->a+x->d*y->c;
  d=x->c*y->b+x->d*y->d;

  z->a=a;
  z->b=b;
  z->c=c;
  z->d=d;
  return 0;
}

