/* loadconfig.c
   ============ */

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
#include "rscript.h"
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"

#include "rxmldoc.h"
#include "xmldoc.h"

int loadXMLdoc(FILE *fp,struct XMLdocdata *ptr) {
  int s=0;
  char *epath="xmldoc";
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

  XMLDBSetText(xmldbdata,XMLdocBuild,ptr);
  
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);
  
  while(fgets(lbuf,255,fp) !=NULL) {
   
    s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
    if (s !=0) break;
  
  }

  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);
  
  return 0;
}

int decodetree(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                  void *data) {
  int i;
  struct xmldoc *xmldoc;
  struct XMLDBelement *e;
 
  xmldoc=(struct xmldoc *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
   
    if (strcmp(e->name,"path")==0) {
      if (xmldoc->tree.path !=NULL) free(xmldoc->tree.path);
      xmldoc->tree.path=getDBstring(e->data);
      if (xmldoc->tree.path==NULL) break;
    } else if (strcmp(e->name,"delim")==0) {
      char *d=NULL;
      d=getDBstring(e->data);
      if (d==NULL) break;
      xmldoc->tree.delim=d[0];
      free(d);
    }
  }
  if (i<ptr->num) return -1;
  return 0;
}


int decodematch(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,n;
  struct xmldoc *xmldoc;
  struct XMLDBelement *e;
 
  xmldoc=(struct xmldoc *)data;
  
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"element") !=0) continue;

    n=xmldoc->match.num;

    if (xmldoc->match.txt==NULL) xmldoc->match.txt=malloc(sizeof(char *)); 
    else xmldoc->match.txt=realloc(xmldoc->match.txt,sizeof(char *)*(n+1)); 
  
    if (xmldoc->match.txt==NULL) break;
    xmldoc->match.txt[n]=getDBstring(e->data);
    if (xmldoc->match.txt[n]==NULL) break;
    xmldoc->match.num++;
  }
  if (i<ptr->num) return -1;
  
  return 0;
}

int decodemap(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                    void *data) {
  int i,s=0,n;
  struct xmldoc *xmldoc;
  struct XMLDBelement *e;
 
  char *search=NULL;
  char *replace=NULL;
 
  xmldoc=(struct xmldoc *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"search")==0) {
      if (search !=NULL) free(search);
      search=getDBstring(e->data);
      if (search==NULL) break;
    } else if (strcmp(e->name,"replace")==0) {
      if (replace !=NULL) free(replace);
      replace=getDBstring(e->data);
      if (replace==NULL) break;
    }
  }
  if (i<ptr->num) s=-1;  


  if ((s==0) && (search !=NULL)) {
    n=xmldoc->map.search.num; 
    if (s==0) {
      if (xmldoc->map.search.txt==NULL) 
        xmldoc->map.search.txt=malloc(sizeof(char *));
      else xmldoc->map.search.txt=realloc(xmldoc->map.search.txt,
                                     sizeof(char *)*(n+1));
      if (xmldoc->map.search.txt==NULL) s=-1;
      if (s==0) xmldoc->map.search.txt[n]=NULL;
    }

    if (s==0) {
      if (xmldoc->map.replace.txt==NULL) 
         xmldoc->map.replace.txt=malloc(sizeof(char *));
      else xmldoc->map.replace.txt=realloc(xmldoc->map.replace.txt,
                                     sizeof(char *)*(n+1));
      if (xmldoc->map.replace.txt==NULL) s=-1;
      if (s==0) xmldoc->map.replace.txt[n]=NULL;
    }

    

    if (s==0) {
      xmldoc->map.search.txt[n]=search;
      xmldoc->map.replace.txt[n]=replace;
      xmldoc->map.search.num++;
      xmldoc->map.replace.num++;
    }
  }

  if (s !=0) {
    if (search !=NULL) free(search);
    if (replace !=NULL) free(replace);
    return -1;
  }

  return 0;
}

