/* xml.c
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
#include "rxml.h"
#include "rfbuffer.h"





int FrameBufferXMLNameText(char *buf,int sze,void *data) {

  char **ptr;
  char *name=NULL;
  int o=0,c=0,i=0;
  char *tmp=NULL;

  if (buf==NULL) return 0;
  if (sze==0) return 0;

  for (i=0;i<sze;i++) if ((buf[i] !=10) && (buf[i] !=13)) c++;

  ptr=(char **) data;
  if (ptr==NULL) return -1;
  if (*ptr !=NULL) o=strlen(*ptr);
  if (*ptr==NULL) *ptr=malloc(c+1);
  else {
    tmp=realloc(*ptr,o+c+1);
    if (tmp==NULL) return -1;
    *ptr=tmp;
  }
  name=*ptr;
  if (name==NULL) return -1;
  c=0;
  for (i=0;i<sze;i++) if ((buf[i] !=10) && (buf[i] !=13)) {
    name[o+c]=buf[i];
    c++;
  }
  name[o+c]=0;
  return 0;
}

int FrameBufferXMLBufferText(char *buf,int sze,void *data) {
  struct FrameBufferXMLBuffer *ptr;
  int i=0,s=0;
  unsigned int val;
  
  ptr=(struct FrameBufferXMLBuffer *) data;
  if (ptr==NULL) return -1;
  if (ptr->ptr==NULL) return -1;
  for (i=0;i<sze;i++) {
    if (isalnum(buf[i])) {
      ptr->num[2+ptr->c]=buf[i];
      ptr->c++;
      if (ptr->c==2) { /* convert number */
        s=sscanf(ptr->num,"%x",&val);
        if (s !=1) return -1;
        if (ptr->off>=ptr->sze) return -1;
	ptr->ptr[ptr->off]=val;
        ptr->off++;
        ptr->c=0;
      }
    }
  }
  return 0;
}

int FrameBufferXMLBufferEnd(char *name,char *buf,int sze,void *data) {
  struct FrameBufferXMLBuffer *ptr;
  ptr=(struct FrameBufferXMLBuffer *) data;
  if (ptr->off !=ptr->sze) return -1;
   XMLSetText(ptr->xml,NULL,NULL);
   XMLSetEnd(ptr->xml,FrameBufferXMLEnd,ptr->data);
  if (data !=NULL) free(data);
  return 0;
}

int FrameBufferXMLStart(char *name,char end,
			int atnum,char **atname,char **atval,
			char *buf,int sze,void *data) {
  int i;
  int wdt=0,hgt=0,depth=0;
  struct FrameBufferXML *ptr;
  struct FrameBuffer *img;
  ptr=(struct FrameBufferXML *) data;

  if (ptr==NULL) return -1;
  
  if (strcmp(name,"img")==0) {
    int d=1,s=0;
    for (i=0;i<atnum;i++) {
      if ((strcmp(atname[i],"width")==0) && (atval[i] !=NULL))
	wdt=atoi(atval[i]);
      else if ((strcmp(atname[i],"height")==0) && (atval[i] !=NULL))
        hgt=atoi(atval[i]);
      else if ((strcmp(atname[i],"depth")==0) && (atval[i] !=NULL))
        depth=atoi(atval[i]);
    }

    *(ptr->img)=malloc(sizeof(struct FrameBuffer));
    img=*(ptr->img);
    if (img==NULL) return -1;

    img->name=NULL;
    img->wdt=wdt;
    img->hgt=hgt;
    img->depth=depth;
    img->user.pixel=NULL;
    img->user.data=NULL;

    if (depth !=8) d=3;
    if (s==0) img->img=malloc(wdt*hgt*d);
    if (img->img==NULL) s=-1;
    if (s==0) img->msk=malloc(wdt*hgt);
    if (img->msk==NULL) s=-1;

    if (s !=0) {
      if (img->img !=NULL) free(img->img);
      if (img->msk !=NULL) free(img->msk);
      free(img);
      return -1;
    }
    ptr->state=0;;
    return 0;
  } 
  img=*(ptr->img);
  if (img==NULL) return -1;
 
  if (strcmp(name,"name")==0) {
    if (img->name !=NULL) free(img->name);
    img->name=NULL;
    XMLSetText(ptr->xml,FrameBufferXMLNameText,&img->name);
    return 0;
  } 

  if (strcmp(name,"i")==0) {
    struct FrameBufferXMLBuffer *buf;
    int d=1;
    if (img->depth !=8) d=3;
    memset(img->img,0,d*img->wdt*img->hgt);
    buf=malloc(sizeof(struct FrameBufferXMLBuffer));
    buf->xml=ptr->xml;
    buf->data=data;
    buf->ptr=img->img;
    buf->off=0;
    buf->sze=d*img->wdt*img->hgt;
    buf->c=0;
    buf->num[0]='0';
    buf->num[1]='x';
    buf->num[4]=0;
    ptr->state=ptr->state | 0x01;
    XMLSetText(ptr->xml,FrameBufferXMLBufferText,buf);
    XMLSetEnd(ptr->xml,FrameBufferXMLBufferEnd,buf);
    return 0;
  }
  
  if (strcmp(name,"a")==0) {
    struct FrameBufferXMLBuffer *buf;
    memset(img->msk,0,img->wdt*img->hgt);
    buf=malloc(sizeof(struct FrameBufferXMLBuffer));
    buf->xml=ptr->xml;
    buf->data=data;
    buf->ptr=img->msk;
    buf->off=0;
    buf->sze=img->wdt*img->hgt;
    buf->c=0;
    buf->num[0]='0';
    buf->num[1]='x';
    buf->num[4]=0;
    ptr->state=ptr->state | 0x02;
    XMLSetText(ptr->xml,FrameBufferXMLBufferText,buf);
    XMLSetEnd(ptr->xml,FrameBufferXMLBufferEnd,buf);
    return 0;
  }  
  return 0;
}  

