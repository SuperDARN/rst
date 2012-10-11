/* gtable.h
   ========
   Author: R.J.Barnes
*/


/*
 $Llicense$ 
*/




#ifndef _GTABLE_H
#define _GTABLE_h

#define MAJOR_REVISION 0
#define MINOR_REVISION 91


struct GridBm {
  int bm;
  int frang;
  int rsep;
  int rxrise;
  int nrang;
  double *azm;
  double *ival;
  int *inx;
};
  
struct GridPnt {
  int max;
  int cnt;
  int ref;
  double mlat;
  double mlon;
  double azm; 

  struct {
   double median;
   double median_n;
   double median_e;
   double sd;
  } vel;

  struct {
   double median;
   double sd;
  } pwr;

  struct {
   double median;
   double sd;
  } wdt;
  
};

struct GridTable {
  double st_time;
  double ed_time;
  int chn;
  int status;
  int st_id;
  int prog_id;
  int nscan;
  int npnt;
  double freq;
  struct {
    double mean;
    double sd;
  } noise;
  int gsct; 
  double min[4],max[4];
  int bnum;
  struct GridBm *bm;  int pnum;
  struct GridPnt *pnt;
};

struct GridTable *GridTableMake();
void GridTableFree(struct GridTable *ptr);
int GridTableTest(struct GridTable *ptr,struct RadarScan *scan,int tlen);
int GridTableMap(struct GridTable *ptr,struct RadarScan *scan,
                  struct RadarSite *pos,int tlen,int iflg,double alt);
 
#endif
