/* image.c
   ======= 
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




int PostScriptImage(struct PostScript *ptr,
	            struct PostScriptMatrix *matrix,
                    struct FrameBuffer *img,
                    unsigned char mask,
                    float x,float y,
                    struct PostScriptClip *clip) {
  int s=0;
  int d=1;

  float ma=1.0,mb=0.0,mc=0.0,md=1.0;

  int i=0,j=0;
  char buf[4096];

  if (ptr==NULL) return -1;
  if (img==NULL) return -1;
  if (img->wdt==0) return 0;
  if (img->hgt==0) return 0;

  if (img->depth !=8) d=3; 

  if (ptr->cnt >0) {
    sprintf(buf,"d\n");
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    ptr->cnt=0;
  }
  if (s !=0) return s;

  if (clip !=ptr->clip) {
    s=PostScriptClip(ptr,clip);
    if (s !=0) return s;
    ptr->clip=clip;
  }

  if (matrix !=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  sprintf(buf,"s [%g %g %g %g %g %g] concat 0 0 m ",
          ma,mb,mc,md,ptr->x+x,ptr->y+ptr->hgt-y); 

  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  sprintf(buf,"%d %d translate %d %d scale\n",0,-img->hgt,
	  img->wdt,img->hgt);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;


  sprintf(buf,"/DataString %d string def\n",img->wdt*d);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  sprintf(buf,"%d %d 8 [%d 0 0 %d 0 %d]\n",img->wdt,img->hgt,
          img->wdt,-img->hgt,img->hgt);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  sprintf(buf,"{\ncurrentfile DataString readhexstring pop\n} ");
 
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  if (d==1) {
    unsigned char *v;
    sprintf(buf,"bind image\n");
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

    if (s !=0) return s;
    v=img->img;
    for (i=0;i<img->wdt*img->hgt;i++) {
      if (mask & 0x01) sprintf(buf,"%.2x",v[i]);
      else sprintf(buf,"%.2x",0);
      if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);
      if (s !=0) return s;
      j++;
      if ((j % 32)==0) {
	strcpy(buf,"\n");
        if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);
         if (s !=0) return s;
      }
    }
  } else {
    unsigned char *r,*g,*b,*a;
    int rv=0,gv=0,bv=0;
    float av=0;
 
    sprintf(buf,"bind false 3 colorimage\n");
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

    if (s !=0) return s;
    r=img->img;
    g=img->img+img->wdt*img->hgt;
    b=img->img+2*img->wdt*img->hgt;
    a=img->msk;
    for (i=0;i<img->wdt*img->hgt;i++) {
      rv=255;
      gv=255;
      bv=255;
      av=a[i]/255.0;
      if (mask & 0x04) rv=(1-av)*rv+av*r[i];
      if (mask & 0x02) gv=(1-av)*gv+av*g[i];
      if (mask & 0x01) bv=(1-av)*bv+av*b[i];
      sprintf(buf,"%.2x%.2x%.2x",rv,gv,bv);
      if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);
      if (s !=0) return s;
      j++;
      if ((j % 16)==0) {
	strcpy(buf,"\n");
        if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);
         if (s !=0) return s;
      }
    }
  }
  sprintf(buf,"\nr\n");
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

  return s;

}

