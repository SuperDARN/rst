/* rxmldoc.h
   ========= 
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



#ifndef _RXMLDOC_H
#define _RXMLDOC_H

struct XMLdocfile {
  char *path;
  char *fname;
  int type;
  int (*text)(char *,int,void *); /* renderer */
  void *data;
};


struct XMLdocmod {
  char *name;
  char *path;
  struct TagDBtable *tagdb;
   struct {
     int action; /* what form is the action text/file */
     int xml; /* what form is the XML data text/file */
  } type;
  struct XMLDBbuffer *action; 
};

struct XMLdocmodtable {
  int num;
  struct XMLdocmod **ptr;
};

struct XMLdocelement {
  char *name;
  char *path;
  struct TagDBtable *tagdb;
  struct {
    int action; /* what form is the action text/file */
    int defval; /* what form is the default value text/file */
    int xml; /* what form is the XML data text/file */
  } type;
  struct XMLDBbuffer *action;  
  struct XMLDBbuffer *defval;
  struct XMLdocmodtable mod;
};

struct XMLdoctable {
  int num;
  struct XMLdocelement **ptr;
};

struct XMLdocdata {
  struct TagDBtable *tagdb;
  struct TagDBdata *tag;
  struct Entitydata *ent;
  struct XMLDBbuffer *root; /* document root */
  struct XMLdoctable xml;
};

int XMLdocBuild(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data);
struct XMLdocdata *XMLdocMake();
void XMLdocFree(struct XMLdocdata *ptr);
int XMLdocSetText(struct XMLdocdata *ptr,
             int (*text)(char *,int,
                         void *),void *data);
int XMLdocRender(struct XMLDBtable *ptr,struct XMLdocdata *xmldoc,int index);

int XMLdocGetConfigSection(struct XMLDBbuffer *ptr,char *root,char *path,
              int (*text)(struct XMLDBtable *,struct XMLDBtable *,void *),
			   void *data);

char *getDBstring(struct XMLDBbuffer *ptr);
int XMLdocIncludeText(struct XMLdocdata *xmldoc,char *buf,int sze);


#endif


