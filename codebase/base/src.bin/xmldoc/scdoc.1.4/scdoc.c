/* scdoc.c
   ======= 
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
#include "option.h"
#include "rxml.h"
#include "rxmldb.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"


#include "hlpstr.h"
#include "errstr.h"

int arg;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: scdoc --help\n");
  return(-1);
}

struct {
  int num;
  char **txt;
} search;
    
struct {
  int num;
  char **txt;
} replace;

int streamoutput(char *buf,int sze,void *data) {
  /* Output to a stream */
  FILE *outp;
  outp=(FILE *) data;
  fwrite(buf,sze,1,outp);
  return 0;
}

int decodetable(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {

  int i,s=0,n;
  struct XMLDBelement *e;
  struct XMLDBbuffer *dbuf=NULL;
  char *stxt=NULL;
  char *rtxt=NULL;

  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *entptr;

  entptr=EntityMake();
  if (entptr==NULL) return -1;
  EntityAddEntityStringArray(entptr,entity);
  EntityAddSymbolStringArray(entptr,symbol);

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"search")==0) {
      if (stxt !=NULL) free(stxt);
      stxt=getDBstring(e->data);
      if (stxt==NULL) break;
    } else if (strcmp(e->name,"replace")==0) {
      if (rtxt !=NULL) free(rtxt);
      dbuf=EntityDecodeBuffer(entptr,e->data);
      if (dbuf !=NULL) {
	rtxt=getDBstring(dbuf);
        XMLDBFreeBuffer(dbuf);
      }
      if (rtxt==NULL) break;
     }
  }
  if (i<ptr->num) s=-1;  

  if ((s==0) && (stxt !=NULL)) {
    n=search.num; 
    if (s==0) {
      if (search.txt==NULL) 
        search.txt=malloc(sizeof(char *));
      else search.txt=realloc(search.txt,
                                     sizeof(char *)*(n+1));
      if (search.txt==NULL) s=-1;
      if (s==0) search.txt[n]=NULL;
    }


    if (s==0) {
      if (replace.txt==NULL) 
         replace.txt=malloc(sizeof(char *));
      else replace.txt=realloc(replace.txt,
                                     sizeof(char *)*(n+1));
      if (replace.txt==NULL) s=-1;
      if (s==0) replace.txt[n]=NULL;
    }

    if (s==0) {
      search.txt[n]=stxt;
      replace.txt[n]=rtxt;
      search.num++;
      replace.num++;
    }
  }
  free(entptr);
  if (s !=0) {
    if (stxt !=NULL) free(stxt);
    if (rtxt !=NULL) free(rtxt);
    return -1;
  }
  return 0;
}

int ssiproc(char *token,int sze,void *data) {
  fwrite(token,sze,1,stdout);
  return 0;
}

int loadtable(FILE *fp) {
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

  XMLDBSetText(xmldbdata,decodetable,NULL);
  
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


int main(int argc,char *argv[]) {

  int s=0;
  FILE *fp;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  struct OptionText *ignore=NULL;
  struct OptionText *remove=NULL;

  struct ScriptData *script;
  struct ScriptHTML *shtml;
  
  char lbuf[255];
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"i",'a',&ignore);
  OptionAdd(&opt,"r",'a',&remove);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  } 

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }


  if (argc-arg<2) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  fp=fopen(argv[arg],"r");
  loadtable(fp);
  fclose(fp);
 
  script=ScriptMake();
  shtml=ScriptHTMLMake(script);
  ScriptSetText(script,HTMLWriter,shtml);
  ScriptHTMLSetText(shtml,streamoutput,stdout);
   
  ScriptHTMLSetSearch(shtml,search.num,search.txt);
  ScriptHTMLSetReplace(shtml,replace.num,replace.txt); 
  if (ignore !=NULL)
    ScriptHTMLSetIgnore(shtml,ignore->num,ignore->txt);

  if (remove !=NULL)
    ScriptHTMLSetRemove(shtml,remove->num,remove->txt);


  ScriptHTMLSetSsi(shtml,ssiproc,NULL);

  

  fp=fopen(argv[arg+1],"r");
  if (fp !=NULL) {
    while(fgets(lbuf,255,fp) !=NULL) { 
      s=ScriptDecode(script,lbuf,strlen(lbuf));
      if (s !=0) break;
    }
    fclose(fp);
  }
  ScriptFree(script);
  fflush(stdout);  
  return 0;
}
