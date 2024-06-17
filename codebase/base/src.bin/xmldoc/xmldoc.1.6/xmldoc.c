/* xmldoc.c
   ======== */

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
#include <unistd.h>
#include "option.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"

#include "rxmldoc.h"
#include "xmldoc.h"
#include "loadconfig.h"

#include "hlpstr.h"

int arg;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: xmldoc --help\n");
  return(-1);
}


struct xmldoc xmldoc;

int redirect(char *buf,int sze,void *data) {
  char lbuf[256];
  FILE *xmlfp=NULL;

  int s=0;
  
  struct XMLdata *xmldata;
  
  
  int c,d;
  
  xmldata=(struct XMLdata *) data;
  
  for (c=0;(c<sze) && (buf[c] !='#');c++);
  if (c==sze) return 0;
  for (d=0;(d<sze) && (buf[d] !=' ');d++);
  if (c==sze) return 0;
  buf[d]=0;
  if (strcmp(buf+c,"#include")!=0) return 0;
  d++;
  for (c=d;(c<sze) && (buf[c]==' ');c++);
  if (c==sze) return 0;
  for (d=c;(d<sze) && (buf[d]!='=');d++);
  if (d==sze) return 0; 
  buf[d]=0;  
  if (strcmp(buf+c,"file")!=0) return 0;
  d++;
  for (c=d;(c<sze) && (buf[c]!='"');c++);
  c++;
  for (d=c;(d<sze) && (buf[d]!='"');d++);
  buf[d]=0;

  xmlfp=fopen(buf+c,"r");
  if (xmlfp==NULL) return 0;

  while(fgets(lbuf,255,xmlfp) !=NULL) {
    
   s=XMLDecode(xmldata,lbuf,strlen(lbuf));
   if (s !=0) break;
  
  }
  fclose(xmlfp);
  
  return 0;


}


int stream_output(char *buf,int sze,void *data) {
  /* Output to a stream */
  FILE *outp;
  outp=(FILE *) data;
  fwrite(buf,sze,1,outp);
  return 0;
}

int cmp(char *name,struct XMLDBbuffer *ptr,void *data) {
  
  /* Search for a string in an XMLDB buffer structure */

  char zero[1];
  char *dst;
  int s=0;
  struct XMLDBbuffer *src=NULL;

  zero[0]=0;
  dst=(char *)data;
 
  src=XMLDBCopyBuffer(ptr);
  if (src==NULL) return -1;
  XMLDBAddBuffer(src,zero,1);
  if (strcmp(src->buf,dst)==0) s=1;
  XMLDBFreeBuffer(src); 
  return s;
  
}

