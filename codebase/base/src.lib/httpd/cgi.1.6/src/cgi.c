/* cgi.c
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
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "rcgi.h"



char *CGIGetString(struct CGIarg *ptr) {
  char *txt;
  if (ptr->value.txt.buf==NULL) return NULL;
  txt=malloc(ptr->value.txt.sze+1);
  memcpy(txt,ptr->value.txt.buf,ptr->value.txt.sze);
  txt[ptr->value.txt.sze]=0;
  return txt;
}


char *CGIGetRawString(struct CGIarg *ptr) {
  char *raw;
  if (ptr->value.raw.buf==NULL) return NULL;
  raw=malloc(ptr->value.raw.sze+1);
  memcpy(raw,ptr->value.raw.buf,ptr->value.raw.sze);
  raw[ptr->value.raw.sze]=0;
  return raw;
}

char *CGIMakeURLString(char *inp,int sze) {
  char hex[3];
  char *out=NULL; 
  int i=0,j=0,sz;
  
  out=malloc(sze+1);
  sz=sze+1;
  for (i=0;i<sze;i++) {
    if ((isalnum(inp[i])) || 
        (inp[i]=='-') || (inp[i]=='_')) out[j]=inp[i];
    else if (inp[i]==' ') out[j]='+';
    else {
      sz+=2;
      out=realloc(out,sz);
      sprintf(hex,"%.2x",(unsigned char) inp[i]);
      out[j]='%';
      out[j+1]=toupper(hex[0]);
      out[j+2]=toupper(hex[1]);
      j+=2;
    }
    j++;
  }
  out[j]=0;
  return out;
}

struct CGIdata *CGIMake() {
  struct CGIdata *ptr;
  ptr=malloc(sizeof(struct CGIdata));
  if (ptr==NULL) return 0;
  ptr->state=0;
  ptr->bnd=NULL;
  ptr->num=0;
  ptr->arg=NULL;
  ptr->stp=100;
  ptr->sze=0;
  ptr->max=100;
  ptr->buf=malloc(ptr->max);
  if (ptr->buf==NULL) {
    free(ptr);
    return NULL;
  }

  ptr->text.func=NULL;
  ptr->text.data=NULL;

  ptr->mime.state=0;
  ptr->mime.stp=100;
  ptr->mime.sze=0;
  ptr->mime.max=100;
  ptr->cnt=0;
  ptr->mime.buf=malloc(ptr->mime.max);
  if (ptr->mime.buf==NULL) {
    free(ptr->buf);
    free(ptr);
    return NULL;
  }
  return ptr;
}


void CGIFree(struct CGIdata *ptr) {
  int i;
  struct CGIarg *a;
  if (ptr==NULL) return;
  if (ptr->mime.buf !=NULL) free(ptr->mime.buf);
  if (ptr->buf !=NULL) free(ptr->buf);
 
  if (ptr->arg !=NULL) {
    for (i=0;i<ptr->num;i++) {
      a=ptr->arg[i];
      if (a !=NULL) {
        if (a->name !=NULL) free(a->name);
        if (a->fname !=NULL) free(a->fname);
        if (a->value.raw.buf !=NULL) free(a->value.raw.buf);
        if (a->value.txt.buf !=NULL) free(a->value.txt.buf);
        free (a);
      }
    }
    free(ptr->arg);
  }
  free(ptr);
  return;
}

int CGISetText(struct CGIdata *ptr,int (*text)(struct CGIarg *,char *,
                                                int,void *),void *data) {

  if (ptr==NULL) return -1;
  ptr->text.func=text;
  ptr->text.data=data;
  return 0;
}

int CGIFindInBuf(char *buf,int sze,char *str) {
  int i=0; 
  int j=0;
  int x,y;

  if (buf==NULL) return -1;
  if (str==NULL) return -1;

  while (i<sze) {
    x=buf[i];
    y=str[j];
    if (x==y) j++;
    else j=0;
    if (str[j]==0) break;
    i++;
  }
  if (i==sze) return -1;
  return i-j+1;
}

int CGIInterpretPart(struct CGIdata *ptr,char *buf,int sze) {

  struct CGIarg *a=NULL;
  int n,s=0;
  n=ptr->num;  
  if (ptr==NULL) return -1; 
  if ((buf==NULL) && (n>0)) {
    int i=0,j=0;
    int sz=0;
    char hex[3];

    /* encode the argument */
    a=ptr->arg[n-1];    
  
    if (ptr->text.func !=NULL) s=(ptr->text.func)(a,NULL,0,
                                                    ptr->text.data);

    if ((a->value.txt.buf !=NULL) && (a->value.txt.sze>0)) {
      char *inp,*out;
      
      inp=a->value.txt.buf;
      out=malloc(a->value.txt.sze);
      sz=a->value.txt.sze;
      for (i=0;i<a->value.txt.sze;i++) {
        if ((isalnum(inp[i])) || 
        (inp[i]=='-') || (inp[i]=='_')) out[j]=inp[i];
        else if (inp[i]==' ') out[j]='+';
        else {
          sz+=2;
          out=realloc(out,sz);
          sprintf(hex,"%.2x",inp[i]);
          out[j]='%';
          out[j+1]=toupper(hex[0]);
          out[j+2]=toupper(hex[1]);
          j+=2;
	}
        j++;
      }
      a->value.raw.sze=sz;
      a->value.raw.buf=out;     
    }

    return 0;  
  }

  if (ptr->mime.state==3) { /* first time through */
    char *nid="name=";
    char *fid="filename=";
    int j,p,nl,fl;

    nl=strlen(nid);
    fl=strlen(fid);
    if (ptr->arg==NULL) ptr->arg=malloc(sizeof(struct CGIarg *));
    else ptr->arg=realloc(ptr->arg,sizeof(struct CGIarg *)*(n+1));
    if (ptr->arg==NULL) return -1;

    a=malloc(sizeof(struct CGIarg));
    if (a==NULL) return -1;
  
    ptr->arg[n]=a;
    ptr->num++;

    p=CGIFindInBuf(ptr->mime.buf,ptr->mime.sze,nid);

    if (p==-1) return -1;
    p=p+nl+1;
    for (j=p;j<ptr->mime.sze;j++) if (ptr->mime.buf[j]=='"') break;
    if (j>=ptr->mime.sze) return -1;

    a->name=malloc(j-p+1);
    memcpy(a->name,ptr->mime.buf+p,j-p);
    a->name[j-p]=0;

    a->value.raw.buf=NULL;
    a->value.txt.buf=NULL;
    a->value.raw.sze=0;
    a->value.txt.sze=0;
  
    a->type=0;
    a->fname=NULL;

    /* determine type */
  
    p=CGIFindInBuf(ptr->mime.buf,ptr->mime.sze,fid);
    if (p !=-1) {
      a->type=1;
      p=p+fl+1;
      for (j=p;j<ptr->mime.sze;j++) if (ptr->mime.buf[j]=='"') break;
      if (j>=ptr->mime.sze) return -1;

      a->fname=malloc(j-p+1);
      memcpy(a->fname,ptr->mime.buf+p,j-p);
      a->fname[j-p]=0;
    }
  } else if (n>0) a=ptr->arg[n-1];
  if (a==NULL) return -1;

  
  if (a->type==1) s=1;
  if (ptr->text.func !=NULL) s=(ptr->text.func)(a,buf,sze,ptr->text.data);
  
  if ((s==0) && (sze>0)) { /* go ahead and store the values */    
    if (a->value.txt.buf==NULL) a->value.txt.buf=malloc(sze);
    else a->value.txt.buf=realloc(a->value.txt.buf,a->value.txt.sze+sze);  
    memcpy(a->value.txt.buf+a->value.txt.sze,buf,sze);
    a->value.txt.sze+=sze;
  }
  
  return 0;

}



