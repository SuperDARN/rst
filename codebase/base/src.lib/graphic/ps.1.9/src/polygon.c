/* Polygon.c
   ========= 
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
#include "rps.h"




int PostScriptPolygon(struct PostScript *ptr,
                   struct PostScriptMatrix *matrix,
                   float x,float y,
                   int num,float *px,float *py,int *t,int fill,
                   unsigned int color,float width,
                   struct PostScriptDash *dash,struct PostScriptClip *clip) {
  int s=0;
  int n,c;
  float ma=1.0,mb=0.0,mc=0.0,md=1.0;
  char buf[4096];
  if (ptr==NULL) return -1;
 
  s=PostScriptState(ptr,color,width,dash,clip);
  if (s !=0) return s;

  if (ptr->cnt > 0) {
    sprintf(buf,"d\n");
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->cnt=0;
  }

  if (matrix !=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  sprintf(buf,"s n [%g %g %g %g %g %g] concat %g %g m ",
          ma,mb,mc,md,ptr->x+x,ptr->y+ptr->hgt-y,px[0],-py[0]);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  for (n=1;n<=num;n++) {
    c=(n==num) ? 0 : n;

    if (t[c]==1) {
      n+=2;
      if (n>num) break;
      c=(n==num) ? 0 : n;
      sprintf(buf,"%g %g %g %g %g %g b ",
              px[n-2],-py[n-2],
              px[n-1],-py[n-1],
              px[c],-py[c]);

    } else 
      sprintf(buf,"%g %g l ",px[c],-py[c]);

    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  }
  if (n<=num) return -1;


  if (fill !=0) sprintf(buf,"e f r\n");
  else sprintf(buf,"e d r\n");
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  return 0;
}
