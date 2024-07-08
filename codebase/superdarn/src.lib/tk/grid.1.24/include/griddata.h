/* griddata.h
   ==========
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
#ifndef _GRIDDATA_H
#define _GRIDDATA_H

struct GridSVec {
    int st_id;
    int chn;
    int npnt;
    double freq0;
    char major_revision;
    char minor_revision;
    int prog_id;
    char gsct;
    struct {
        double mean;
        double sd;
    } noise;
    struct {
        double min;
        double max;
    } vel;
    struct {
        double min;
        double max;
    } pwr;
    struct {
        double min;
        double max;
    } wdt;
    struct {
        double min;
        double max;
    } verr;
};

struct GridGVec {
    double mlat,mlon;
    double azm;
    double srng;
    struct {
        double median;
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
    int st_id;
    int chn;
    int index;
};

struct GridData {
    double st_time;
    double ed_time;
    int stnum;
    int vcnum;
    unsigned char xtd;
    struct GridSVec *sdata;
    struct GridGVec *data;
};

struct GridData *GridMake();
void GridFree(struct GridData *ptr);

int GridLocateCell(int npnt,struct GridGVec *ptr,int index);
void GridMerge(struct GridData *,struct GridData *);
void GridAverage(struct GridData *,struct GridData *,int flg);

void GridCopy(struct GridData *a,struct GridData *b);
void GridAdd(struct GridData *a,struct GridData *b,int recnum);
void GridSort(struct GridData *ptr);
void GridIntegrate(struct GridData *a,struct GridData *b,double *err);

#endif
