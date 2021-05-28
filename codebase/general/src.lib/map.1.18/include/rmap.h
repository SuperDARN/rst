/* rmap.h
   ====== 
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



#ifndef _RMAP_H
#define _RMAP_H

typedef int (*MapTFunction)(int,void *,int,void *,void *);

int MapCylindrical(int ssze,void *src,int dsze,void *dst,void *data);
int MapMercator(int ssze,void *src,int dsze,void *dst,void *data);
int MapTransverseMercator(int ssze,void *src,int dsze,void *dst,void *data);
int MapStereographic(int ssze,void *src,int dsze,void *dst,void *data);
int MapOrthographic(int ssze,void *src,int dsze,void *dst,void *data);
int MapGeneralVerticalPerspective(int ssze,
                                  void *src,int dsze,
                                  void *dst,void *data);

int MapGreatCircle(void *pnt1,void *pnt2,double f,void *dst);

struct PolygonData *MapFread(FILE *fp);
struct PolygonData *MapBndFread(FILE *fp);

struct PolygonData *MapCircleClip(float step);
struct PolygonData *MapSquareClip();
 
void MapPlotPolygon(struct Plot *plot,struct PlotMatrix *matrix,
                    float x,float y,float w,float h,int fill,
                    unsigned int color,unsigned char mask,
                    float width,struct PlotDash *dash,
                    struct PolygonData *poly,int type);
 

void MapPlotOpenPolygon(struct Plot *plot,struct PlotMatrix *matrix,
                    float x,float y,float w,float h,
                    unsigned int color,unsigned char mask,
                    float width,struct PlotDash *dash,
		    struct PolygonData *poly,int type);

struct PolygonData *MapWraparound(struct PolygonData *map);

struct PolygonData *MapTransform(struct PolygonData *src,
                                     int sze,
				     int (*bbox)(void *x,
						void *y,int s),
                                     int (*trf)(int ssze,void *src,int dsze,
                                                void *dst,void *data),void *data);


void MapModify(struct PolygonData *src,
                   int (*trf)(int ssze,void *src,int dsze,
                              void *dst,void *data),void *data);

#endif

