/* rxmldb.c
   ======== 
   Author R.J.Barnes
*/

/*
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



struct XMLDBdata *XMLDBMake(struct XMLdata *xml) {

  struct XMLDBdata *ptr;
  ptr=malloc(sizeof(struct XMLDBdata));
  if (ptr==NULL) return NULL;
 
  ptr->ptr=xml;
  ptr->doc=XMLDBMakeTable();

  if (ptr->doc==NULL) {
    free(ptr);
    return NULL;
  }

  ptr->table=NULL;
  ptr->buf=NULL;
  ptr->tree=NULL;
  ptr->depth=0;
  ptr->text.func=NULL;
  ptr->text.data=NULL;
  return ptr;
}

void XMLDBFree(struct XMLDBdata *ptr) {
  if (ptr==NULL) return;
  if (ptr->table !=NULL) XMLDBFreeTable(ptr->table);
  if (ptr->doc !=NULL) XMLDBFreeTable(ptr->doc);
  free(ptr);
}

int XMLDBSetTree(struct XMLDBdata *ptr,struct XMLDBtree *tree) {
  if (ptr==NULL) return -1;
  ptr->tree=tree;
  return 0;
}

int XMLDBSetText(struct XMLDBdata *ptr,
             int (*text)(struct XMLDBtable *,struct XMLDBtable *,
                         void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->text.func=text;
  ptr->text.data=data;
  return 0;
} 


int XMLDBElementStart(char *name,char end,int atnum,char **atname,char **atval,
                char *buf,int sze,void *data) {
  int i;
  int status=0;
  struct XMLDBdata *x;
  struct XMLDBbuffer *b;
  x=(struct XMLDBdata *) data;
  x->depth++;
 
  b=x->buf;

  status=XMLDBAddBuffer(b,"<",1);
  if (status !=0) return status;
  status=XMLDBAddBuffer(b,name,strlen(name));
  if (status !=0) return status;

 
  for (i=0;i<atnum;i++) {
    status=XMLDBAddBuffer(b," ",1);
    if (status !=0) return status;
    status=XMLDBAddBuffer(b,atname[i],strlen(atname[i]));
    if (status !=0) return status;

    if (atval[i]!=NULL) {
      status=XMLDBAddBuffer(b,"=\"",2);
      if (status !=0) return status;
      status=XMLDBAddBuffer(b,atval[i],strlen(atval[i]));
      if (status !=0) return status;
      status=XMLDBAddBuffer(b,"\"",1);
      if (status !=0) return status;
    }
  }
  if (end !=0) status=XMLDBAddBuffer(b,&end,1);
  if (status !=0) return status;
  status=XMLDBAddBuffer(b,">",1);

  if (end !=0) x->depth--;

  return status;
}



int XMLDBElementEnd(char *name,char *buf,int sze,void *data) {
  int status=0;
  struct XMLDBdata *x;
  struct XMLDBbuffer *b;
  x=(struct XMLDBdata *) data;
  x->depth--;
  b=x->buf;

  if (x->depth==0) { /* reset the userr */
    x->depth++; 
    XMLSetStart(x->ptr,XMLDBPeerStart,x);
    XMLSetEnd(x->ptr,XMLDBPeerEnd,x);
    XMLSetText(x->ptr,NULL,NULL);
    status=XMLDBPeerEnd(name,buf,sze,data);
    return status;

  }
  
  status=XMLDBAddBuffer(b,"</",2);
  if (status !=0) return status;
  status=XMLDBAddBuffer(b,name,strlen(name));
  if (status !=0) return status;
  status=XMLDBAddBuffer(b,">",1);
  return status;
 
}

int XMLDBElementText(char *buf,int sze,void *data) {
  int status=0;
  struct XMLDBdata *x;
  struct XMLDBbuffer *b;
  x=(struct XMLDBdata *) data;
  b=x->buf;
  status=XMLDBAddBuffer(b,buf,sze);
  return status;
}


int XMLDBPeerStart(char *name,char end,int atnum,char **atname,char **atval,
                char *buf,int sze,void *data) {

  
  struct XMLDBdata *x;
  struct XMLDBelement *e;


  x=(struct XMLDBdata *) data;
  x->depth++;

  e=XMLDBMakeElement(name,end,atnum,atname,atval,100);
  if (e==NULL) return -1;
  if (x->table==NULL) x->table=XMLDBMakeTable();
 
  if (x->table==NULL) {
     XMLDBFreeElement(e);
     return -1;
  }
  
  if (XMLDBAddTable(x->table,e) !=0) {
    XMLDBFreeElement(e);
    return -1;
  }
 
  if (end==0) {
    x->buf=e->data;
    XMLSetStart(x->ptr,XMLDBElementStart,x);
    XMLSetEnd(x->ptr,XMLDBElementEnd,x);
    XMLSetText(x->ptr,XMLDBElementText,x);
  } else x->depth--;


  return 0;
}

int XMLDBPeerEnd(char *name,char *buf,int sze,void *data) {

  int status=0;
  struct XMLDBdata *x;
  x=(struct XMLDBdata *) data;
  x->depth--; 
 
  if (x->depth<0) { 

    /* we have read in all the peers at this level */

    
    if ((x->text.func !=NULL)) 
      status=(x->text.func)(x->table,x->doc,x->text.data);
      
    XMLDBFreeTable(x->table);
    x->table=NULL;

    if (status !=0) return status;  
    XMLSetStart(x->ptr,XMLDBStart,x);
    XMLSetEnd(x->ptr,XMLDBEnd,x);
    status=XMLDBEnd(name,buf,sze,data);
  }
  return status;
}





int XMLDBStart(char *name,char end,int atnum,char **atname,char **atval,
                char *buf,int sze,void *data) {
  int i=0;

  struct XMLDBdata *x;
  struct XMLDBtree *t;
  struct XMLDBtable *d;
  struct XMLDBelement *e;
 
  x=(struct XMLDBdata *) data;
  t=x->tree;
  d=x->doc;

  e=XMLDBMakeElement(name,end,atnum,atname,atval,10);
  if (e==NULL) return -1;
  
  if (XMLDBAddTable(d,e) !=0) {
    XMLDBFreeElement(e);
    return -1;
  }


  if (d->num==t->depth) {
    for (i=0;i<t->depth;i++) 
      if (strcmp(t->name[i],
                (d->element[i])->name) !=0) break;
  }

  if (i==t->depth) {
     /* we are now in the right depth */
      x->depth=0;
      XMLSetStart(x->ptr,XMLDBPeerStart,data);
      XMLSetEnd(x->ptr,XMLDBPeerEnd,data);
  }
  return 0;
 
}

int XMLDBEnd(char *name,char *buf,int sze,void *data) {

  struct XMLDBdata *x;
  struct XMLDBtable *t;
  
  x=(struct XMLDBdata *) data;
  t=x->doc;

  if (t->num==0) return -1;

  XMLDBRemoveTable(t);
  
  return 0; 

}

