int CGIDecodePart(struct CGIdata *ptr,char *buf,int sze) {
  int i,c;

  if (ptr==NULL) return -1;
  if (buf==NULL) { 
    /* end of segment */

    CGIInterpretPart(ptr,NULL,0);    
    ptr->mime.state=0;
    ptr->mime.sze=0;
    ptr->mime.max=ptr->mime.stp;
    ptr->mime.buf=realloc(ptr->mime.buf,ptr->mime.max);
    return 0;
  }

  if (ptr->mime.state<3) {
    for (i=0;i<sze;i++) {
      c=buf[i];
      if (c==10) ptr->mime.state++;
      ptr->mime.buf[ptr->mime.sze]=c;
      ptr->mime.sze++;
      if (ptr->mime.sze>ptr->mime.max) {
        ptr->mime.max+=ptr->stp;
        ptr->mime.buf=realloc(ptr->mime.buf,ptr->mime.max);
        if (ptr->mime.buf==NULL) break;
      }
      if (ptr->mime.state>2) break;
    }
    if (ptr->mime.state>2) { /* we've got the mime type */
      if (i<sze) {
        CGIInterpretPart(ptr,buf+i+1,sze-i-1);
        ptr->mime.state++;
      }
      return 0;
    }
  }
  if (ptr->mime.state>2) { /* pass on to the handler */  
    CGIInterpretPart(ptr,buf,sze);
    if (ptr->mime.state==3) ptr->mime.state++; 
  }

  return 0;
}

