/* tagdb.c
   ======= 
   Author: R.J.Barnes
*/

/*
   See license.txt
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
