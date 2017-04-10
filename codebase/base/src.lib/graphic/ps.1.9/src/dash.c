/* dash.c
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



struct PostScriptDash *PostScriptMakeDash(float *p,float phase,int sze) {
  struct PostScriptDash *ptr=NULL;

  int i;
  
  if (p==NULL) return NULL;
  if (sze==0) return NULL;

  ptr=malloc(sizeof(struct PostScriptDash));
  if (ptr==NULL) return NULL;
  ptr->p=NULL;
 
  ptr->sze=sze;
  ptr->p=malloc(sizeof(float)*sze);
  if (ptr->p==NULL) {
    free(ptr);
    return NULL;
  }

  for (i=0;i<sze;i++) {
    ptr->p[i]=p[i];
  }
  ptr->phase=phase;  
  return ptr;
}

struct PostScriptDash *PostScriptMakeDashString(char *str) {
  struct PostScriptDash *ptr=NULL;
  char *tmp=NULL;
  char *tok=NULL;
  float *ftmp=NULL;
  int stp=10,max=10;
  int sze=0;
  int s=0;
   
  if (str==NULL) return NULL;

  tmp=malloc(strlen(str)+1);
  if (tmp==NULL) return NULL;
  strcpy(tmp,str);

  ptr=malloc(sizeof(struct PostScriptDash));
  if (ptr==NULL) {
    free(tmp);
    return NULL;
  }
  ptr->p=NULL;

  tok=strtok(tmp," ");
  if (tok==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }
  ptr->phase=atof(tok);

  ptr->p=malloc(stp*sizeof(float));
  if (ptr->p==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }  

  while ((tok=strtok(NULL," ")) !=NULL) {
    ptr->p[sze]=atof(tok);
    sze++;
    if (sze==max) {
      max+=stp;
      ftmp=realloc(ptr->p,max*sizeof(float));
      if (ftmp==NULL) {
        s=-1;
        break;
      }
      ptr->p=ftmp;
    } 
  }

  if (s==-1) {
    free(ptr->p);
    free(ptr);
    return NULL;
  }

  ptr->sze=sze;
  ftmp=realloc(ptr->p,sze);
  if (ftmp==NULL) {
    free(ptr->p);
    free(ptr);
    return NULL;
  }

  free(tmp);
  return ptr;
}

void PostScriptFreeDash(struct PostScriptDash *ptr) {
  if (ptr==NULL) return;
  if (ptr->p !=NULL) free(ptr->p);
  free(ptr);
  return;
}
