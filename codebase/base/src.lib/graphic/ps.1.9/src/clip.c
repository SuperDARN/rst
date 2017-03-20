/* clip.c
   ====== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "rps.h"



struct PostScriptClip *PostScriptMakeClip(float x,float y,float wdt,
                                          float hgt, 
                                          int num,float *px,
                                          float *py,int *t) {
  struct PostScriptClip *ptr=NULL;

  int i;
 
  if (px==NULL) return NULL;
  if (py==NULL) return NULL;
  if (t==NULL) return NULL;
  if (num==0) return NULL;

  ptr=malloc(sizeof(struct PostScriptClip));
  if (ptr==NULL) return NULL;
 
  ptr->num=num;
  ptr->px=malloc(sizeof(float)*num);
  if (ptr->px==NULL) {
    free(ptr);
    return NULL;
  }

  ptr->py=malloc(sizeof(float)*num);
  if (ptr->py==NULL) {
    free(ptr->px);
    free(ptr);
    return NULL;
  }
  ptr->t=malloc(sizeof(int)*num);
  if (ptr->t==NULL) {
    free(ptr->px);
    free(ptr->py);
    free(ptr);
    return NULL;
  }

  for (i=0;i<num;i++) {
    ptr->px[i]=x+px[i];
    ptr->py[i]=y+hgt-py[i];
    ptr->t[i]=t[i];
  }
 
  return ptr;
}

void PostScriptFreeClip(struct PostScriptClip *ptr) {
  if (ptr==NULL) return;
  if (ptr->px !=NULL) free(ptr->px);
  if (ptr->py !=NULL) free(ptr->py);
  if (ptr->t !=NULL) free(ptr->t);
  free(ptr);
  return;
}


int PostScriptClip(struct PostScript *ptr,
                   struct PostScriptClip *clip) {
  int s=0;
  int n,c;
  char buf[4096];
  if (ptr==NULL) return -1;
 
  if (clip==NULL) {
     sprintf(buf,"initclip\n");
     if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
     return s;
  }
 
  sprintf(buf,"n %g %g m ",
          clip->px[0],clip->py[0]);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  for (n=1;n<=clip->num;n++) {
    c=(n==clip->num) ? 0 : n;

    if (clip->t[c]==1) {
      n+=2;
      if (n>clip->num) break;
      c=(n==clip->num) ? 0 : n;
      sprintf(buf,"%g %g %g %g %g %g b ",
              clip->px[n-2],clip->py[n-2],
              clip->px[n-1],clip->py[n-1],
              clip->px[c],clip->py[c]);

    } else 
      sprintf(buf,"%g %g l ",clip->px[c],clip->py[c]);

    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  }
  if (n<=clip->num) return -1;

  sprintf(buf,"e clip n %g %g m\n",ptr->px,ptr->py);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  return 0;
}





