/* fontdb.h
   ======== 
   Author R.J.Barnes
*/

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
 
