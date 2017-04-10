/* imagedb.h
   ========= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _IMAGEDB_H
#define _IMAGEDB_H

struct FrameBufferDB {
  int num;
  struct FrameBuffer **img;
};

void FrameBufferDBFree(struct FrameBufferDB *ptr);
struct FrameBufferDB *FrameBufferDBMake();
int FrameBufferDBAdd(struct FrameBufferDB *ptr,struct FrameBuffer *img);
struct FrameBuffer *FrameBufferDBFind(struct FrameBufferDB *ptr,char *name);

#endif




