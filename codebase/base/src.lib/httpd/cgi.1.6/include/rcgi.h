/* rcgi.h
   ====== 
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




#ifndef _RCGI_H
#define _RCGI_H

struct CGIarg {
  char *name;
  char *fname;
  int type;
  struct {
    struct {
      int sze;
      char *buf;
    } raw;
    struct {
      int sze;
      char *buf;
    } txt;
  } value;
};

struct CGIdata {
  int num;
  char *buf;
  int sze;
  int stp;
  int max;
  int state;
  int cnt; 
  char *bnd;

  struct {
    int (*func)(struct CGIarg *,char *,int,void *);
    void *data;
  } text;

  struct {
    int state;
    int sze;
    char *buf;
    int stp;
    int max;
  } mime;

  struct CGIarg **arg;
};

char *CGIGetString(struct CGIarg *);
char *CGIGetRawString(struct CGIarg *);
char *CGIMakeURLString(char *inp,int sze);

struct CGIdata *CGIMake();
void CGIFree(struct CGIdata *ptr);
int CGISetText(struct CGIdata *ptr,int (*text)(struct CGIarg *,char *,
					       int,void *),void *data);
int CGIFindInBuf(char *buf,int sze,char *str);
int CGIGet(struct CGIdata *ptr);
 
#endif
