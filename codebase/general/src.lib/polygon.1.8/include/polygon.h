/* polygon.h
   =========
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/




#ifndef _POLYGON_H
#define _POLYGON_H

struct PolygonData {
  int polnum;
  int pntnum;
  int *num;
  int *off;
  int *type;
  int sze;
  void *data;
  void *box;
  int (*bbox)(void *a,void *b,int);
};

int PolygonXYbbox(void *a,void *b,int s);
struct PolygonData *PolygonMake(int sze,int (*bbox)(void *a,void *b,int s));
void PolygonFree(struct PolygonData *a);
int PolygonAddPolygon(struct PolygonData *a,int type);
int PolygonAdd(struct PolygonData *a,void *pnt);
void *PolygonRead(struct PolygonData *ptr,int n,int o);
int PolygonReadType(struct PolygonData *ptr,int n);
int PolygonRemove(struct PolygonData *a);
struct PolygonData *PolygonCopy(struct PolygonData *src);
struct PolygonData *PolygonClip(struct PolygonData *apol,
                                struct PolygonData *bpol);
#endif