int CGIDecodeMultiPart(struct CGIdata *ptr,char *type,char *buf,int sze) {
  int s=0,i,st,c;
  int match=0;
  if (ptr==NULL) return -1;
  if (buf==NULL) {
    if (ptr->sze>0) {
      int sze;
      sze=ptr->sze;
      s=CGIDecodePart(ptr,ptr->buf,sze);
      ptr->state=0;
      ptr->sze=0;
      ptr->max=ptr->stp;
      ptr->buf=realloc(ptr->buf,ptr->max);
      ptr->cnt=0;
    }
    if (ptr->bnd !=NULL) free(ptr->bnd);
    ptr->bnd=NULL;
    return s;
  }  
    
  if (ptr->bnd==NULL) { 
    char *bid="boundary=";
    int p;
    /* first time through, so find the boundary */
    p=CGIFindInBuf(type,strlen(type),bid);
    if (p==-1) return -1;
    p+=strlen(bid);
    if (p>=strlen(type)) return -1;
    ptr->bnd=malloc(strlen(type+p)+3);
    strcpy(ptr->bnd,"--");
    strcat(ptr->bnd,type+p); 
  }

  for (i=0;i<sze;i++) {
    c=buf[i];
    match=0;
    st=ptr->state;

    if ((ptr->bnd !=NULL) && (c==ptr->bnd[st])) {
      ptr->state++;
      st++;
      match=1;
    }
    st=ptr->state;
    if (ptr->bnd[st]==0) {
      if (ptr->cnt>0) s=CGIDecodePart(ptr,NULL,0);
      if (s !=0) break;
      ptr->state=0;
      ptr->sze=0;
      ptr->max=ptr->stp;
      ptr->buf=realloc(ptr->buf,ptr->max);
      if (ptr->cnt==0) { /* change boundary string */
        char nl[3]={0x0d,0x0a,0};
        char *bnd=NULL;
        bnd=malloc(strlen(ptr->bnd)+3);
        strcpy(bnd,nl);
        strcat(bnd,ptr->bnd);
        free(ptr->bnd);
        ptr->bnd=bnd;
      }
      ptr->cnt++;
      continue;
    }
    if (match==1) {
      ptr->buf[ptr->sze]=c;
      ptr->sze++;
      if (ptr->sze==ptr->max) {
        ptr->max+=ptr->stp;
        ptr->buf=realloc(ptr->buf,ptr->max);
        if (ptr->buf==NULL) break;
      }
      continue;
    } else if (ptr->sze>0) { /* pass on the data to the subhandler */
      s=CGIDecodePart(ptr,ptr->buf,ptr->sze);
      if (s !=0) break;
      ptr->state=0;
      ptr->sze=0;
      ptr->max=ptr->stp;
      ptr->buf=realloc(ptr->buf,ptr->max);
    }
    s=CGIDecodePart(ptr,buf+i,1);  
    if (s !=0) break;  
  }
  if (i<sze) return -1;
  return 0;
}

