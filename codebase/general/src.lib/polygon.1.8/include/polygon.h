/* polygon.h
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
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