int render(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {
  int i,j;
  int c=0,s=0;
  struct xmldoc *xmldoc; 
  struct XMLDBelement *e;
  char lbuf[255];
  FILE *fp;


  xmldoc=(struct xmldoc *) data; 
  
  if ((xmldoc->compare.data !=NULL) && (xmldoc->match.txt !=NULL)) {
    
     for (i=0;i<ptr->num;i++) {
       e=ptr->element[i];
       for (j=0;j<xmldoc->match.num;j++) {
         if ((xmldoc->match.txt[j] !=NULL) && 
             (strcmp(xmldoc->match.txt[j],e->name)==0) && 
             (xmldoc->compare.check !=NULL)) {
	   c=(xmldoc->compare.check)(xmldoc->match.txt[j],e->data,
                                   xmldoc->compare.data);
           /* Match found */

           if (c !=0) break;
         }
       }
       if (c !=0) break;
     }
     /* No match */

     if (c==0) return 0;
   }
   
  xmldoc->table=ptr;
  
  if (xmldoc->sc.buf==NULL) return 0; /* nothing to do */

  if (xmldoc->sc.type==1) {
    fp=fopen((xmldoc->sc.buf)->buf,"r");
    if (fp==NULL) return 0;

     while(fgets(lbuf,255,fp) !=NULL) { 
       s=ScriptDecode(xmldoc->script,lbuf,strlen(lbuf));
       if (s !=0) break;
     }
     fclose(fp);
  } else s=ScriptDecode(xmldoc->script,(xmldoc->sc.buf)->buf,
		       (xmldoc->sc.buf)->sze);

  return 0;
}


int mapxml(char *buf,int sze,void *data) {
  int i,j,status;
  struct XMLdocdata *doc;
  struct xmldoc *xmldoc;
  struct ScriptData *script;
  xmldoc=(struct xmldoc *) data;

  doc=xmldoc->doc;
  script=xmldoc->script;
  if ((sze>7) && 
      (buf[0]=='<') && (buf[1]=='!') && (buf[2]=='-') && 
      (buf[3]=='-') && (buf[sze-2]=='-') && (buf[sze-3]=='-')) {
  
      char *tag;
      struct XMLdocelement *e;
     
      tag=malloc(sze-6);
      if (tag==NULL) return -1;
      memcpy(tag,buf+4,sze-7);
      tag[sze-7]=0;   

      
      if (xmldoc->map.ignore.txt !=NULL) {
        for (i=0;i<xmldoc->map.ignore.num;i++) {
          if ((xmldoc->map.ignore.txt[i] !=NULL) &&
             (strcmp(xmldoc->map.ignore.txt[i],tag)==0)) break;
	}
        if (i<xmldoc->map.ignore.num) {
          if (xmldoc->map.iflg==-1) xmldoc->map.iflg=i;
          else {
            xmldoc->map.iflg=-1;
            free(tag);
            return 0;
	  }
        }
      }

      if (xmldoc->map.iflg !=-1) {
	free(tag);
        return 0;
      }
      
      if (xmldoc->map.search.txt !=NULL) {
        for (i=0;i<xmldoc->map.search.num;i++) {
          if ((xmldoc->map.search.txt[i] !=NULL) &&
             (strcmp(xmldoc->map.search.txt[i],tag)==0)) break;
	}
         

        if (i<xmldoc->map.search.num) {
          if ((xmldoc->map.replace.txt==NULL) || 
              (xmldoc->map.replace.txt[i]==NULL))
	    return 0;  
          for (j=0;j<doc->xml.num;j++) {
            e=doc->xml.ptr[j];
            if (strcmp(e->name,xmldoc->map.replace.txt[i])==0) 
	      break;
	  } 
          if (j<doc->xml.num) XMLdocRender(xmldoc->table,doc,j);
          free(tag);
          return 0;
        }
      }

      if (xmldoc->external.search.txt !=NULL) {
        for (i=0;i<xmldoc->external.search.num;i++) {
          if ((xmldoc->external.search.txt[i] !=NULL) &&
             (strcmp(xmldoc->external.search.txt[i],tag)==0)) break;
	}
         

        if (i<xmldoc->external.search.num) {
          if ((xmldoc->external.replace.txt==NULL) || 
              (xmldoc->external.replace.txt[i]==NULL)) {
            free(tag);
	    return 0; 
	  } 
	  XMLdocIncludeText(xmldoc->doc,xmldoc->external.replace.txt[i],
                            strlen(xmldoc->external.replace.txt[i]));
          free(tag);
          return 0;

        }
      }

      if (xmldoc->map.remove.txt !=NULL) {
        for (i=0;i<xmldoc->map.remove.num;i++) {
          if ((xmldoc->map.remove.txt[i] !=NULL) &&
             (strcmp(xmldoc->map.remove.txt[i],tag)==0)) break;
	}
        if (i<xmldoc->map.remove.num) {
	  free(tag);
          return 0;
	}
      }
      free(tag);
  } else if ((buf[0]=='<') && (buf[sze-1]=='>') && (xmldoc->map.iflg==-1)) {
    status=XMLdocIncludeText(xmldoc->doc,"<",1);
    if (status==0) status=ScriptDecode(script,buf+1,sze-2);
    if (status==0) status=XMLdocIncludeText(xmldoc->doc,">",1);
    return status;
  }
  if (xmldoc->map.iflg !=-1) return 0;
  return XMLdocIncludeText(xmldoc->doc,buf,sze);
}


int main(int argc,char *argv[]) {
  int s=0,m,n;
  FILE *fp;
  FILE *xmlfp;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  unsigned char ilf=0; 
  char *pathstr=NULL;
  char *scstr=NULL;
  int sctype=0;

  struct XMLDBbuffer *scbuf=NULL;  

  struct XMLdata *xmldata=NULL;
  struct XMLDBdata *xmldbdata=NULL;
  struct XMLDBtree *tree=NULL;

  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *entptr;

  struct OptionText *ignore=NULL;
  struct OptionText *remove=NULL;


  char lbuf[255];
  char zero[1]={0};

  entptr=EntityMake();
 
  EntityAddEntityStringArray(entptr,entity);
  EntityAddSymbolStringArray(entptr,symbol);
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"ilf",'x',&ilf);

  OptionAdd(&opt,"i",'a',&ignore);
  OptionAdd(&opt,"r",'a',&remove);

  OptionAdd(&opt,"path",'t',&pathstr);
  OptionAdd(&opt,"script",'t',&scstr);
  OptionAdd(&opt,"sctype",'i',&sctype);

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
 

  if ((argc-arg)<3) {
    fprintf(stderr,"xmldoc pwd cfgfile xmldata\n");
    exit(-1);
  }
 
  xmlfp=fopen(argv[arg+2],"r");
  if (xmlfp==NULL) {
    fprintf(stderr,"file not found.\n");
    exit(-1);
  }

  s=chdir(argv[arg]);
  if (s !=0) {
    /*exit(-1);*/
  }

  fp=fopen(argv[arg+1],"r");
  loadconfig(fp,&xmldoc);
  fclose(fp);

 
  if (argc>3) xmldoc.compare.data=argv[arg+3];
  else xmldoc.compare.data=NULL;

  xmldoc.compare.check=cmp;  

  xmldoc.map.iflg=-1;

  if (pathstr !=NULL) {
    free(xmldoc.tree.path);
    xmldoc.tree.path=pathstr;   
  }

  if (scstr !=NULL) {
    xmldoc.sc.type=sctype;
    scbuf=XMLDBMakeBuffer(64);
    XMLDBAddBuffer(scbuf,scstr,strlen(scstr));
    XMLDBFreeBuffer(xmldoc.sc.buf);

    if (sctype==1) {
      xmldoc.sc.buf=XMLDBCopyBuffer(scbuf);
      XMLDBAddBuffer(xmldoc.sc.buf,zero,1);
    } else xmldoc.sc.buf=EntityDecodeBuffer(entptr,scbuf);
    XMLDBFreeBuffer(scbuf);
  }
  
  xmldata=XMLMake();
  xmldbdata=XMLDBMake(xmldata);
  tree=XMLDBMakeTree();

  XMLDBBuildTree(xmldoc.tree.path,xmldoc.tree.delim,tree);
  XMLDBSetTree(xmldbdata,tree);
    
  /* Set the renderer to XMLrender */

  XMLDBSetText(xmldbdata,render,&xmldoc);

  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata); 
  XMLSetComment(xmldata,redirect,xmldata);  

  xmldoc.script=ScriptMake();
  ScriptSetText(xmldoc.script,mapxml,&xmldoc);

  XMLdocSetText(xmldoc.doc,stream_output,stdout);

  if (remove !=NULL) {

    xmldoc.map.remove.num=remove->num;
    xmldoc.map.remove.txt=remove->txt;
  }

  if (ignore !=NULL) {
    xmldoc.map.ignore.num=ignore->num;
    xmldoc.map.ignore.txt=ignore->txt;
  }


  while(fgets(lbuf,255,xmlfp) !=NULL) {
    if (ilf) {
      m=0;
      for (n=0;(lbuf[n] !=0) && (n<256);n++) {
        if (lbuf[n]=='\n') continue;
        lbuf[m]=lbuf[n];
        m++;
      }    
      lbuf[m]=0;
    }
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }

  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);
  ScriptFree(xmldoc.script);
  fclose(xmlfp); 
  fflush(stdout);  
  return 0;

}
