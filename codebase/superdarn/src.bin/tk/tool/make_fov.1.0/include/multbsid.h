/* scandata.h
   ==========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _MULTBSID_H
#define _MULTBSID_H

#ifndef _SCANDATA_H
#include "scandata.h"
#endif

struct CellBSIDFlgs
{
  int fov;
  int grpflg;
  int grpnum;
  char *grpid;
};

struct CellBSIDLoc
{
  double elv;
  double elv_e;
  double phi0;
  double phi0_e;
  double vh;
  double vh_e;
  char *vh_m;
  double lat;
  double lon;
  double hop;
};  

struct RadarBSIDBeam
{
    int scan;
    int bm;
    float bmazm;
    double time;
    int cpid;
    struct
    {
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
    struct RadarCell *med_rng;
    struct CellBSIDFlgs *rng_flgs;
    struct CellBSIDLoc *rng_loc;
};

struct RadarBSIDScan
{
  double st_time;
  double ed_time;
  int num;
  struct RadarBSIDBeam *bm;
};

struct RadarBSIDCycl
{
  struct RadarBSIDScan *scan_data;
  struct RadarBSIDCycl *next_scan;
  struct RadarBSIDCycl *prev_scan;
};

struct MultRadarBSID
{
  int stid;
  struct
  {
    int major;
    int minor;
  } version;

  double st_time;
  double ed_time;
  int num_scans;

  struct RadarBSIDCycl scan;
  struct RadarBSIDCycl *scan_ptr;
  struct RadarBSIDCycl *last_ptr;
};

/* struct MultRadarScan *MultRadarScanMake();
 * void MultRadarScanFree(struct MultRadarScan *ptr);
 * int MultRadarScanReset(struct MultRadarScan *ptr);
 * int MultRadarScanResetScan(struct MultRadarScan *ptr, int *scan_ptr);
 * struct RadarScan *MultRadarScanAddScan(struct MultRadarScan *ptr);
 */

#endif
