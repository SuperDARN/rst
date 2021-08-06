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
#include "fontdb.h"
#include "rps.h"
#include "iplot.h"
#include "splot.h"

int SplotBezier(struct Splot *ptr,
                float x1,float y1,float x2,float y2,
                float x3,float y3,float x4,float y4,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) {
    unsigned int pscolor;      
    pscolor=SplotPostScriptColor(color);
    SplotPostScriptDash(ptr,dash);
    return PostScriptBezier(ptr->ps.ptr,x1,y1,x2,y2,x3,y3,x4,y4,
                     pscolor,width,ptr->ps.dash,ptr->ps.clip);
  }

  if (ptr->fb.ptr !=NULL) {
    struct FrameBuffer *img;
    img=*(ptr->fb.ptr);
    if (img==NULL) return -1;
    SplotFrameBufferDash(ptr,dash);
    return FrameBufferBezier(img,x1,y1,x2,y2,x3,y3,x4,y4,0.05,color,mask,
                          width,ptr->fb.dash,ptr->fb.clip);
  }


  return 0;
}

