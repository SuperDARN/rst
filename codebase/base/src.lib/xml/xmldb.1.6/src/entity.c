/* entity.c
   ======== 
   Author R.J.Barnes
*/

/*
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
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"



struct Entitydata *EntityMake() {
  struct Entitydata *ptr;
  ptr=malloc(sizeof(struct Entitydata));
  ptr->entity.num=0;
  ptr->entity.buf=NULL;
  ptr->entity.state=NULL;
  ptr->symbol.num=0;
  ptr->symbol.buf=NULL;
  ptr->symbol.state=NULL;

  ptr->text.func=NULL;
  ptr->text.data=NULL;

  ptr->encode.func=NULL;
  ptr->encode.data=NULL;
  
  ptr->decode.func=NULL;
  ptr->decode.data=NULL;
 
  ptr->stp=10;
  ptr->sze=0;
  ptr->max=10;
  ptr->buf=malloc(ptr->max);
  if (ptr->buf==NULL) {
    free(ptr);
    return NULL;
  }
  return ptr;
}

void EntityFree(struct Entitydata *ptr) {
  int i;
  if (ptr==NULL) return;
  if (ptr->entity.buf !=NULL) {
    for (i=0;i<ptr->entity.num;i++) 
      if (ptr->entity.buf[i] !=NULL) XMLDBFreeBuffer(ptr->entity.buf[i]);
    free(ptr->entity.buf);
  }
  if (ptr->entity.state !=NULL) free(ptr->entity.state);

  if (ptr->symbol.buf !=NULL) {
    for (i=0;i<ptr->symbol.num;i++) 
      if (ptr->symbol.buf[i] !=NULL) XMLDBFreeBuffer(ptr->symbol.buf[i]);
    free(ptr->symbol.buf);
  }
 if (ptr->symbol.state !=NULL) free(ptr->symbol.state);

  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}

void EntityFreeEntity(struct Entitydata *ptr) {
  int i;
  if (ptr==NULL) return;
  if (ptr->entity.buf !=NULL) {
    for (i=0;i<ptr->entity.num;i++) 
      if (ptr->entity.buf[i] !=NULL) XMLDBFreeBuffer(ptr->entity.buf[i]);
    free(ptr->entity.buf);
    ptr->entity.buf=NULL;
    ptr->entity.num=0;
  }
  if (ptr->entity.state !=NULL) free(ptr->entity.state);
  ptr->entity.state=NULL;
}

void EntityFreeSymbol(struct Entitydata *ptr) {
  int i;
  if (ptr==NULL) return;
  if (ptr->symbol.buf !=NULL) {
    for (i=0;i<ptr->entity.num;i++) 
      if (ptr->symbol.buf[i] !=NULL) XMLDBFreeBuffer(ptr->symbol.buf[i]);
    free(ptr->symbol.buf);
    ptr->symbol.buf=NULL;
    ptr->symbol.num=0;
  }
  if (ptr->symbol.state !=NULL) free(ptr->symbol.state);
  ptr->symbol.state=NULL;
}





int EntitySetText(struct Entitydata *ptr,
		  int (*text)(char *,int,
                         void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->text.func=text;
  ptr->text.data=data;
  return 0;
} 

int EntitySetDecodeTrap(struct Entitydata *ptr,
		  int (*text)(char *,int,
                         void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->decode.func=text;
  ptr->decode.data=data;
  return 0;
} 

int EntitySetEncodeTrap(struct Entitydata *ptr,
		  int (*text)(char *,int,
                         void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->encode.func=text;
  ptr->encode.data=data;
  return 0;
} 


int EntityAddEntity(struct Entitydata *ptr,
                    struct XMLDBbuffer *entity) {
  struct XMLDBbuffer **tmp;
  int *itmp;
  int n;
  if (ptr==NULL) return -1;

  n=ptr->entity.num;
 
  if (ptr->entity.buf==NULL) 
       ptr->entity.buf=malloc(sizeof(struct XMLDBbuffer *));
  else {
    tmp=realloc(ptr->entity.buf,
                sizeof(struct XMLDBbuffer *)*(n+1));
    if (tmp==NULL) return -1;
    ptr->entity.buf=tmp;
  } 

  if (ptr->entity.buf==NULL) return -1;

  if (ptr->entity.state==NULL) ptr->entity.state=malloc(sizeof(int));
  else {
    itmp=realloc(ptr->entity.state,sizeof(int)*(n+1)); 
    if (itmp==NULL) return -1;
    ptr->entity.state=itmp;
  }
  if (ptr->entity.state==NULL) return -1;

  ptr->entity.buf[n]=NULL;
  ptr->entity.state[n]=0;

  if (entity !=NULL) ptr->entity.buf[n]=XMLDBCopyBuffer(entity);
  ptr->entity.num++;
  return 0;
}


int EntityAddSymbol(struct Entitydata *ptr,
                    struct XMLDBbuffer *symbol) {
  int n;
  struct XMLDBbuffer **tmp;
  int *itmp;
  if (ptr==NULL) return -1;

  n=ptr->symbol.num;
  if (ptr->symbol.buf==NULL) 
    ptr->symbol.buf=malloc(sizeof(struct XMLDBbuffer *));
  else {
    tmp=realloc(ptr->symbol.buf,sizeof(struct XMLDBbuffer *)*(n+1)); 
    if (tmp==NULL) return -1;
    ptr->symbol.buf=tmp;
  }
  if (ptr->symbol.buf==NULL) return -1;
  if (ptr->symbol.state==NULL) ptr->symbol.state=malloc(sizeof(int));
  else {
     itmp=realloc(ptr->symbol.state,sizeof(int)*(n+1)); 
     if (itmp==NULL) return -1;
     ptr->symbol.state=itmp;
  }
  if (ptr->symbol.state==NULL) return -1;

  ptr->symbol.buf[n]=NULL;
  ptr->symbol.state[n]=0;
  if (symbol !=NULL) ptr->symbol.buf[n]=XMLDBCopyBuffer(symbol);

  ptr->symbol.num++;
  return 0;
}

int EntityAddEntityString(struct Entitydata *ptr,char *str) {
  struct XMLDBbuffer *tmp=NULL;
  int s=0; 
  if (str !=NULL) {
    tmp=XMLDBMakeBuffer(100);
    if (tmp==NULL) return -1;
    s=XMLDBAddBuffer(tmp,str,strlen(str));
  }
  if (s==0) s=EntityAddEntity(ptr,tmp);
  if (tmp !=NULL) XMLDBFreeBuffer(tmp);
  return s;
}


int EntityAddSymbolString(struct Entitydata *ptr,char *str) {
  struct XMLDBbuffer *tmp=NULL;
  int s=0; 
  if (str !=NULL) {
    tmp=XMLDBMakeBuffer(100);
    if (tmp==NULL) return -1;
    s=XMLDBAddBuffer(tmp,str,strlen(str));
  }
  if (s==0) s=EntityAddSymbol(ptr,tmp);
  if (tmp !=NULL) XMLDBFreeBuffer(tmp);
  return s;
}

int EntityAddEntityStringArray(struct Entitydata *ptr,char **str) {
  struct XMLDBbuffer *tmp=NULL;
  int s=0,n=0;

  for (n=0;str[n] !=NULL;n++) {
    
    tmp=XMLDBMakeBuffer(100);
    if (tmp==NULL) return -1;
    s=XMLDBAddBuffer(tmp,str[n],strlen(str[n]));
    if (s==0) s=EntityAddEntity(ptr,tmp);
    XMLDBFreeBuffer(tmp);
    if (s !=0) break;
  }
  
  return s;
}

int EntityAddSymbolStringArray(struct Entitydata *ptr,char **str) {
  struct XMLDBbuffer *tmp=NULL;
  int s=0,n=0;

  for (n=0;str[n] !=NULL;n++) {
    
    tmp=XMLDBMakeBuffer(100);
    if (tmp==NULL) return -1;
    s=XMLDBAddBuffer(tmp,str[n],strlen(str[n]));
    if (s==0) s=EntityAddSymbol(ptr,tmp);
    XMLDBFreeBuffer(tmp);
    if (s !=0) break;
  }
  
  return s;
}




int EntityDecode(char *buf,int sze,void *data) {
  struct Entitydata *ptr=NULL;
  struct XMLDBbuffer *entity=NULL;
  struct XMLDBbuffer *symbol=NULL;
  int i,j,c,st,status=0;
  int match=0;

  ptr=(struct Entitydata *) data;
  if (ptr==NULL) return -1;

  if (buf==NULL) { /* flush the buffer */
     char *tmp;
     if (ptr->sze==0) return 0;
     status=(ptr->text.func)(ptr->buf,ptr->sze,
			   ptr->text.data);
     ptr->sze=0;
     ptr->max=ptr->stp;
     tmp=realloc(ptr->buf,ptr->max);
     if (tmp==NULL) return -1;
     ptr->buf=tmp;
     return status;
  }

  for (i=0;i<sze;i++) {
    c=buf[i];
    match=0;
    for (j=0;j<ptr->entity.num;j++) {
      st=ptr->entity.state[j];
      entity=ptr->entity.buf[j];
      if (entity==NULL) continue;
      if ((ptr->symbol.buf !=NULL) && (j<ptr->symbol.num)) 
         symbol=ptr->symbol.buf[j];
      else symbol=NULL;

      if (c==entity->buf[st]) {
	ptr->entity.state[j]++;
        match=1;
      } else ptr->entity.state[j]=0;
      st=ptr->entity.state[j];
      if (entity->sze==st) break;
    }   
    if (j<ptr->entity.num) {
      char *tmp;
      if (ptr->decode.func !=NULL)
         status=(ptr->decode.func)(entity->buf,entity->sze,
                                  ptr->decode.data);
      if ((status==0) && (symbol !=NULL)) 
         status=(ptr->text.func)(symbol->buf,symbol->sze,
                                  ptr->text.data);
      if (status !=0) break;
      ptr->entity.state[j]=0;
      ptr->sze=0;
      ptr->max=ptr->stp;
      tmp=realloc(ptr->buf,ptr->max);
      if (tmp==NULL) {
        status=-1;
        break;
      }
      ptr->buf=tmp;
      continue;
    }

    if (match==1) {
      ptr->buf[ptr->sze]=c;
      ptr->sze++;
      if (ptr->sze==ptr->max) {
        char *tmp;
        ptr->max+=ptr->stp;
        tmp=realloc(ptr->buf,ptr->max);
        if (ptr->buf==NULL) {
          status=-1;
          break;
	}
        ptr->buf=tmp;
      }
      continue;
    } else if (ptr->sze>0) {
      char *tmp;
      status=(ptr->text.func)(ptr->buf,ptr->sze,
                               ptr->text.data);
      if (status !=0) break;
      ptr->sze=0;
      ptr->max=ptr->stp;
      tmp=realloc(ptr->buf,ptr->max);
      if (tmp==NULL) { 
        status=-1;
        break;
      }
      ptr->buf=tmp;

    }
    status=(ptr->text.func)(buf+i,1,ptr->text.data);
    if (status !=0) break;
  }
  
 
  return status;
}

