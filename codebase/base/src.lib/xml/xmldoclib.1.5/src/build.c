/* build.c
   =======
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

#include "rxmldoc.h"
#include "getstring.h"



int loadtagDB(FILE *fp,struct TagDBtable *tag) {
  int s=0;
  char *epath="tagdb/tag";
  char edelim='/';

  struct XMLdata *xmldata;
  struct XMLDBdata *xmldbdata;
  struct XMLDBtree *tree;
 
  char lbuf[255];

  xmldata=XMLMake();
  xmldbdata=XMLDBMake(xmldata);
  tree=XMLDBMakeTree();

  XMLDBBuildTree(epath,edelim,tree);   
  XMLDBSetTree(xmldbdata,tree);
  XMLDBSetText(xmldbdata,TagDBBuild,tag);
  
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);
  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) return -1;
  }
  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);
  return s;
}

int XMLdocGetConfigSection(struct XMLDBbuffer *ptr,char *root,char *path,
              int (*text)(struct XMLDBtable *,struct XMLDBtable *,void *),
              void *data) {

  int s;
  char delim='/';

  char *start=NULL;
  char *end=NULL;

  struct XMLdata *xmldata;
  struct XMLDBdata *xmldbdata;
  struct XMLDBtree *tree;

  start=malloc(strlen(root)+3);
  end=malloc(strlen(root)+4);

  strcpy(start,"<");
  strcat(start,root);
  strcat(start,">");
  strcpy(end,"</");
  strcat(end,root);
  strcat(end,">");

  xmldata=XMLMake();
  xmldbdata=XMLDBMake(xmldata);
  tree=XMLDBMakeTree();

  XMLDBBuildTree(path,delim,tree);   
  XMLDBSetTree(xmldbdata,tree);
  
  XMLDBSetText(xmldbdata,text,data);
  
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);
  
  s=XMLDecode(xmldata,start,strlen(start));
  s=XMLDecode(xmldata,ptr->buf,ptr->sze);
  s=XMLDecode(xmldata,end,strlen(end));
  if (s !=0) return -1;

  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);
  return 0;
}

int decodemod(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,j,n;
  struct XMLdocmodtable *tab=NULL;
  struct XMLdocmod *mod=NULL;
  struct XMLDBelement *e;
 
  char *name=NULL;
  char *path=NULL;
  int atp=0;
  int xtp=0;
 
  struct TagDBtable  *tagdb=NULL;
  struct XMLDBbuffer *action=NULL;
 
  tab=(struct XMLdocmodtable *)data;

  for (i=0;i<ptr->num;i++) {
     e=ptr->element[i];  
     if (strcmp(e->name,"name")==0) {
     if (name !=NULL) free(name);
       name=getDBstring(e->data);
     } else if (strcmp(e->name,"path")==0) {
       if (path !=NULL) free(path);
       path=getDBstring(e->data);
     } else if (strcmp(e->name,"type")==0) {
       char *type=NULL;
       type=getDBstring(e->data);
       if (strcmp(type,"file")==0) xtp=1;
       if (strcmp(type,"rawfile")==0) xtp=2;
       free(type);
     } else if (strcmp(e->name,"action")==0) {
       action=e->data;
       for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) atp=1;
       }  
     } else if (strcmp(e->name,"tagdb")==0) {
       int ttp=0;

       if (tagdb !=NULL) TagDBFreeTable(tagdb);
       tagdb=TagDBMakeTable();
    
       for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) ttp=1;
       }  

       if (ttp==0) /* database is inline */
         XMLdocGetConfigSection(e->data,"tagdb","tagdb/tag",TagDBBuild,tagdb);
       else {
         char *fname=NULL;
         FILE *fp;
         fname=getDBstring(e->data);
         if (fname==NULL) break;
         fp=fopen(fname,"r");
         if (fp !=NULL) { 
            loadtagDB(fp,tagdb);
            fclose(fp);
          }
       } 
     }
  }

  if (name==NULL) return -1;
  n=tab->num;

  if (tab->ptr==NULL) 
       tab->ptr=malloc(sizeof(struct XMLdocmod *));
  else {
    struct XMLdocmod **tmp;
    tmp=realloc(tab->ptr, sizeof(struct XMLdocmod *)*(n+1));
    if (tmp==NULL) return -1;
    tab->ptr=tmp;
  }
  if (tab->ptr==NULL) return -1;
  
  mod=malloc(sizeof(struct XMLdocmod));
  mod->name=name;
  mod->path=path;
  mod->type.xml=xtp;
  mod->type.action=atp;
  mod->action=XMLDBCopyBuffer(action);
  mod->tagdb=tagdb;
 
  tab->ptr[n]=mod;
  tab->num++;
  return 0;
}

