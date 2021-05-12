/* char.c
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



 
int FrameBufferFontIntDecode(unsigned char *b) {
  int i,val;
  int test=1;
  unsigned char *out;
  out=(unsigned char *) &val;
  if (*((unsigned char *) &test)) for (i=0;i<4;i++) out[i]=b[3-i];
  else for (i=0;i<4;i++) out[i]=b[i];
  return val;
}

int FrameBufferFontIntEncode(int val,unsigned char *out) {
  int i;
  int test=1;
  unsigned char *b;
  b=(unsigned char *) &val;
  if (out==NULL) return -1;
  if (*((unsigned char *) &test)) for (i=0;i<4;i++) out[i]=b[3-i];
  else for (i=0;i<4;i++) out[i]=b[i];
  return 0;
}


struct FrameBufferFont *FrameBufferLoadFont(FILE *fp) {
  int i,c,sze;
  unsigned char buf[4*6];
  struct FrameBufferFont *ptr=NULL;

  ptr=malloc(sizeof(struct FrameBufferFont));
  if (ptr==NULL) return 0;
  ptr->num=0;
  ptr->name=NULL;
  ptr->sze=0;
  ptr->chr=NULL;

  if (fread(buf,4,1,fp) !=1) {
    free(ptr); 
    return NULL;
  }      
  ptr->num=FrameBufferFontIntDecode(buf);
  
  if (fread(buf,4,1,fp) !=1) {
    free(ptr); 
    return NULL;
  }      
 
  sze=FrameBufferFontIntDecode(buf);
  if (sze !=0) {
     ptr->name=malloc(sze+1);
     if (ptr->name==NULL) {
       free(ptr);
       return NULL;
     }
    if (fread(ptr->name,sze,1,fp) !=1) {
      free(ptr->name);
      free(ptr); 
      return NULL;
    }
    ptr->name[sze]=0;
  }    
  
  if (fread(buf,4,1,fp) !=1) {
    if (ptr->name !=NULL) free(ptr->name);
    free(ptr); 
    return NULL;
  }      
  ptr->sze=FrameBufferFontIntDecode(buf);
 
  ptr->chr=malloc(ptr->num*sizeof(struct FrameBufferChar));
  memset(ptr->chr,0,ptr->num*sizeof(struct FrameBufferChar));

  for (c=0;c<ptr->num;c++) {
    if (fread(buf,4,1,fp) !=1) break;
    sze=FrameBufferFontIntDecode(buf);

    if (fread(buf,4,6,fp) !=6) break;
    ptr->chr[c].w=FrameBufferFontIntDecode(buf);
    ptr->chr[c].h=FrameBufferFontIntDecode(buf+4);
    ptr->chr[c].l=FrameBufferFontIntDecode(buf+8);
    ptr->chr[c].r=FrameBufferFontIntDecode(buf+12);
    ptr->chr[c].b=FrameBufferFontIntDecode(buf+16);
    ptr->chr[c].t=FrameBufferFontIntDecode(buf+20);
    ptr->chr[c].sze=sze;

    if (sze==0) continue;
    if ((ptr->chr[c].data=malloc(sze))==NULL) break;
    if (fread(ptr->chr[c].data,sze,1,fp) !=1) break;
  }

  if (c<ptr->num) {
    for (i=0;i<c;i++) free(ptr->chr[i].data);
    free(ptr->chr);
    free(ptr);
    return NULL;
  }
  return ptr;
}

void FrameBufferFreeFont(struct FrameBufferFont *ptr) {

  int i;
  if (ptr==NULL) return;
  if (ptr->chr !=NULL) {
      for (i=0;i<ptr->num;i++) 
        if (ptr->chr[i].data !=NULL) free(ptr->chr[i].data);
      free(ptr->chr);
  }
  free(ptr);
  return;

}


int FrameBufferSaveFont(FILE *fp,
                           struct FrameBufferFont *ptr) {

  int c,sze;
  unsigned char buf[4*6];

  if (ptr==NULL) return -1;
  if (fp==NULL) return -1;
  
  FrameBufferFontIntEncode(ptr->num,buf); 
  if (fwrite(buf,4,1,fp) !=1) return -1;  
  
  if (ptr->name !=NULL) FrameBufferFontIntEncode(strlen(ptr->name),buf);
  if (fwrite(buf,4,1,fp) !=1) return -1;  
  if (fwrite(ptr->name,strlen(ptr->name),1,fp) !=1) return -1;

  FrameBufferFontIntEncode(ptr->sze,buf); 
  if (fwrite(buf,4,1,fp) !=1) return -1;  
 
  for (c=0;c<ptr->num;c++) {
    sze=ptr->chr[c].sze;
    FrameBufferFontIntEncode(sze,buf); 
    if (fwrite(buf,4,1,fp) !=1) return -1;  
    FrameBufferFontIntEncode(ptr->chr[c].w,buf);
    FrameBufferFontIntEncode(ptr->chr[c].h,buf+4);
    FrameBufferFontIntEncode(ptr->chr[c].l,buf+8);
    FrameBufferFontIntEncode(ptr->chr[c].r,buf+12);
    FrameBufferFontIntEncode(ptr->chr[c].b,buf+16);
    FrameBufferFontIntEncode(ptr->chr[c].t,buf+20);
    if (fwrite(buf,4,6,fp) !=6) return -1;  
    if (sze==0) continue;
    if (fwrite(ptr->chr[c].data,sze,1,fp) !=1) break;
  }
  return 0;
}


int FrameBufferTextBox(struct FrameBufferFont *fnt,
                       int num,char *txt,int *box) {

  int l,r,x=0,c;  
  int d=4096;
  int rf=0;
  int t=0;
  int n=0;
  box[0]=0;
  box[1]=0;
  box[2]=0;

  if (fnt==NULL) return -1;

  for (n=0;n<num;n++) {
     c=txt[n];
     l=fnt->chr[c].l;
     r=fnt->chr[c].r;
     if (n==0) l=0;
     x+=1+r-l;
     if (fnt->chr[c].data==NULL) continue;
     if (fnt->chr[c].b<d) d=fnt->chr[c].b;
     if (fnt->chr[c].t>t) t=fnt->chr[c].t;
     rf=1;
  }
  if (rf==0) d=0;
  box[0]=x;
  box[1]=d;
  box[2]=t;
  return 0;
}



int FrameBufferText(struct FrameBuffer *ptr,struct FrameBufferMatrix *matrix,
                        struct FrameBufferFont *fnt,int x,int y,
                        int num,char *txt,unsigned int color,
                        unsigned char m,int sflg,
                        struct FrameBufferClip *clip) {
  int bit;
  unsigned char r,g,b,a;
  unsigned char rv,gv,bv,av;
  float ma=1,mb=0,mc=0,md=1;
  float determ;
  float ia,ib,ic,id;

  int tr,flg;
  int btm=0,lft=0,rgt=0,top=0;
  int ix[4]={0,0,0,0},iy[4]={0,0,0,0};
  int tix,tiy;

  int sze;
  int s=0;
  int lf,rg;
  int dx,dy;
  int i,j;
  int n=0,c;
  int u=0;
  int tx,ty;
  int rf=0;
  int w=0,h=0,o,bt=1e6,tp=-1e6;
  unsigned char *tbuf;

  if (ptr==NULL) return -1;
  if (fnt==NULL) return -1;

  if (matrix!=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }
 
  sze=ptr->wdt*ptr->hgt;
  a=color>>24;;
  r=color>>16;
  g=color>>8;
  b=color;

  /* find out the size of the font in pixels */

  for (n=0;n<num;n++) {
    c=txt[n];
    rg=fnt->chr[c].r;
    lf=fnt->chr[c].l;
    if (n==0) lf=0; 
    w+=1+rg-lf;
    if (fnt->chr[c].data==NULL) continue;
    if (fnt->chr[c].b<bt) bt=fnt->chr[c].b;
    if (fnt->chr[c].t>tp) tp=fnt->chr[c].t;   
    rf=1;
    
  }
  if (rf==0) return 0; /* nothing to render */
  h=1+tp-bt;
  o=tp;
  if ((w==0) || (h==0)) return 0;
  tbuf=malloc(w*h);   
  if (tbuf==NULL) return -1;
  memset(tbuf,0,w*h);

  /* now render our font */
 

  for (n=0;n<num;n++) {
    c=txt[n];
    rg=fnt->chr[c].r;
    lf=fnt->chr[c].l;
    if (n==0) lf=0;
    if (fnt->chr[c].data==NULL) {
      u+=1+rg-lf;
      continue;
    } 
    bit=(fnt->chr[c].h/8)*8;
    bit=((fnt->chr[c].h-bit)>0) ?  (bit+8) : bit; 

    for (dx=0;dx<fnt->chr[c].w;dx++) {
      for (dy=0;dy<fnt->chr[c].h;dy++) {
        i=(dx*bit+(dy/8));
        j=fnt->chr[c].data[i] & (1<<(dy  % 8));
        if (j!=0) {
          tx=u-lf+dx;
          ty=o-fnt->chr[c].b-dy;
          tbuf[ty*w+tx]=1;
	}
      }
    }        
    u+=1+rg-lf;
  }  
  
  /* now render */

  determ=1/(ma*md-mb*mc);
  ia=md*determ;
  ib=-mb*determ;
  ic=-mc*determ;
  id=ma*determ;

  /* work out the limits of the plot in image space */

  ix[1]=w;
  ix[2]=w;
  iy[0]=-o;
  iy[1]=-o;
  iy[2]=h-o;
  iy[3]=h-o;
 
  lft=ptr->wdt;
  btm=ptr->hgt;

  for (tr=0;tr<4;tr++) {
    tx=x+ix[tr]*ma+iy[tr]*mb;
    ty=y+ix[tr]*mc+iy[tr]*md;
    if (tx<lft) lft=tx;
    if (ty<btm) btm=ty;
    if (tx>rgt) rgt=tx;
    if (ty>top) top=ty;
  } 

  if (lft<0) lft=0;
  if (btm<0) btm=0;
  if (rgt>=ptr->wdt) rgt=ptr->wdt-1;
  if (top>=ptr->hgt) top=ptr->hgt-1;

  ty=btm;
  while (ty !=top+1) {
    tx=lft;
    while (tx !=rgt+1) {
   
      flg=1;
      ix[0]=(tx-x)*ia+(ty-y)*ib;
      iy[0]=o+(tx-x)*ic+(ty-y)*id;
          
      if (ix[0]<0) flg=0;
      if (iy[0]<0) flg=0;
      if (ix[0]>=w) flg=0;
      if (iy[0]>=h) flg=0;
      tix=ix[0];
      tiy=iy[0];
      
      if (flg) {
        if (sflg) {  
          int cv=0,c=0;
          float alpha;
          unsigned char ra,ga,ba,aa;
          int ilft=0,ibtm=0,irgt=0,itop=0;
          ix[1]=(tx+1-x)*ia+(ty-y)*ib;
          iy[1]=iy[0];
          ix[2]=ix[1];
          iy[2]=o+(tx-x)*ic+(ty+1-y)*id;
          ix[3]=ix[0];
          iy[3]=iy[2];
          ilft=w;
          ibtm=h;
          for (i=0;i<4;i++) {
            if (ix[i]<0) ix[i]=0;
            if (iy[i]<0) iy[i]=0;
            if (ix[i]>=w) ix[i]=w-1;
            if (iy[i]>=h) iy[i]=h-1;
            if (ix[i]<ilft) ilft=ix[i];
            if (iy[i]<ibtm) ibtm=iy[i];
            if (ix[i]>irgt) irgt=ix[i];
            if (iy[i]>itop) itop=iy[i];
	  }
          if ((ilft==irgt) || (itop==ibtm)) {
            if (ilft==irgt) {
	      tix=ilft;
              tiy=(ibtm+itop)/2;

	    }
            if (ibtm==itop) {
	      tix=(ilft+irgt)/2;
              tiy=ibtm;
	    }
            if (tbuf[tiy*w+tix] !=0) cv++;
            c=1;
	  } else {
            for (tix=ilft;tix<irgt;tix++) {
              for (tiy=ibtm;tiy<itop;tiy++) {
                if (tbuf[tiy*w+tix] !=0) cv++;
                c++;
	      }
	    }
	  }
          alpha=(float) cv/c;
          rv=ptr->img[ty*ptr->wdt+tx];
          av=ptr->msk[ty*ptr->wdt+tx];
          if (ptr->depth !=8) {
            gv=ptr->img[ty*ptr->wdt+tx+sze];
            bv=ptr->img[ty*ptr->wdt+tx+2*sze];
	  } else {
            gv=rv;
            bv=rv;
	  }

          ra=rv*(1-alpha)+r*alpha;
          ga=gv*(1-alpha)+g*alpha;
          ba=bv*(1-alpha)+b*alpha;
          aa=av*(1-alpha)+a*alpha;
           
          s=FrameBufferRawPixel(ptr,tx,ty,sze,ra,ga,ba,aa,m,clip);   
        } else if (tbuf[tiy*w+tix] !=0) 
           s=FrameBufferRawPixel(ptr,tx,ty,sze,r,g,b,a,m,clip);
      }
      if (s !=0) break;
      tx++;
    }
    if (s !=0) break;
    ty++;
  }
  free(tbuf);
  return s;
}