int CGIDecodeURLEncoded(struct CGIdata *ptr,char *type,char *buf,int sze) {
  int i,j,c=0,n;
  char hex[16]="0123456789ABCDEF";
  struct CGIarg *a; 

  if (ptr==NULL) return -1;  
  if (buf==NULL) {
    /* tidy up */

    if (ptr->sze !=0) {
        int sze=0;
        char *buf=NULL;
        n=ptr->num;
        if (ptr->arg==NULL) return -1;
        a=ptr->arg[n-1];
        if (a==NULL) return -1; 
        a->value.raw.buf=realloc(ptr->buf,ptr->sze);
        a->value.raw.sze=ptr->sze;
	
        buf=malloc(ptr->sze);
        for (j=0;j<ptr->sze;j++) {

          c=ptr->buf[j]; 
	  buf[sze]=c;
          if (c=='+') buf[sze]=' ';
          if (c=='%') { 
            int l,h;
            for (h=0;h<16;h++) if (hex[h]==ptr->buf[j+1]) break;
            for (l=0;l<16;l++) if (hex[l]==ptr->buf[j+2]) break;
            buf[sze]=h*16+l;
            j+=2;
          }
          sze++;
        }
        buf=realloc(buf,sze);
        
        a->value.txt.buf=buf;
        a->value.txt.sze=sze;
        ptr->state=0;
        ptr->sze=0;
        ptr->max=ptr->stp;
        ptr->buf=malloc(ptr->max);
    
    }
    return 0;
  }

  for (i=0;i<sze;i++) {
    c=buf[i];
    n=ptr->num;
    if ((ptr->state==0) && (c=='=')) {
      /* got the name of an argument */

      int sze=0;
      char *buf=NULL;

      if (ptr->sze==0) break; /* can't have an unamed argument */
     
      if (ptr->arg==NULL) ptr->arg=malloc(sizeof(struct CGIarg *));
      else ptr->arg=realloc(ptr->arg,sizeof(struct CGIarg *)*(n+1));

      if (ptr->arg==NULL) break;

      a=malloc(sizeof(struct CGIarg));
      if (a==NULL) break;
  
      ptr->arg[n]=a;
      ptr->num++;

      buf=malloc(ptr->sze);
      for (j=0;j<ptr->sze;j++) {
        c=ptr->buf[j]; 
	buf[sze]=c;
        if (c=='+') buf[sze]=' ';
        if (c=='%') { 
          int l,h;
          for (h=0;h<16;h++) if (hex[h]==ptr->buf[j+1]) break;
          for (l=0;l<16;l++) if (hex[l]==ptr->buf[j+2]) break;
          buf[sze]=h*16+l;
          j+=2;
        }
        sze++;
      }
      buf=realloc(buf,sze+1);
      buf[sze]=0;

      a->name=buf; 
      a->fname=NULL; 
      a->value.raw.buf=NULL;
      a->value.txt.buf=NULL;
      a->value.raw.sze=0;
      a->value.txt.sze=0; 
      a->type=0;
       
      ptr->state=1;
      ptr->sze=0;
      ptr->max=ptr->stp;
      free(ptr->buf);
      ptr->buf=malloc(ptr->max);
      continue;
    } else if ((ptr->state==1) && (c=='&')) {
      /* got value */

      int sze=0;
      char *buf=NULL;

      if (ptr->arg==NULL) break;
      a=ptr->arg[n-1];
      if (a==NULL) break;
      if (ptr->sze==0) {
        ptr->state=0;
        continue;
      }
      a->value.raw.buf=realloc(ptr->buf,ptr->sze);
      a->value.raw.sze=ptr->sze;
      
      buf=malloc(ptr->sze);
      for (j=0;j<ptr->sze;j++) {
        c=ptr->buf[j]; 
	buf[sze]=c;
        if (c=='+') buf[sze]=' ';
        if (c=='%') { 
          int l,h;
          for (h=0;h<16;h++) if (hex[h]==ptr->buf[j+1]) break;
          for (l=0;l<16;l++) if (hex[l]==ptr->buf[j+2]) break;
          buf[sze]=h*16+l;
          j+=2;
        }
        sze++;
      }
      buf=realloc(buf,sze);
      
      a->value.txt.buf=buf;
      a->value.txt.sze=sze;

      ptr->state=0;
      ptr->sze=0;
      ptr->max=ptr->stp;
      ptr->buf=malloc(ptr->max);
      continue;
    }

    if ((ptr->state==0) && (c=='&')) break;
    if ((ptr->state==0) && (c=='=')) break;
  
    ptr->buf[ptr->sze]=c;
    ptr->sze++;
    if (ptr->sze==ptr->max) {
      ptr->max+=ptr->stp;
      ptr->buf=realloc(ptr->buf,ptr->max);
      if (ptr->buf==NULL) break;
    }
 }
 if (i<sze) return -1;
 return 0;
}


int CGIDecode(struct CGIdata *ptr,char *type,char *buf,int sze) {
  if (ptr==NULL) return -1;
  if ((type !=NULL) && (strncmp(type,"multipart/form-data;",20)==0)) 
      return CGIDecodeMultiPart(ptr,type,buf,sze);
  return CGIDecodeURLEncoded(ptr,type,buf,sze); 
}


int CGIGet(struct CGIdata *ptr) {
  int s=0;
  int size=0;
  char *content_type=NULL;
  char *request_method=NULL;
 
  content_type=getenv("CONTENT_TYPE");
  request_method=getenv("REQUEST_METHOD");
  if (request_method==NULL) return -1; /* no request method */

  if (strcmp(request_method,"GET")==0) {
    char *query_string=NULL;
    query_string=getenv("QUERY_STRING");
    if (query_string==NULL) return 0;

    size=strlen(query_string);
    s=CGIDecode(ptr,content_type,query_string,size);
    if (s==0) s=CGIDecode(ptr,content_type,NULL,0);
  } else {
    char *content_length=NULL;
    int stp=1024;
    int off=0;
    int num=0;
    char lbuf[1024];
    int fn;
    content_length=getenv("CONTENT_LENGTH");
    if (content_length==NULL) return 0; /* no arguments */
    size=atoi(content_length);
    if (size==0) return 0;

    /* get chunks of input and feed them to the decoder */

    fn=fileno(stdin);

    while(off<size) {
      num=read(fn,lbuf,stp);
      if (num !=0) s=CGIDecode(ptr,content_type,lbuf,num);
      
      off+=num;
      if (s !=0) break;
      if ((num==0) && (off<size)) break; /* end of file but not enough bytes */
    }
    if (off<size) return -1;
    if (s==0) s=CGIDecode(ptr,content_type,NULL,0);
  }
  return s;
}
