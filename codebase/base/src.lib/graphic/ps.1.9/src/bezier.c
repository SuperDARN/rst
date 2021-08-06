/* bezier.c
   ======== 
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





int PostScriptBezier(struct PostScript *ptr,
		     float x1,float y1,float x2,float y2,float x3,float y3,
                     float x4,float y4,
                     unsigned int color,float width,
                     struct PostScriptDash *dash,struct PostScriptClip *clip) {
  int s=0;
  char buf[4096];
  if (ptr==NULL) return -1;
 
  s=PostScriptState(ptr,color,width,dash,clip);
  if (s !=0) return s;

  if ((x1 !=ptr->px) || (y1 !=ptr->py)) {
    sprintf(buf,"%g %g m ",ptr->x+x1,ptr->y+ptr->hgt-y1);
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  } 

  sprintf(buf,"%g %g %g %g %g %g b ",ptr->x+x2,ptr->y+ptr->hgt-y2,
          ptr->x+x3,ptr->y+ptr->hgt-y3,ptr->x+x4,ptr->y+ptr->hgt-y4);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  
  ptr->cnt++;
  if (ptr->cnt>STROKE_COUNT) {
  
    sprintf(buf,"d\n %g %g m ",ptr->x+x4,ptr->y+ptr->hgt-y4);
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->cnt=0;
  }

  ptr->px=x4;
  ptr->py=y4;
  return 0;
}
