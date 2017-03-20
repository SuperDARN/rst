/* rmap.h
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
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