int EntityEncode(char *buf,int sze,void *data) {

  struct Entitydata *ptr=NULL;
  struct XMLDBbuffer *entity=NULL;
  struct XMLDBbuffer *symbol=NULL;

  int i,j,c,st,status=0;
  int match=0;
  ptr=(struct Entitydata *) data;

  if (ptr==NULL) return -1;
 
  if (buf==NULL) { /* flush the buffer */
     char *tmp;
     if (ptr->sze==0) return 0;
     status=(ptr->text.func)(ptr->buf,ptr->sze,
			   ptr->text.data);
     ptr->sze=0;
     ptr->max=ptr->stp;
     tmp=realloc(ptr->buf,ptr->max);
     if (tmp==NULL) return -1;
     ptr->buf=tmp;
     return status;
  }

  for (i=0;i<sze;i++) {
    c=buf[i];
  

    match=0;
    
    for (j=0;j<ptr->symbol.num;j++) {
      st=ptr->entity.state[j];
      symbol=ptr->symbol.buf[j];
      if (symbol==NULL) continue;
      if ((ptr->entity.buf !=NULL) && (j<ptr->entity.num))
         entity=ptr->entity.buf[j];
      else entity=NULL;

      if (c==symbol->buf[st]) {
	ptr->symbol.state[j]++;
        match=1;
      } else ptr->symbol.state[j]=0;
      st=ptr->symbol.state[j];
      if (symbol->sze==st) break;
    }   
    if (j<ptr->symbol.num) {
      char *tmp;
      if (ptr->encode.func !=NULL)
         status=(ptr->encode.func)(symbol->buf,symbol->sze,
                                  ptr->encode.data);
      if ((status==0) && (entity !=NULL)) 
         status=(ptr->text.func)(entity->buf,entity->sze,ptr->text.data);
      if (status !=0) break;
      ptr->symbol.state[j]=0;
      ptr->sze=0;
      ptr->max=ptr->stp;
      tmp=realloc(ptr->buf,ptr->max);
      if (tmp==NULL) {
        status=-1;
        break;
      }
      ptr->buf=tmp;
      continue;
    }

    if (match==1) {
      ptr->buf[ptr->sze]=c;
      ptr->sze++;
      if (ptr->sze==ptr->max) {
        char *tmp;
        ptr->max+=ptr->stp;
        tmp=realloc(ptr->buf,ptr->max);
        if (tmp==NULL) {
          status=-1;
          break;
	}
        ptr->buf=tmp;
      }
      continue;
    } else if (ptr->sze>0) {
      char *tmp;
      status=(ptr->text.func)(ptr->buf,ptr->sze,
                               ptr->text.data);
      if (status !=0) break;
      ptr->sze=0;
      ptr->max=ptr->stp;
      tmp=realloc(ptr->buf,ptr->max);
      if (tmp==NULL) {
        status=-1;
        break;
      }
      ptr->buf=tmp;
    }
    status=(ptr->text.func)(buf+i,1,ptr->text.data);
    if (status !=0) break;
  }
  return status;
}




