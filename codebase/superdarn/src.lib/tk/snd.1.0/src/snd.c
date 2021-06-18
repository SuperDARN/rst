/* snd.c
   ========
   Author: E.G.Thomas

   
   Copyright (C) <year>  <name of author>

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "snddata.h"


struct SndData *SndMake() {

  struct SndData *ptr=NULL;
  ptr=malloc(sizeof(struct SndData));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct SndData));
  ptr->origin.time=NULL;
  ptr->origin.command=NULL;
  ptr->combf=NULL;
  ptr->rng=NULL;
  return ptr;
}


void SndFree(struct SndData *ptr) {

  if (ptr==NULL) return;
  if (ptr->origin.time !=NULL) free(ptr->origin.time);
  if (ptr->origin.command !=NULL) free(ptr->origin.command);
  if (ptr->combf !=NULL) free(ptr->combf);
  if (ptr->rng !=NULL) free(ptr->rng);
  free(ptr);
  return;
}


int SndSetOriginTime(struct SndData *ptr,char *str) {
  char *tmp=NULL;
  if (ptr==NULL) return -1;

  if (str==NULL) {
    if (ptr->origin.time !=NULL) free(ptr->origin.time);
    ptr->origin.time=NULL;
    return 0;
  }

  if (ptr->origin.time==NULL) tmp=malloc(strlen(str)+1);
  else tmp=realloc(ptr->origin.time,strlen(str)+1);

  if (tmp==NULL) return -1;
  strcpy(tmp,str);
  ptr->origin.time=tmp;
  return 0;
}


int SndSetOriginCommand(struct SndData *ptr,char *str) {
  char *tmp=NULL;
  if (ptr==NULL) return -1;

  if (str==NULL) {
    if (ptr->origin.command !=NULL) free(ptr->origin.command);
    ptr->origin.command=NULL;
    return 0;
  }

  if (ptr->origin.command==NULL) tmp=malloc(strlen(str)+1);
  else tmp=realloc(ptr->origin.command,strlen(str)+1);

  if (tmp==NULL) return -1;
  strcpy(tmp,str);
  ptr->origin.command=tmp;
  return 0;
}


int SndSetCombf(struct SndData *ptr,char *str) {
  void *tmp=NULL;
  if (ptr==NULL) return -1;

  if (str==NULL) {
    if (ptr->combf !=NULL) free(ptr->combf);
    ptr->combf=NULL;
    return 0;
  }

  if (ptr->combf==NULL) tmp=malloc(strlen(str)+1);
  else tmp=realloc(ptr->combf,strlen(str)+1);

  if (tmp==NULL) return -1;
  strcpy(tmp,str);
  ptr->combf=tmp;
  return 0;
}


int SndSetRng(struct SndData *ptr,int nrang) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if (nrang==0) {
    if (ptr->rng !=NULL) free(ptr->rng);
    ptr->rng=NULL;
    return 0;
  }
  if (ptr->rng==NULL) tmp=malloc(sizeof(struct SndRange)*nrang);
  else tmp=realloc(ptr->rng,sizeof(struct SndRange)*nrang);

  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(struct SndRange)*nrang);
  ptr->rng=tmp;
  return 0;
}