int decodexml(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,j,n;
  struct XMLdoctable *tab=NULL;
  struct XMLdocelement *el=NULL;
  struct XMLDBelement *e;
 
  char *name=NULL;
  char *path=NULL;
  int atp=0;
  int dtp=0;
  int xtp=0;

  struct TagDBtable  *tagdb=NULL;
  struct XMLDBbuffer *action=NULL;
  struct XMLDBbuffer *defval=NULL;
  struct XMLDBbuffer *modifiers=NULL;


  tab=(struct XMLdoctable *)data;

  for (i=0;i<ptr->num;i++) {
     e=ptr->element[i];  
     if (strcmp(e->name,"name")==0) {
     if (name !=NULL) free(name);
       name=getDBstring(e->data);
     } else if (strcmp(e->name,"path")==0) {
       if (path !=NULL) free(path);
       path=getDBstring(e->data);
     } else if (strcmp(e->name,"type")==0) {
       char *type=NULL;
       type=getDBstring(e->data);
       if (strcmp(type,"file")==0) xtp=1;
       if (strcmp(type,"rawfile")==0) xtp=2;
       free(type);
     } else if (strcmp(e->name,"action")==0) {
       action=e->data;
       for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) atp=1;
       }  
     } else if (strcmp(e->name,"default")==0) {
       defval=e->data;
       for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) dtp=1;
       }  
     } else if (strcmp(e->name,"tagdb")==0) {
       int ttp=0;

       if (tagdb !=NULL) TagDBFreeTable(tagdb);
       tagdb=TagDBMakeTable();
    

       for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) ttp=1;
       }  

       if (ttp==0) /* database is inline */
         XMLdocGetConfigSection(e->data,"tagdb","tagdb/tag",TagDBBuild,tagdb);
       else {
         char *fname=NULL;
         FILE *fp;
         fname=getDBstring(e->data);
         if (fname==NULL) break;
         fp=fopen(fname,"r");
         if (fp !=NULL) { 
            loadtagDB(fp,tagdb);
            fclose(fp);
          }
       }
     } else if (strcmp(e->name,"modifiers")==0) modifiers=e->data;
  }

  if (name==NULL) return -1;
  n=tab->num;

  if (tab->ptr==NULL) 
       tab->ptr=malloc(sizeof(struct XMLdocelement *));
  else {
    struct XMLdocelement **tmp;
    tmp=realloc(tab->ptr, sizeof(struct XMLdocelement *)*(n+1));
    if (tmp==NULL) return -1;
    tab->ptr=tmp;
  }
  if (tab->ptr==NULL) return -1;
  
  el=malloc(sizeof(struct XMLdocelement));
  el->name=name;
  el->path=path;
  el->type.xml=xtp;
  el->type.action=atp;
  el->type.defval=dtp;
  el->action=XMLDBCopyBuffer(action);
  el->defval=XMLDBCopyBuffer(defval);  
  el->tagdb=tagdb;

  el->mod.num=0;
  el->mod.ptr=NULL;

  /* decode the modifiers */
  
  if (modifiers !=NULL) XMLdocGetConfigSection(modifiers,"modifiers",
                                   "modifiers/attr",decodemod,&(el->mod));
  

  tab->ptr[n]=el;
  tab->num++;
  return 0;
}

int XMLdocBuild(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {

  int i,j;
  struct XMLdocdata *xmldoc;
  struct XMLDBelement *e;
  xmldoc=(struct XMLdocdata *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"xml")==0) {
      /* XML element list */
      XMLdocGetConfigSection(e->data,"xml","xml/element",decodexml,
                 &(xmldoc->xml));
    } else if (strcmp(e->name,"root")==0) {
      /* Document root */

      int rtp=0;

      for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"env")==0)) rtp=1;
      }  
      if (xmldoc->root !=NULL) XMLDBFreeBuffer(xmldoc->root);
      if (rtp==0) {
        xmldoc->root=XMLDBCopyBuffer(e->data);
      } else if (rtp==1) {
        char *env=NULL;
        env=getenv(getDBstring(e->data));
        if (env==NULL) continue;
	xmldoc->root=XMLDBMakeBuffer(strlen(env));
        XMLDBAddBuffer(xmldoc->root,env,strlen(env));
      } 
      EntityFreeSymbol(xmldoc->ent);
      EntityFreeEntity(xmldoc->ent);
      EntityAddEntityString(xmldoc->ent,"&root;");
      EntityAddSymbol(xmldoc->ent,xmldoc->root);
    } else if (strcmp(e->name,"tagdb")==0) {
      /* global tag database replacement */
      int ttp=0;
      if (xmldoc->tagdb !=NULL) TagDBFreeTable(xmldoc->tagdb);
      xmldoc->tagdb=TagDBMakeTable();
      
      for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) ttp=1;
      }  

      if (ttp==0) /* database is inline */
        XMLdocGetConfigSection(e->data,"tagdb","tagdb/tag",
                               TagDBBuild,xmldoc->tagdb);
      else {
        char *fname=NULL;
        FILE *fp;
        fname=getDBstring(e->data);
        if (fname==NULL) break;
        fp=fopen(fname,"r");
        if (fp !=NULL) { 
           loadtagDB(fp,xmldoc->tagdb);
           fclose(fp);
         }
      }
    }
  }
  return 0;
}





