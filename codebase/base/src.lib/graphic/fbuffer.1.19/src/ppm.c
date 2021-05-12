/* ppm.c
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
#include "rfbuffer.h"



struct FrameBuffer *FrameBufferLoadPPM(FILE *fp,char *name) {
  char str[256];
  unsigned int max;
  int depth=8,wdt=0,hgt=0;
  int s=0;
  int d=1;
  struct FrameBuffer *ptr;
  char *timg=NULL;
  int c,i;
 
  if (fp==NULL) return NULL;
  ptr=malloc(sizeof(struct FrameBuffer));
  if (ptr==NULL) return NULL;
  ptr->name=NULL;
  ptr->img=NULL;
  ptr->msk=NULL;

  ptr->user.pixel=NULL;
  ptr->user.data=NULL;

  if (name !=NULL) {
    ptr->name=malloc(strlen(name)+1);
    if (ptr->name !=NULL) strcpy(ptr->name,name);
    else s=-1;
  }

  if ((s==0) && (fgets(str,255,fp) ==NULL)) s=-1;
  str[2]=0;
  if ((s==0) && (strcmp(str,"P6")==0)) depth=24;
  if ((s==0) && (strcmp(str,"P5")==0)) depth=8;
  
  while ((s==0) && (fgets(str,255,fp) !=NULL) && (str[0]=='#'));
  if ((s==0) && (sscanf(str,"%d %d",&wdt,&hgt) !=2)) s=-1;
  if ((s==0) && (fgets(str,255,fp)==NULL)) s=-1;
  if ((s==0) && (sscanf(str,"%u",&max) !=1)) s=-1; 

  ptr->wdt=wdt;
  ptr->hgt=hgt;
  ptr->depth=depth;

  
  if (depth !=8) d=3;
  if (s==0) ptr->img=malloc(wdt*hgt*d);
  if (ptr->img==NULL) s=-1;
  if (s==0) ptr->msk=malloc(wdt*hgt);
  if (ptr->msk==NULL) s=-1;

  if (s !=0) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk);
    return NULL;
  }

  memset(ptr->msk,255,wdt*hgt);
  if (d==3) {
    timg=malloc(3*wdt*hgt);
    if (timg==NULL) {
      if (ptr->name !=NULL) free(ptr->name);
      if (ptr->img !=NULL) free(ptr->img);
      if (ptr->msk !=NULL) free(ptr->msk);
      return NULL;
    }
    c=fread(timg,wdt*hgt*3,1,fp);
    for (i=0;i<wdt*hgt;i++) {
      ptr->img[i]=timg[i*3];
      ptr->img[wdt*hgt+i]=timg[i*3+1];
      ptr->img[wdt*hgt*2+i]=timg[i*3+2];
    }
    free(timg);
  } else c=fread(ptr->img,wdt*hgt,1,fp);

  if (c==-1) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk);
    return NULL;
  }
  return ptr;
}


int FrameBufferSavePPM(struct FrameBuffer *ptr,FILE *fp) {
  unsigned char *timg=NULL;
  int o,i,s=0;
  if (ptr==NULL) return -1;
  if (fp==NULL) return -1;
  if (ptr->img==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;

  o=ptr->wdt*ptr->hgt;
  if (ptr->depth !=8) {
    timg=malloc(3*o);
    if (timg==NULL) return -1;
    for (i=0;i<o;i++) {
      timg[i*3]=ptr->img[i];
      timg[i*3+1]=ptr->img[i+o];
      timg[i*3+2]=ptr->img[i+o*2];
    }
  }
  
  if (ptr->depth==8) s=fprintf(fp,"P5\n");  
  else s=fprintf(fp,"P6\n");
  if (s!=-1) s=fprintf(fp,"%d %d\n",ptr->wdt,ptr->hgt);
  if (s!=-1) s=fprintf(fp,"255\n");
  if (s!=-1) {
    if (ptr->depth !=8) s=fwrite(timg,3*o,1,fp);
    else s=fwrite(ptr->img,o,1,fp); 
  }
  if (timg !=NULL) free(timg);
  if (s==-1) return -1;
  return 0;
}


int FrameBufferSavePPMAlpha(struct FrameBuffer *ptr,FILE *fp) {
  int o,s=0;
  if (ptr==NULL) return -1;
  if (fp==NULL) return -1;
  if (ptr->img==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;

  o=ptr->wdt*ptr->hgt;
 
  s=fprintf(fp,"P5\n");  
  if (s!=-1) s=fprintf(fp,"%d %d\n",ptr->wdt,ptr->hgt);
  if (s!=-1) s=fprintf(fp,"255\n");
  if (s!=-1) s=fwrite(ptr->msk,o,1,fp); 
  if (s==-1) return -1;
  return 0;
}


struct FrameBuffer *FrameBufferLoadPPMX(FILE *fp) {
  char str[256];
  unsigned int max;
  int depth=8,wdt=0,hgt=0;
  int s=0;
  int d=1;
  struct FrameBuffer *ptr;
  int c;
 
 
  if (fp==NULL) return NULL;
  ptr=malloc(sizeof(struct FrameBuffer));
  if (ptr==NULL) return NULL;
  ptr->name=NULL;
  ptr->img=NULL;
  ptr->msk=NULL;

  ptr->user.pixel=NULL;
  ptr->user.data=NULL;

  if ((s==0) && (fgets(str,255,fp) ==NULL)) s=-1;
  str[3]=0;
  if ((s==0) && (strcmp(str,"PX6")==0)) depth=24;
  if ((s==0) && (strcmp(str,"PX5")==0)) depth=8;
  
  while ((s==0) && (fgets(str,255,fp) !=NULL) && (str[0]=='#'));
  if ((s==0) && (sscanf(str,"%d %d",&wdt,&hgt) !=2)) s=-1;
  if ((s==0) && (fgets(str,255,fp)==NULL)) s=-1;
  if ((s==0) && (sscanf(str,"%u",&max) !=1)) s=-1; 
  if ((s==0) && (fgets(str,255,fp)==NULL)) s=-1;
  if (s==0) {
    for (c=0;str[c] !=0;c++) if (!isspace(str[c])) break;
    if (str[c] !=0) {	 
      int l;
      l=strlen(str+c);
      if (l>0) {
        str[c+l-1]=0;
        ptr->name=malloc(strlen(str+c)+1);
        if (ptr->name !=NULL) strcpy(ptr->name,str+c);
        else s=-1;
      }
    }
  }
  ptr->wdt=wdt;
  ptr->hgt=hgt;
  ptr->depth=depth;

  
  if (depth !=8) d=3;
  if (s==0) ptr->img=malloc(wdt*hgt*d);
  if (ptr->img==NULL) s=-1;
  if (s==0) ptr->msk=malloc(wdt*hgt);
  if (ptr->msk==NULL) s=-1;

  if (s !=0) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk);
    return NULL;
  }

  if (depth !=8) c=fread(ptr->img,wdt*hgt*3,1,fp);
  else c=fread(ptr->img,wdt*hgt,1,fp); 
  if (c !=-1) {
    c=fread(ptr->msk,wdt*hgt,1,fp);
    if (c !=1) c=-1;
  }

  if (c==-1) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk);
    return NULL;
  }
  return ptr;
}


int FrameBufferSavePPMX(struct FrameBuffer *ptr,FILE *fp) {
  int o,s=0;
  if (ptr==NULL) return -1;
  if (fp==NULL) return -1;
  if (ptr->img==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;

  o=ptr->wdt*ptr->hgt;
  
  if (ptr->depth==8) s=fprintf(fp,"PX5\n");  
  else s=fprintf(fp,"PX6\n");
  if (s!=-1) s=fprintf(fp,"%d %d\n",ptr->wdt,ptr->hgt);
  if (s!=-1) s=fprintf(fp,"255\n");
  if (s!=-1) {
    if (ptr->name !=NULL) s=fprintf(fp,"%s\n",ptr->name);
    else s=fprintf(fp,"\n");
  }

  if (s!=-1) {
    if (ptr->depth !=8) s=fwrite(ptr->img,3*o,1,fp);
    else s=fwrite(ptr->img,o,1,fp); 
  }
  s=fwrite(ptr->msk,o,1,fp);
  if (s==-1) return -1;
  return 0;
}