int EntityBuffer(char *buf,int sze,void *data) {
  struct XMLDBbuffer *dst=NULL;

  dst=(struct XMLDBbuffer *)data;
  if (dst==NULL) return -1;
  return XMLDBAddBuffer(dst,buf,sze);
  
}


struct XMLDBbuffer *EntityDecodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src) {
  int s=0;
  struct XMLDBbuffer *dst=NULL;
  if (src==NULL) return NULL;
  dst=XMLDBMakeBuffer(src->stp);

  if (dst==NULL) return NULL;
  
  EntitySetText(ptr,EntityBuffer,dst);
  s=EntityDecode(src->buf,src->sze,ptr);
  if (s==0) s=EntityDecode(NULL,0,ptr);  
  if (s !=0) {
    XMLDBFreeBuffer(dst);
    return NULL;
  }
  
  return dst;
}

struct XMLDBbuffer *EntityEncodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src) {
  int s=0;
  struct XMLDBbuffer *dst=NULL;
  if (src==NULL) return NULL;
  dst=XMLDBMakeBuffer(src->stp);
  if (dst==NULL) return NULL;
  EntitySetText(ptr,EntityBuffer,dst);
  s=EntityEncode(src->buf,src->sze,ptr);
  if (s==0) s=EntityEncode(NULL,0,ptr);
  if (s !=0) {
    XMLDBFreeBuffer(dst);
    return NULL;
  }
  return dst;
}





