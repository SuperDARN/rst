/* dash.c
   ====== 
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"





struct FrameBufferDash *FrameBufferMakeDash(int *p,int phase,int sze) {
  struct FrameBufferDash *ptr=NULL;

  int i;
  int l=0; 
  
  if (p==NULL) return NULL;
  if (sze==0) return NULL;


  ptr=malloc(sizeof(struct FrameBufferDash));
  if (ptr==NULL) return NULL;
  ptr->p=NULL;
 
  ptr->sze=sze;
  ptr->state=1;
  ptr->p=malloc(sizeof(int)*sze);
  if (ptr->p==NULL) {
    free(ptr);
    return NULL;
  }

  for (i=0;i<sze;i++) {
    l+=p[i];
    ptr->p[i]=l;
  }
  phase=phase-l*(phase/l); 
  for (i=0;(i<sze) && (phase>ptr->p[i]);i++);
  ptr->c=phase;
  ptr->ph=phase;
  ptr->i=i;

  return ptr;
}

void FrameBufferFreeDash(struct FrameBufferDash *ptr) {
  if (ptr==NULL) return;
  if (ptr->p !=NULL) free(ptr->p);
  free(ptr);
  return;
}



int FrameBufferDashState(struct FrameBufferDash *ptr) {
  int c,i;

  if (ptr==NULL) return 1;
  if (ptr->p==NULL)  return 1;
  c=ptr->c;
  i=ptr->i;
   
  if (c>=ptr->p[i]) { /* end of segment */
    ptr->state=1-ptr->state;
    if (i==ptr->sze-1) {
       ptr->i=0;
       ptr->c-=ptr->p[i];
    } else ptr->i++;     
  }
  ptr->c++;
  return ptr->state;
}


struct FrameBufferDash *FrameBufferMakeDashString(char *str) {
  struct FrameBufferDash *ptr=NULL;
  char *tmp=NULL;
  char *tok=NULL;
  int *itmp;

  int stp=10,max=10;
  int i,sze=0,phase;
  int s=0;
  int l=0; 
  
  
  if (str==NULL) return NULL;

  tmp=malloc(strlen(str)+1);
  if (tmp==NULL) return NULL;
  strcpy(tmp,str);


  ptr=malloc(sizeof(struct FrameBufferDash));
  if (ptr==NULL) {
    free(tmp);
    return NULL;
  }
  ptr->p=NULL;
  ptr->state=1;

  tok=strtok(tmp," ");
  if (tok==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }
  phase=atoi(tok);

  ptr->p=malloc(stp*sizeof(int));
  if (ptr->p==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }  

  while ((tok=strtok(NULL," ")) !=NULL) {
    ptr->p[sze]=atoi(tok);
    sze++;
    if (sze==max) {
   
      max+=stp;
      itmp=realloc(ptr->p,max*sizeof(int));
      if (itmp==NULL) {
        s=-1;
        break;
      }
      ptr->p=itmp;
    } 
  }

  if (s==-1) {
    free(ptr->p);
    free(tmp);
    free(ptr);
    return NULL;
  }

  ptr->sze=sze;
  for (i=0;i<sze;i++) {
    l+=ptr->p[i];
    ptr->p[i]=l;
  }

  itmp=realloc(ptr->p,sze);
  if (itmp==0) {
    free(ptr->p);
    free(tmp);
    free(ptr);
    return NULL;
  }
  ptr->p=itmp;

  phase=phase-l*(phase/l); 
  for (i=0;(i<sze) && (phase>ptr->p[i]);i++);
  ptr->c=phase;
  ptr->ph=phase;
  ptr->i=i;

  free(tmp);
  return ptr;
}
