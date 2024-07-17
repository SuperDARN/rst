/* gtable.h
   ========
   Author: R.J.Barnes
*/

/*
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
#ifndef _GTABLE_h
#define _GTABLE_h

#define MAJOR_REVISION 2
#define MINOR_REVISION 0

struct GridBm {
    int bm;
    int frang;
    int rsep;
    int rxrise;
    int nrang;
    double *azm;
    double *srng;
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
    double srng;
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
    struct GridBm *bm;
    int pnum;
    struct GridPnt *pnt;
};

struct GridTable *GridTableMake();
void GridTableFree(struct GridTable *ptr);
int GridTableTest(struct GridTable *ptr,struct RadarScan *scan);
int GridTableMap(struct GridTable *ptr,struct RadarScan *scan,
                 struct RadarSite *pos,int tlen,int iflg,double alt,
                 int chisham,int old_aacgm);

#endif
