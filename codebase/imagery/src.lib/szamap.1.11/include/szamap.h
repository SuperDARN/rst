/* szamap.c
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



int SZATransform(int ssze,void *src,int dsze,void *dst,void *data);

struct PolygonData **SZAContour(int yr,int mo,int dy,int hr,int mt,int sc,
                                 int flg,int mode,float step,int znum,
                                double *zenith);

struct PolygonData *SZATerminator(int yr,int mo,int dy,int hr,int mt,int sc,
                                    int flg,int mode,float step,
				  float zenith);

float *SZAMap(int yr,int mo,int dy,int hr,int mt,int sc,float latmin,
               int wdt,int hgt,int mode,
               int (*trf)(int ssze,void *src,int dsze, void *dst,void *data),
	      void *data);
