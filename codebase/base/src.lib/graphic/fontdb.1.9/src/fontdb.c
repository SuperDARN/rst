/* fontdb.c
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
#include <math.h>
#include <ctype.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "fontdb.h"



int FrameBufferFontDBText(char *buf,int sze,void *data) {
  int o=0,c=0,i;
  char *tmp;
  struct FrameBufferFontDB *ptr;
  ptr=(struct FrameBufferFontDB *)data;
  if (ptr==NULL) return -1;
  if (buf==NULL) return 0;
  if (sze==0) return 0;
 
  for (i=0;i<sze;i++) if (!isspace(buf[i])) c++;

  if (ptr->buf !=NULL) o=strlen(ptr->buf);
  if (ptr->buf==NULL) ptr->buf=malloc(c+1);
  else {
    tmp=realloc(ptr->buf,o+c+1);
    if (tmp==NULL) return -1;
    ptr->buf=tmp;
  }
  if (ptr->buf==NULL) return -1;
  c=0;
  for (i=0;i<sze;i++) if (!isspace(buf[i])) {
    ptr->buf[o+c]=buf[i];
    c++;
  }
  ptr->buf[o+c]=0;
  return 0;
}


int FrameBufferFontDBStart(char *name,char end,
		  int atnum,char **atname,char **atval,
		  char *buf,int sze,void *data) {
  int i;
  struct FrameBufferFontDB *ptr;
  ptr=(struct FrameBufferFontDB *)data;
  if (strcmp(name,"font") !=0) return 0;
  if (ptr==NULL) return -1;
  if (ptr->buf !=NULL) free(ptr->buf);
  ptr->buf=NULL;
  ptr->flg=0;
  for (i=0;i<atnum;i++) if (strcmp(atname[i],"default")==0) ptr->flg=1;
  return 0;
}

int FrameBufferFontDBEnd(char *name,char *buf,int sze,void *data) {
  FILE *fp;
  char *fname=NULL;
  int s;
  struct FrameBufferFont *font=NULL;
  struct FrameBufferFontDB *ptr;
  ptr=(struct FrameBufferFontDB *)data;

  if (strcmp(name,"path")==0) {
    if (ptr->path !=NULL) free(ptr->path);
    ptr->path=ptr->buf;
    ptr->buf=NULL;
    return 0;
  }

  if (strcmp(name,"env")==0) {
    char *tmp;
    tmp=getenv(ptr->buf);
    if (tmp !=NULL) {
      if (ptr->path !=NULL) free(ptr->path);
      ptr->path=malloc(strlen(tmp)+1);
      if (ptr->path !=NULL) strcpy(ptr->path,tmp);
    }
    free(ptr->buf);
    ptr->buf=NULL;
    return 0;
  }


  if (strcmp(name,"font") !=0) return 0;
  if (ptr==NULL) return -1;
  if (ptr->buf==NULL) return 0;

  s=strlen(ptr->buf)+1;
  if (ptr->path !=NULL) s+=strlen(ptr->path)+1;
  fname=malloc(s);
  if (fname==NULL) return -1;
  if (ptr->path !=NULL) sprintf(fname,"%s/%s",ptr->path,ptr->buf);
  else sprintf(fname,"%s",ptr->buf);
 
  fp=fopen(fname,"r");
  free(fname);
  free(ptr->buf);
  ptr->buf=NULL;

  if (fp !=NULL) {
    font=FrameBufferLoadFont(fp);
    fclose(fp);
  }
  if (font==NULL) return -1;

  if (ptr->font==NULL) ptr->font=malloc(sizeof(struct FrameBufferFont *));
  else {
    struct FrameBufferFont **tmp;
    tmp=realloc(ptr->font,sizeof(struct FrameBufferFont *)*(ptr->num+1));
    if (tmp==NULL) return -1;
    ptr->font=tmp;
  }
  if (ptr->font==NULL) return -1;
  ptr->font[ptr->num]=font;
  if (ptr->flg) ptr->dfont=font;
  ptr->num++;

  return 0;
}


void FrameBufferFontDBFree(struct FrameBufferFontDB *ptr) {
  int i;
  if (ptr==NULL) return;
  if (ptr->path !=NULL) free(ptr->path);
  if (ptr->buf !=NULL) free(ptr->buf);
  if (ptr->font !=NULL) {
    for (i=0;i<ptr->num;i++) FrameBufferFreeFont(ptr->font[i]);
    free(ptr->font);
  }
  free(ptr);  
   
}

struct FrameBufferFontDB *FrameBufferFontDBLoad(FILE *fp) {
  int s=0;
  int i;
  char lbuf[256];
  struct XMLdata *xmldata=NULL;
  struct FrameBufferFontDB *ptr;


  if (fp==NULL) return NULL;  
  ptr=malloc(sizeof(struct FrameBufferFontDB));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->path=NULL;
  ptr->buf=NULL;
  ptr->font=NULL;
  ptr->dfont=NULL;
  
  xmldata=XMLMake();
  if (xmldata==NULL) {
    free(ptr);
    return NULL;
  }

  XMLSetStart(xmldata,FrameBufferFontDBStart,ptr);
  XMLSetEnd(xmldata,FrameBufferFontDBEnd,ptr);
  XMLSetText(xmldata,FrameBufferFontDBText,ptr);

  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }
  XMLFree(xmldata);
  if (s !=0) { 
    if (ptr->path !=NULL) free(ptr->path);
    if (ptr->buf !=NULL) free(ptr->buf);   
    if (ptr->font !=NULL) for (i=0;i<ptr->num;i++) FrameBufferFreeFont(ptr->font[i]);
    free(ptr->font);
    free(ptr);
    return NULL;
  }
  return ptr;
}


struct FrameBufferFont *FrameBufferFontDBFind(struct FrameBufferFontDB *ptr,char *name,int sze) {
  int n;
  int a,b;
  struct FrameBufferFont *fnt=NULL;
  struct FrameBufferFont *tst=NULL;
   
  if (ptr==NULL) return NULL;

  for (n=0;n<ptr->num;n++) {
    tst=ptr->font[n];
    if (strcmp(tst->name,name) !=0) continue;
    if (tst->sze==sze) {
      fnt=tst;
      break;
    }
    if (fnt==NULL) {
      fnt=tst;    
      continue;
    }
    a=fabs(tst->sze-sze);
    b=fabs(fnt->sze-sze);
    if (a<b) fnt=tst;
  }
  if (fnt==NULL) return ptr->dfont;
  return fnt;

}
 
