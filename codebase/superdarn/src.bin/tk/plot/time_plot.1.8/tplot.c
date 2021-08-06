/* tplot.c
   =======
   Author: R.J.Barnes
*/


/*
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "tplot.h"



int tplotset(struct tplot *ptr,int nrang) {
  void *tmp=NULL;
  if (ptr==NULL) return -1;
    
  if (ptr->qflg==NULL) tmp=malloc(sizeof(int)*nrang);
  else tmp=realloc(ptr->qflg,sizeof(int)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(int)*nrang);
  ptr->qflg=tmp;

  if (ptr->gsct==NULL) tmp=malloc(sizeof(int)*nrang);
  else tmp=realloc(ptr->gsct,sizeof(int)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(int)*nrang);
  ptr->gsct=tmp;

  if (ptr->p_0==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->p_0,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->p_0=tmp;

  if (ptr->p_l==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->p_l,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->p_l=tmp;

  if (ptr->p_l_e==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->p_l_e,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->p_l_e=tmp;

  if (ptr->v==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->v,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->v=tmp;

  if (ptr->v_e==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->v_e,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->v_e=tmp;

  if (ptr->w_l==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->w_l,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->w_l=tmp;

  if (ptr->w_l_e==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->w_l_e,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->w_l_e=tmp;

  if (ptr->phi0==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->phi0,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->phi0=tmp;

  if (ptr->elv==NULL) tmp=malloc(sizeof(double)*nrang);
  else tmp=realloc(ptr->elv,sizeof(double)*nrang);
  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(double)*nrang);
  ptr->elv=tmp;

  return 0;
}
