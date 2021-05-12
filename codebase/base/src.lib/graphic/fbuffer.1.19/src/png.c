/* png.c
   ===== 
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
#include <ctype.h>
#include <png.h>
#include "rfbuffer.h"

struct FrameBuffer *FrameBufferLoadPNG(FILE *fp,char *name) {
  int s=0,x,y,d=1,m=0;
  int wdt,hgt,rowbytes;

  unsigned char hdr[8];
  png_structp pngptr=NULL;
  png_infop infoptr=NULL;
  png_bytep *rowptr=NULL;
  png_bytep imgptr=NULL; 
  png_bytep rp=NULL;
  
  struct FrameBuffer *ptr=NULL;

  s=fread(hdr,1,8,fp);
  if (s !=8) return NULL;
  
  if (png_sig_cmp(hdr,0,8) !=0) return NULL;

  pngptr=png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (pngptr==NULL) return NULL;

  infoptr=png_create_info_struct(pngptr); 

  if (infoptr==NULL) {
    png_destroy_read_struct(&pngptr,&infoptr,NULL);
    return NULL;
  }

  if (setjmp(png_jmpbuf(pngptr))) {
    png_destroy_read_struct(&pngptr,&infoptr,NULL);
    return NULL;
  }

  png_init_io(pngptr,fp);
  png_set_sig_bytes(pngptr,8);

  png_read_info(pngptr,infoptr);

  wdt = (int)png_get_image_width(pngptr, infoptr);
  hgt = (int)png_get_image_height(pngptr, infoptr);
  
  switch (png_get_color_type(pngptr, infoptr)) {
  case PNG_COLOR_TYPE_GRAY:
    d=1;
    m=0;
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA:
    d=1;
    m=1;
    break;
  case PNG_COLOR_TYPE_RGB:
    d=3;
    m=0;
    break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
    d=3;
    m=1;
    break;
  }

  if (png_get_bit_depth(pngptr, infoptr) == 16) {
    png_set_strip_16(pngptr);
  }

  if (png_get_bit_depth(pngptr, infoptr) < 8) {
    png_destroy_read_struct(&pngptr,&infoptr,NULL);
    return NULL;
  }
   
 
  ptr=malloc(sizeof(struct FrameBuffer));
  if (ptr==NULL) {
    png_destroy_read_struct(&pngptr,&infoptr,NULL);
    return NULL;
  }

  ptr->name=NULL;
  ptr->img=NULL;
  ptr->msk=NULL;

  ptr->user.pixel=NULL;
  ptr->user.data=NULL;

  s=0;

  if (name !=NULL) {
    ptr->name=malloc(strlen(name)+1);
    if (ptr->name !=NULL) strcpy(ptr->name,name);
    else s=-1;
  }

  ptr->wdt=wdt;
  ptr->hgt=hgt;
  ptr->depth=d*8;

  if (s==0) ptr->img=malloc(wdt*hgt*d);
  if (ptr->img==NULL) s=-1;
  if (s==0) ptr->msk=malloc(wdt*hgt);
  if (ptr->msk==NULL) s=-1;

  if (s !=0) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk);
    free(ptr);
    png_destroy_read_struct(&pngptr,&infoptr,NULL); 
    return NULL;
  }

  if (setjmp(png_jmpbuf(pngptr))) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk);
    free(ptr);
    if (rowptr !=NULL) free(rowptr);
    if (imgptr !=NULL) free(imgptr);
    png_destroy_read_struct(&pngptr,&infoptr,NULL);
    return NULL;
  }

  if (m==0) memset(ptr->msk,255,wdt*hgt);
  memset(ptr->msk,255,wdt*hgt);
  
  png_set_interlace_handling(pngptr);

  png_read_update_info(pngptr,infoptr);

  rowptr=(png_bytep *) malloc(sizeof(png_bytep)*hgt);
  if (rowptr==NULL) s=-1;
  
  rowbytes = png_get_rowbytes(pngptr, infoptr);
  if (s==0) imgptr=(png_byte *) malloc(rowbytes*hgt);
  if (imgptr==NULL) s=-1;  

  if (s !=0) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk);
    free(ptr);
    if (rowptr !=NULL) free(rowptr);
    if (imgptr !=NULL) free(imgptr);
    png_destroy_read_struct(&pngptr,&infoptr,NULL); 
    return NULL;
  }
   

  for (y=0; y<hgt; y++)
    rowptr[y] = (png_byte*) &imgptr[y*rowbytes];
  
  png_read_image(pngptr,rowptr);

  for (y=0;y<hgt;y++){
    rp=rowptr[y];
    for (x=0;x<wdt;x++) {
      if (m==1) ptr->msk[y*wdt+x]=rp[x*(d+m)+d];
      ptr->img[y*wdt+x]=rp[x*(d+m)];
      if (d !=1) {
        ptr->img[wdt*hgt+y*wdt+x]=rp[x*(d+m)+1];
        ptr->img[2*wdt*hgt+y*wdt+x]=rp[x*(d+m)+2];
      }
    }
  }  

  free(imgptr);
  free(rowptr);

  png_destroy_read_struct(&pngptr,&infoptr,NULL);

  return ptr;

}

int FrameBufferSavePNG(struct FrameBuffer *ptr,FILE *fp) {
  int x,y;
  png_structp pngptr=NULL;
  png_infop infoptr=NULL;
  png_bytep *rowptr=NULL;
  png_bytep imgdata=NULL; 
  png_bytep rp=NULL;

  int depth=8,type=0,bw;

  bw=ptr->depth/8+(ptr->msk !=NULL);


  switch (bw) {
  case 1 :
    type=PNG_COLOR_TYPE_GRAY;
    break;
  case 2: 
    type=PNG_COLOR_TYPE_GRAY_ALPHA;
    break;
  case 3:
    type=PNG_COLOR_TYPE_RGB; 
    break;
  default:
    type=PNG_COLOR_TYPE_RGBA;
    break;
  }   
  rowptr=malloc(sizeof(png_bytep *)*ptr->hgt);
  if (rowptr==NULL) return -1;

  imgdata=malloc(sizeof(png_byte)*ptr->wdt*ptr->hgt*bw);
  if (imgdata==NULL) {
    free(rowptr);
    return -1;
  }

  for (y=0;y<ptr->hgt;y++) {
    rowptr[y]=&imgdata[y*ptr->wdt*bw];
    rp=rowptr[y];
    for (x=0;x<ptr->wdt;x++) {
      if ((bw==2) || (bw==4)) rp[x*bw+bw-1]=ptr->msk[y*ptr->wdt+x];
      rp[x*bw]=ptr->img[y*ptr->wdt+x];           
      if (bw>2) {
         rp[x*bw+1]=ptr->img[ptr->wdt*ptr->hgt+y*ptr->wdt+x];
         rp[x*bw+2]=ptr->img[2*ptr->wdt*ptr->hgt+y*ptr->wdt+x];
      }  
    }
  }

  pngptr=png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
  if (pngptr==NULL) return -1;

  infoptr=png_create_info_struct(pngptr);
  if (infoptr==NULL) {
    png_destroy_write_struct(&pngptr,NULL); 
    return -1;
  }

  if (setjmp(png_jmpbuf(pngptr))) {
    png_destroy_write_struct(&pngptr,&infoptr); 
    return -1;
  } 
  
  png_init_io(pngptr,fp);
  png_set_IHDR(pngptr, infoptr, ptr->wdt,ptr->hgt,depth,
	       type,PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
  png_write_info(pngptr, infoptr);

  png_write_image(pngptr,rowptr);

  png_write_end(pngptr, NULL);
  
  free(imgdata);
  free(rowptr);
  
  png_destroy_write_struct(&pngptr,&infoptr);


  return 0;
}


