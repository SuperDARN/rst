/* fontdb.h
   ======== 
   Author R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _FONTDB_H
#define _FONTDB_H

struct FrameBufferFontDB {
  char *path;
  char *buf;
  int flg;
  int num;
  struct FrameBufferFont **font;
  struct FrameBufferFont *dfont;
};

void FrameBufferFontDBFree(struct FrameBufferFontDB *ptr);
struct FrameBufferFontDB *FrameBufferFontDBLoad(FILE *fp);
struct FrameBufferFont *FrameBufferFontDBFind(struct FrameBufferFontDB *ptr,char *name,int sze);

#endif
 
