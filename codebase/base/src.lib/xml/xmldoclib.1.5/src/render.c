/* render.c
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
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"

#include "getstring.h"
#include "rxmldoc.h"



int XMLdocRenderFile(struct XMLdocfile *ptr) {
  FILE *fp;
  char lbuf[256];
  struct Entitydata *ent;
 
  char *filename=NULL;
  int s=0;
  if (ptr==NULL) return 0;
  if (ptr->fname==NULL) return 0;
  if (ptr->text==NULL) return 0;
  if (ptr->path !=NULL) {
    filename=malloc(strlen(ptr->path)+strlen(ptr->fname)+2);
    sprintf(filename,"%s/%s",ptr->path,ptr->fname);
  } else {
    filename=malloc(strlen(ptr->fname)+1);
    strcpy(filename,ptr->fname);
  }
    
  fp=fopen(filename,"r");
  free(filename);
  if (fp==NULL) return 0;
 
  ent=EntityMake();
  if (ent==NULL) {  
    fclose(fp);
    return -1;
  }

  s=EntitySetText(ent,ptr->text,ptr->data);

  if ((s==0) && (ptr->type !=0)) { /* raw file so must encode entities */
    char *symbol[]={"\"","\\","<",">","&",0};
    char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
    s=EntityAddEntityStringArray(ent,entity);
    if (s==0) s=EntityAddSymbolStringArray(ent,symbol);
  }

  while ((s==0) && (fgets(lbuf,255,fp) !=NULL)) {
    s=EntityEncode(lbuf,strlen(lbuf),ent);
    if (s !=0) break;
  }
  fclose(fp);
  return s;
}

int XMLdocFileTrap(char *buf,int sze,void *data) {
  struct XMLdocfile *ptr;
  
  if (strncmp(buf,"&file;",sze) !=0) return 0; /* ignore this one */
  ptr=(struct XMLdocfile *) data; 
  if (ptr !=NULL) return XMLdocRenderFile(ptr);
  return 0;
}

int XMLdocRenderElement(struct TagDBdata *tag,struct TagDBtable *tagdb,
                 struct XMLdocelement *x,struct XMLDBelement *e) {
  int i,j,s=0;
  struct XMLdocmod *m=NULL;
  struct XMLDBbuffer *action=NULL;
  struct XMLDBbuffer *data=NULL;
  char *path=NULL;
  int xtp=0;
  int atp=0;

  path=x->path;
  action=x->action;
  xtp=x->type.xml;
  atp=x->type.action;
 
  TagDBSetTable(tag,tagdb); 
  if (x->tagdb !=NULL) TagDBSetTable(tag,x->tagdb);

  if (e !=NULL) data=e->data;
  else {
    data=x->defval;
    xtp=x->type.defval;
  }

  if (e !=NULL) {
    /* decide if we need a modifier */
    for (i=0;i<e->atnum;i++) {
      for (j=0;j<x->mod.num;j++) {
        m=x->mod.ptr[j];
        if (strcmp(e->atval[i],m->name)==0) break;
      }
      if (j<x->mod.num) {
	/* we have a matching attribute */
        if (m->action !=NULL) action=m->action;
        if (m->tagdb !=NULL) TagDBSetTable(tag,m->tagdb);
        if (m->path !=NULL) path=m->path;
        atp=m->type.action;
        xtp=m->type.xml;
      }
    }
  }

  /* we are now in a position to render this element */
   
  if (action==NULL) { /* no action - render as text */
    if (xtp==0) { /* data is text */
      s=TagDBDecode(data->buf,data->sze,tag);
      s=TagDBDecode(NULL,0,tag);
    } else {
      struct XMLdocfile file;
      file.fname=NULL;
      if (data !=NULL) file.fname=getDBstring(data);
      file.path=path;
      file.text=TagDBDecode;
      file.data=tag;
      file.type=xtp-1;
      s= XMLdocRenderFile(&file);
      if (file.fname !=NULL) free(file.fname);
    }   
  } else {

    /* use an entity decoder to handle the action */

    struct Entitydata *ent;
    ent=EntityMake();
    if (ent==NULL) return -1;
   
    s=EntityAddEntityString(ent,"&lt;");
    s=EntityAddEntityString(ent,"&gt;");
    s=EntityAddSymbolString(ent,"<");
    s=EntityAddSymbolString(ent,">");
   
    if (xtp==0) { 
       s=EntityAddEntityString(ent,"&xml;");
       if (s==0) s=EntityAddSymbol(ent,data);
    } else {
      struct XMLdocfile file;
      file.fname=NULL;
      if (data !=NULL) file.fname=getDBstring(data);
      file.path=path;
      file.text=TagDBDecode;
      file.data=tag;
      file.type=xtp-1;

      s=EntityAddEntityString(ent,"&filename;");
      if (s==0) s=EntityAddEntityString(ent,"&file;");
      if (s==0) s=EntityAddSymbol(ent,e->data); 
      if (s==0) s=EntitySetDecodeTrap(ent,XMLdocFileTrap,&file);
      
    }     

    EntitySetText(ent,TagDBDecode,tag);
    if (atp==0) { 
      s=EntityDecode(action->buf,action->sze,ent);
      if (s==0) s=EntityDecode(NULL,0,ent);
    } else {
      FILE *fp;
      char *afname=NULL;
      char lbuf[256];
      afname=getDBstring(action); 
      if (afname !=NULL) {	
        fp=fopen(afname,"r");
        if (fp !=NULL) {
          while ((s==0) && (fgets(lbuf,255,fp) !=NULL)) {
             s=EntityDecode(lbuf,strlen(lbuf),ent);
             if (s !=0) break;
          }
          if (s==0) s=EntityDecode(NULL,0,ent);
          fclose(fp);
        }
        free(afname);
      }  
    }
    EntityFree(ent);
  }
  return 0;
}


int XMLdocRender(struct XMLDBtable *ptr,struct XMLdocdata *xmldoc,int index) {
  struct XMLDBelement *e;
  struct XMLdocelement *x;
  int j;
  int dflg=1;
 
  x=xmldoc->xml.ptr[index];
  dflg=1;
  if (ptr !=NULL) for (j=0;j<ptr->num;j++) {
    e=ptr->element[j];
    if (strcmp(e->name,x->name)==0) { 
      /* decode and output the element */
      XMLdocRenderElement(xmldoc->tag,xmldoc->tagdb,x,e);
      dflg=0;
    }
  }
  if (dflg==1) XMLdocRenderElement(xmldoc->tag,xmldoc->tagdb,x,NULL);
  return 0;
}

int XMLdocIncludeText(struct XMLdocdata *xmldoc,char *buf,int sze) {
  struct Entitydata *ent;
  ent=xmldoc->ent;
  return (ent->text.func)(buf,sze,ent->text.data);
}



