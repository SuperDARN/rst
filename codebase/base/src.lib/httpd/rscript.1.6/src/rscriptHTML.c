/* rscriptHTML.c
   ============= 
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
#include "rscript.h"
#include "rscriptHTML.h"



struct ScriptHTML *ScriptHTMLMake(struct ScriptData *script) {
  struct ScriptHTML *ptr;
  ptr=malloc(sizeof(struct ScriptHTML));
  if (ptr==NULL) return NULL;
  ptr->ptr=script;
  ptr->search.txt=NULL;
  ptr->replace.txt=NULL;
  ptr->ignore.txt=NULL;
  ptr->remove.txt=NULL;
  ptr->post.txt=NULL;

  ptr->search.num=0;
  ptr->replace.num=0;
  ptr->ignore.num=0;
  ptr->remove.num=0;
  ptr->post.num=0;

  ptr->text.func=NULL;
  ptr->ssi.func=NULL;
  ptr->postp.func=NULL;
  ptr->iflg=-1;
  ptr->pflg=-1;
  return ptr;
}

void ScriptHTMLFree(struct ScriptHTML *ptr) {
  if (ptr==NULL) return;
  free(ptr);
}

int ScriptHTMLSetText(struct ScriptHTML *ptr,
             int (*text)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->text.func=text;
  ptr->text.data=data;
  return 0;
} 

int ScriptHTMLSetSsi(struct ScriptHTML *ptr,
             int (*text)(char *,int,void *),void *data) {

  if (ptr==NULL) return -1;
  ptr->ssi.func=text;
  ptr->ssi.data=data;
  return 0;
} 


int ScriptHTMLSetPostp(struct ScriptHTML *ptr,
             int (*text)(char *,int,char *,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->postp.func=text;
  ptr->postp.data=data;
  return 0;
} 


int ScriptHTMLSetSearch(struct ScriptHTML *ptr,int num,char **search) {
  if (ptr==NULL) return -1;
  ptr->search.txt=search;
  ptr->search.num=num;
  return 0;
}

int ScriptHTMLSetReplace(struct ScriptHTML *ptr,int num,char **replace) {
  if (ptr==NULL) return -1;
  ptr->replace.txt=replace;
  ptr->replace.num=num;
  return 0;
}


int ScriptHTMLSetIgnore(struct ScriptHTML *ptr,int num,char **ignore) {
  if (ptr==NULL) return -1;
  ptr->ignore.txt=ignore;
  ptr->ignore.num=num;
  return 0;
}

int ScriptHTMLSetRemove(struct ScriptHTML *ptr,int num,char **remove) {
  if (ptr==NULL) return -1;
  ptr->remove.txt=remove;
  ptr->remove.num=num;
  return 0;
}

int ScriptHTMLSetPost(struct ScriptHTML *ptr,int num,char **post) {
  if (ptr==NULL) return -1;
  ptr->post.txt=post;
  ptr->post.num=num;
  return 0;
}


int ScriptHTMLSetSearchCount(struct ScriptHTML *ptr,char **search) {
  int num=0;
  if (ptr==NULL) return -1;
  ptr->search.txt=search;
  for (num=0;search[num] !=NULL;num++);
  ptr->search.num=num;
  return 0;
}

int ScriptHTMLSetReplaceCount(struct ScriptHTML *ptr,char **replace) {
  int num=0;
  if (ptr==NULL) return -1;
  ptr->replace.txt=replace;
  for (num=0;replace[num] !=NULL;num++);
  ptr->replace.num=num;
  return 0;
}


int ScriptHTMLSetIgnoreCount(struct ScriptHTML *ptr,char **ignore) {
  int num=0;
  if (ptr==NULL) return -1;
  ptr->ignore.txt=ignore;
  for (num=0;ignore[num] !=NULL;num++);
  ptr->ignore.num=num;
  return 0;
}

int ScriptHTMLSetRemoveCount(struct ScriptHTML *ptr,char **remove) {
  int num=0;
  if (ptr==NULL) return -1;
  ptr->remove.txt=remove;
   for (num=0;remove[num] !=NULL;num++);
  ptr->remove.num=num;
  return 0;
}

int ScriptHTMLSetPostCount(struct ScriptHTML *ptr,char **post) {
  int num=0;
  if (ptr==NULL) return -1;
  ptr->post.txt=post;
  for (num=0;post[num] !=NULL;num++);
  ptr->post.num=num;
  return 0;
}


int cmp_tag(char *token,char *str) {
  int t=0;
  if (token==NULL) return 0;
  if (str==NULL) return 0;
  if (strcmp(token+t,str) !=0) return 0;
  return 1;
}

int cmp_tags(char *token,int num,char *str[]) {
  int i;
  if (token==NULL) return -1;
  if (str==NULL) return -1;
  for (i=0;i<num;i++) { 
    if (str[i]==NULL) continue;
    if (cmp_tag(token,str[i]) !=0) return i;
  }
  return -1;
}

int HTMLWriter(char *buf,int sze,void *data) {
  int status=0;
  struct ScriptHTML *ptr;

  ptr=(struct ScriptHTML *)data;
 
  if (buf[0]=='<') {
 
    if ((sze>7) && (buf[1]=='!') && (buf[2]=='-') && (buf[3]=='-') &&
        (buf[4]=='#') && (buf[sze-2]=='-') && (buf[sze-3]=='-')) {
  
      /* pass an SSI token to the SSI handler  */

      if ((ptr->iflg==-1) && (ptr->ssi.func !=NULL))
	status=(ptr->ssi.func)(buf,sze,ptr->ssi.data);
      if (status !=0) return status;
    } else if ((sze>6) && (buf[1]=='!') && (buf[2]=='-') && (buf[3]=='-') && 
	       (buf[sze-2]=='-') && (buf[sze-3]=='-')) {

      /* a comment tag */
    
      int i;

      char *tag;

      tag=malloc(sze-6);
      if (tag==NULL) return -1;
      memcpy(tag,buf+4,sze-7);
      tag[sze-7]=0;
    
      /* ignored sections */
      
      if (ptr->ignore.txt !=NULL) {
        if (ptr->iflg==-1) {
  	  i=cmp_tags(tag,ptr->ignore.num,ptr->ignore.txt);
          if (i !=-1) ptr->iflg=i;
	} else if (cmp_tag(ptr->ignore.txt[ptr->iflg],tag)==1) {
	  ptr->iflg=-1;
          free(tag);
          return 0;
	}
      }

      if (ptr->iflg !=-1) {
	free(tag);
        return 0;
      }
      
      /* post processed sections */
      
      if (ptr->post.txt !=NULL) {
        if (ptr->pflg==-1) {
  	  i=cmp_tags(tag,ptr->post.num,ptr->post.txt);
          if (i !=-1) ptr->pflg=i;
	} else if (cmp_tag(ptr->post.txt[ptr->pflg],tag)==1) {
          if (ptr->postp.func !=NULL)
             status=(ptr->postp.func)
                    (NULL,0,ptr->post.txt[ptr->pflg],ptr->postp.data);
	  ptr->pflg=-1;
          free(tag);
          return status;
	}
      }
      
      if (ptr->pflg !=-1) {
	free(tag);
        return 0;
      }
      

      /* search and replace */
      
      if ((ptr->search.txt !=NULL)) {
        i=cmp_tags(tag,ptr->search.num,ptr->search.txt);
        if (i !=-1) {
          if ((ptr->replace.txt !=NULL) && (ptr->replace.txt[i] !=NULL) &&
	      (ptr->text.func !=NULL)) 
             status=(ptr->text.func)(ptr->replace.txt[i],
                                      strlen(ptr->replace.txt[i]),
                                      ptr->text.data); 
	  free(tag);
          return status;
        } 
      }

      if (ptr->remove.txt !=NULL) {
        i=cmp_tags(tag,ptr->remove.num,ptr->remove.txt);
        if (i !=-1) {
          free(tag);
          return 0;
	}
      }      
      
      if (ptr->text.func !=NULL) 
          status=(ptr->text.func)("<",1,ptr->text.data); 
      if (status==0) status=ScriptDecode(ptr->ptr,buf+1,sze-2);
      if ((status==0) && (ptr->text.func !=NULL)) 
         status=(ptr->text.func)(">",1,ptr->text.data); 
      free(tag);
      return status;
    } else {
      /* regular HTML tag */
      if (ptr->pflg !=-1) {
	if (ptr->postp.func !=NULL)
           status=(ptr->postp.func)("<",1,ptr->post.txt[ptr->pflg],
                                    ptr->postp.data);
        if (status==0) status=ScriptDecode(ptr->ptr,buf+1,sze-2);
        if ((status==0) && (ptr->postp.func !=NULL))
	  status=(ptr->postp.func)(">",1,ptr->post.txt[ptr->pflg],
                                   ptr->postp.data);
      } else if (ptr->iflg==-1) {
        if (ptr->text.func !=NULL) 
          status=(ptr->text.func)("<",1,ptr->text.data); 
        if (status==0) status=ScriptDecode(ptr->ptr,buf+1,sze-2);
        if ((status==0) && (ptr->text.func !=NULL)) 
           status=(ptr->text.func)(">",1,ptr->text.data); 
     
      }
    }
    return status;
  }
  if (ptr->pflg !=-1) {
    if (ptr->postp.func !=NULL) 
    status=(ptr->postp.func)(buf,sze,
                              ptr->post.txt[ptr->pflg],ptr->postp.data);
  } else if ((ptr->iflg==-1) && (ptr->text.func !=NULL)) 
    status=(ptr->text.func)(buf,sze,ptr->text.data); 

  return status;
}


