/* scandata.h
   ==========
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
2021-09-21 Angeline G. Burrell: added `exclude_outofscan` routine to header.

*/
#ifndef _SCANDATA_H
#define _SCANDATA_H

struct RadarCell {
    int gsct;
    double p_0;
    double p_0_e;
    double v;
    double v_e;
    double w_l;
    double w_l_e;
    double p_l;
    double p_l_e;
    double phi0;
    double elv;
};

struct RadarBeam {
    int scan;
    int bm;
    float bmazm;
    double time;
    int cpid;
    struct {
        int sc;
        int us;
    } intt;
    int nave;
    int frang;
    int rsep;
    int rxrise;
    int freq;
    int noise;
    int atten;
    int channel;
    int nrang;
    unsigned char *sct;
    struct RadarCell *rng;
};

struct RadarScan {
    int stid;
    struct {
        int major;
        int minor;
    } version;

    double st_time;
    double ed_time;
    int num;
    struct RadarBeam *bm;
};

struct RadarScan *RadarScanMake();
void RadarScanFree(struct RadarScan *ptr);
int RadarScanReset(struct RadarScan *ptr);
int RadarScanResetBeam(struct RadarScan *ptr,int bmnum,int *bmptr);
struct RadarBeam *RadarScanAddBeam(struct RadarScan *ptr,int nrang);
int exclude_outofscan(struct RadarScan *ptr);

#endif