int FrameBufferXMLEnd(char *name,char *buf,int sze,void *data) {
  struct FrameBufferXML *ptr;
  ptr=(struct FrameBufferXML *) data;
  if (strcmp(name,"img") !=0) return 0;
  if (ptr->state !=0x03) return -1;
  return 0;
}


int FrameBufferSaveXML(struct FrameBuffer *ptr,
                       int (*text)(char *,int,void *),void *data ) {
  int o,i,j=0,s=0;
  int d=1;
  char quot='"';
  char lbuf[256];

  if (ptr==NULL) return -1;
  if (text==NULL) return -1;
  if (ptr->img==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;

  sprintf(lbuf,"<img width=%c%d%c height=%c%d%c depth=%c%d%c>\n",
	  quot,ptr->wdt,quot,quot,ptr->hgt,quot,quot,ptr->depth,quot);
 
  s=(*text)(lbuf,strlen(lbuf),data);
  if (s !=0) return -1;

  if (ptr->name !=NULL) {
    sprintf(lbuf,"<name>");
    s=(*text)(lbuf,strlen(lbuf),data);
    if (s !=0) return -1;
    s=(*text)(ptr->name,strlen(ptr->name),data);
    if (s !=0) return -1;
    sprintf(lbuf,"</name>\n");
    s=(*text)(lbuf,strlen(lbuf),data);
    if (s !=0) return -1;
  }

  if (ptr->depth !=8) d=3;

  o=ptr->wdt*ptr->hgt;
 
  sprintf(lbuf,"<i>\n");
  s=(*text)(lbuf,strlen(lbuf),data);
  if (s !=0) return -1;
  for (i=0;i<o*d;i++) {
    sprintf(lbuf,"%.2x",ptr->img[i]);
    s=(*text)(lbuf,strlen(lbuf),data);
    if (s !=0) return -1;
    j++;
    if ((j % 32)==0) { 
      sprintf(lbuf,"\n");
      s=(*text)(lbuf,strlen(lbuf),data);
      if (s !=0) return -1;
    }
  }
  sprintf(lbuf,"\n</i>\n");
  s=(*text)(lbuf,strlen(lbuf),data);
  if (s !=0) return -1;
  
  sprintf(lbuf,"<a>\n");
  s=(*text)(lbuf,strlen(lbuf),data);
  if (s !=0) return -1;
  j=0;
  for (i=0;i<o;i++) {
    sprintf(lbuf,"%.2x",ptr->msk[i]);
    s=(*text)(lbuf,strlen(lbuf),data);
    if (s !=0) return -1;
    j++;
    if ((j % 32)==0) {
      sprintf(lbuf,"\n");
      s=(*text)(lbuf,strlen(lbuf),data);
      if (s !=0) return -1;
    }
  }
  sprintf(lbuf,"\n</a>\n");
  s=(*text)(lbuf,strlen(lbuf),data);
  if (s !=0) return -1;
  sprintf(lbuf,"</img>\n");
  s=(*text)(lbuf,strlen(lbuf),data);
  if (s !=0) return -1;
  return 0;
}





