/* contour.h
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




#ifndef _CONTOUR_H
#define _CONTOUR_H


#define contour_INT 1
#define contour_FLOAT 2
#define contour_DOUBLE 4

void ContourFree(int cnum,struct PolygonData **ptr);


struct PolygonData **Contour(void *zbuffer,int type,
		            int wdt,int hgt,int cnum,void *cval,
		            int smooth,int flag,int rflag,int bflag);

struct PolygonData **ContourTransform(int cnum,struct PolygonData **src,
                                      int (*trf)(int ssze,void *src,int dsze,
                                                 void *dst,void *data),
                                      void *data);

void ContourModify(int cnum,struct PolygonData **src,
                                      int (*trf)(int ssze,void *src,int dsze,
                                                 void *dst,void *data),
		   void *data);

void ContourFree(int cnum,struct PolygonData **src);
                 
#endif
