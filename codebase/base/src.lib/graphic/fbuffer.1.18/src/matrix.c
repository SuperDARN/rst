/* matrix.c
   ======== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "rfbuffer.h"



#ifndef PI
  #define PI 3.14159265358979323846
#endif



struct FrameBufferMatrix *FrameBufferMatrixString(char *str) {
  struct FrameBufferMatrix *ptr;
  char *tmp=NULL;
  char *tok=NULL;
  int s=0;

  if (str==NULL) return NULL;

  tmp=malloc(strlen(str)+1);
  if (tmp==NULL) return NULL;
  strcpy(tmp,str);

  ptr=malloc(sizeof(struct FrameBufferMatrix));
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

int FrameBufferScaleMatrix(struct FrameBufferMatrix *ptr,
                            float xscale,float yscale) {
  
  if (ptr==NULL) return -1;
  ptr->a=ptr->a*xscale;
  ptr->b=ptr->b*yscale;
  ptr->c=ptr->c*xscale;
  ptr->d=ptr->d*yscale;
 return 0;

}
        
int FrameBufferRotateMatrix(struct FrameBufferMatrix *ptr,
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
                       

int FrameBufferTransform(struct FrameBufferMatrix *ptr,int ix,int iy,
                        int *ox,int *oy) {
  if (ptr==NULL) return -1;
  *ox=ptr->a*ix+ptr->b*iy;
  *oy=ptr->c*ix+ptr->d*iy;
  return 0;
}