int decodeexternal(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {

  int i,s=0,n;
  struct xmldoc *xmldoc;
  struct XMLDBelement *e;
  struct XMLDBbuffer *dbuf=NULL;
  char *search=NULL;
  char *replace=NULL;

  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *entptr;

  entptr=EntityMake();
  if (entptr==NULL) return -1;
  EntityAddEntityStringArray(entptr,entity);
  EntityAddSymbolStringArray(entptr,symbol);


  xmldoc=(struct xmldoc *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"search")==0) {
      if (search !=NULL) free(search);
      search=getDBstring(e->data);
      if (search==NULL) break;
    } else if (strcmp(e->name,"replace")==0) {
      if (replace !=NULL) free(replace);
      dbuf=EntityDecodeBuffer(entptr,e->data);
      if (dbuf !=NULL) {
	replace=getDBstring(dbuf);
        XMLDBFreeBuffer(dbuf);
      }
      if (replace==NULL) break;
     }
  }
  if (i<ptr->num) s=-1;  

  if ((s==0) && (search !=NULL)) {
    n=xmldoc->external.search.num; 
    if (s==0) {
      if (xmldoc->external.search.txt==NULL) 
        xmldoc->external.search.txt=malloc(sizeof(char *));
      else xmldoc->external.search.txt=realloc(xmldoc->external.search.txt,
                                     sizeof(char *)*(n+1));
      if (xmldoc->external.search.txt==NULL) s=-1;
      if (s==0) xmldoc->external.search.txt[n]=NULL;
    }


    if (s==0) {
      if (xmldoc->external.replace.txt==NULL) 
         xmldoc->external.replace.txt=malloc(sizeof(char *));
      else xmldoc->external.replace.txt=realloc(xmldoc->external.replace.txt,
                                     sizeof(char *)*(n+1));
      if (xmldoc->external.replace.txt==NULL) s=-1;
      if (s==0) xmldoc->external.replace.txt[n]=NULL;
    }

    if (s==0) {
      xmldoc->external.search.txt[n]=search;
      xmldoc->external.replace.txt[n]=replace;
      xmldoc->external.search.num++;
      xmldoc->external.replace.num++;
      
    }
  }
  free(entptr);
  if (s !=0) {
    if (search !=NULL) free(search);
    if (replace !=NULL) free(replace);
    return -1;
  }
  return 0;
}




int loadexternal(FILE *fp,struct xmldoc *ptr) {
  int s=0;
  char *epath="table/entry";
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

  XMLDBSetText(xmldbdata,decodeexternal,ptr);
  
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);
  
  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
    if (s !=0) break;
  
  }
  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);
  return 0;
}




int xmldocbuild(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {

  int i,j;
  struct xmldoc *xmldoc;
  struct XMLDBelement *e;
  char zero[1]={0};

  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *entptr;

  entptr=EntityMake();
  if (entptr==NULL) return -1;
  EntityAddEntityStringArray(entptr,entity);
  EntityAddSymbolStringArray(entptr,symbol);

  xmldoc=(struct xmldoc *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
  
     if (strcmp(e->name,"tree")==0) {
      XMLdocGetConfigSection(e->data,"tree","tree",decodetree,xmldoc);
     } else if (strcmp(e->name,"match")==0) {
      XMLdocGetConfigSection(e->data,"match","match",decodematch,xmldoc);
    } else if (strcmp(e->name,"script")==0) {
      if (xmldoc->sc.buf !=NULL) 
      XMLDBFreeBuffer(xmldoc->sc.buf);
      xmldoc->sc.type=0;
      for (j=0;j<e->atnum;j++) {
	if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) 
        xmldoc->sc.type=1;
      }
      if (xmldoc->sc.type==1) { 
        xmldoc->sc.buf=XMLDBCopyBuffer(e->data);
        XMLDBAddBuffer(xmldoc->sc.buf,zero,1);
      } else xmldoc->sc.buf=EntityDecodeBuffer(entptr,e->data);
      if (xmldoc->sc.buf==NULL) break;
    } else if (strcmp(e->name,"map")==0) {
      XMLdocGetConfigSection(e->data,"map","map/entry",decodemap,xmldoc);
    } else if (strcmp(e->name,"external")==0) {
      int xtp=0;
      for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) xtp=1;
      }  

      if (xtp==0)
         XMLdocGetConfigSection(e->data,"external","external/entry",
                                decodeexternal,xmldoc);
       else {
         char *fname=NULL;
         FILE *fp;
         fname=getDBstring(e->data);
         
         if (fname==NULL) break;
         fp=fopen(fname,"r");
         if (fp !=NULL) { 
           loadexternal(fp,xmldoc);
           fclose(fp);
         }
       }
    } else if (strcmp(e->name,"xmldoc")==0) {
      int xtp=0;
      if (xmldoc->doc !=NULL) XMLdocFree(xmldoc->doc);
      xmldoc->doc=XMLdocMake();

      for (j=0;j<e->atnum;j++) {
         if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) xtp=1;
      }  

      if (xtp==0)
         XMLdocGetConfigSection(e->data,"xmldoc","xmldoc",XMLdocBuild,
                             xmldoc->doc);
       else {
         char *fname=NULL;
         FILE *fp;
         fname=getDBstring(e->data);
         
         if (fname==NULL) break;
         fp=fopen(fname,"r");
         if (fp !=NULL) { 
           loadXMLdoc(fp,xmldoc->doc);
           fclose(fp);
         }
       }
    }
  
  }
  free(entptr);
  return 0;
}

int loadconfig(FILE *fp,struct xmldoc *ptr) {
  int s=0;
  char *epath="config";
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

  XMLDBSetText(xmldbdata,xmldocbuild,ptr);
  
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);
  
  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
    if (s !=0) break;
  
  }

  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);

  return 0;
}

