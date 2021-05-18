/* rxmldb.h
   ======== 
   Author: R.J.Barnes

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

 This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/



#ifndef _RXMLDB_H
#define _RXMLDB_H

struct XMLDBbuffer {
  int sze; 
  int max;
  int stp; 
 char *buf;
};

struct XMLDBtree {
  int depth;
  char **name;
};

struct XMLDBelement {
  char *name;
  char end;
  int atnum;
  char **atname; 
  char **atval;
  struct XMLDBbuffer *data;
}; 

struct XMLDBtable {
  int num;
  struct XMLDBelement **element;
};

struct XMLDBdata {
  struct XMLdata *ptr;
  struct XMLDBtree *tree;
  struct XMLDBtable *doc;
  struct XMLDBbuffer *buf;
  struct XMLDBtable *table;

  struct {
    int (*func)(struct XMLDBtable *,struct XMLDBtable *,void *);
    void *data;
  } text;
 
  int depth;
};

struct XMLDBbuffer *XMLDBCopyBuffer(struct XMLDBbuffer *);

struct XMLDBbuffer *XMLDBMakeBuffer(int stp);
void XMLDBFreeBuffer(struct XMLDBbuffer *ptr);
int XMLDBAddBuffer(struct XMLDBbuffer *ptr,char *str,int sze);

struct XMLDBelement *XMLDBMakeElement(char *name,char end,int atnum,
				      char **atname,char **atval,int stp);
void XMLDBFreeElement(struct XMLDBelement *ptr);


struct XMLDBtable *XMLDBMakeTable();
void XMLDBFreeTable(struct XMLDBtable *ptr);
int XMLDBRemoveTable(struct XMLDBtable *ptr);
int XMLDBAddTable(struct XMLDBtable *ptr,struct XMLDBelement *element);


struct XMLDBdata *XMLDBMake(struct XMLdata *xml);
void XMLDBFree(struct XMLDBdata *ptr);  

int XMLDBSetTree(struct XMLDBdata *ptr,struct XMLDBtree *tree);

int XMLDBSetText(struct XMLDBdata *ptr,
	      int (*text)(struct XMLDBtable *,struct XMLDBtable *,void *),
              void *data);

int XMLDBElementStart(char *name,char end,int atnum,char **atname,char **atval,
		      char *buf,int sze,void *data);

int XMLDBElementEnd(char *name,char *buf,int sze,void *data);

int XMLDBElementText(char *buf,int sze,void *data);


int XMLDBPeerStart(char *name,char end,int atnum,char **atname,char **atval,
		   char *buf,int sze,void *data);

int XMLDBPeerEnd(char *name,char *buf,int sze,void *data);

int XMLDBStart(char *name,char end,int atnum,char **atname,char **atval,
	       char *buf,int sze,void *data);

int XMLDBEnd(char *name,char *buf,int sze,void *data);

struct XMLDBtree *XMLDBMakeTree();
void XMLDBFreeTree(struct XMLDBtree *ptr);
int XMLDBBuildTree(char *path,char delim,struct XMLDBtree *ptr);

#endif

  





  
