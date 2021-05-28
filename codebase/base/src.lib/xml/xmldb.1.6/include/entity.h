/* entity.h
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




#ifndef _ENTITY_H
#define _ENTITY_H

struct Entitydata {
  struct {
    int num;
    struct XMLDBbuffer **buf;
    int *state;
  } entity;
  struct {
    int num;
    struct XMLDBbuffer **buf;
    int *state;
  } symbol;
  char *buf;
  int sze;
  int max;
  int stp;
  struct {
    int (*func)(char *,int,void *);
    void *data;
  } text;

  struct {
    int (*func)(char *,int,void *);
    void *data;
  } decode;

 struct {
    int (*func)(char *,int,void *);
    void *data;
  } encode;


};

struct Entitydata *EntityMake();

void EntityFree(struct Entitydata *ptr);

void EntityFreeSymbol(struct Entitydata *ptr);
void EntityFreeEntity(struct Entitydata *ptr);

int EntitySetText(struct Entitydata *ptr,
		  int (*text)(char *,int,
			      void *),void *data);


int EntitySetEncodeTrap(struct Entitydata *ptr,
		  int (*text)(char *,int,
			      void *),void *data);

int EntitySetDecodeTrap(struct Entitydata *ptr,
		        int (*text)(char *,int,
			      void *),void *data);



int EntityAddEntity(struct Entitydata *ptr,struct XMLDBbuffer *entity);

int EntityAddSymbol(struct Entitydata *ptr,struct XMLDBbuffer *symbol);


int EntityAddEntityString(struct Entitydata *ptr,char *str);
int EntityAddSymbolString(struct Entitydata *ptr,char *str);

int EntityAddEntityStringArray(struct Entitydata *ptr,char **str);
int EntityAddSymbolStringArray(struct Entitydata *ptr,char **str);

int EntityDecode(char *buf,int sze,void *data);
int EntityEncode(char *buf,int sze,void *data);


struct XMLDBbuffer *EntityDecodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src);
struct XMLDBbuffer *EntityEncodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src);

#endif


