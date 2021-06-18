/* tagdb.c
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



#ifndef _TAGDB_H
#define _TAGDB_H

struct TagDBtag {
  char *name;
  struct {
    struct XMLDBbuffer *encode;
    struct XMLDBbuffer *decode;
  } start;

  struct {
    struct XMLDBbuffer *encode;
    struct XMLDBbuffer *decode;
  } end;

};

struct TagDBtable {
  int num;
  struct TagDBtag **tag;
};

struct TagDBdata {
  struct XMLdata *ptr;
  struct TagDBtable *tagdb;
  struct {
    int (*func)(char *,int,void *); 
    void *data;
  } text;
};

struct TagDBtable *TagDBMakeTable();
void TagDBFreeTable(struct TagDBtable *ptr);
int TagDBAddTable(struct TagDBtable *ptr,
             struct XMLDBbuffer *name,struct XMLDBbuffer *start,
             struct XMLDBbuffer *end); 
int TagDBBuild(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data);

 
int TagDBText(char *buf,int sze,void *data);
int TagDBEnd(char *name,char *buf,int sze,void *data);
int TagDBStart(char *name,char end,int atnum,char **atname,char **atval,
	       char *buf,int sze,void *data);
struct TagDBdata *TagDBMake(struct TagDBtable *tagdb);
void TagDBFree(struct TagDBdata *ptr);
int TagDBSetTable(struct TagDBdata *ptr,struct TagDBtable *tagdb);
int TagDBSetText(struct TagDBdata *ptr,
		int (*text)(char *,int,void *),void *data);

int TagDBDecode(char *buf,int sze,void *data);

#endif
