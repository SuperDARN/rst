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

#ifndef _MULTSCAN_H
#define _MULTSCAN_H

#ifndef _SCANDATA_H
#include "scandata.h"
#endif

struct RadarScanCycl
{
  struct RadarScan scan_data;
  struct RadarScanCycl *next_scan;
  struct RadarScanCycl *prev_scan;
};

struct MultRadarScan
{
  int stid;
  struct {
    int major;
    int minor;
  } version;

  double st_time;
  double ed_time;
  int num_scans;

  struct RadarScanCycl scan;
  struct RadarScanCycl *scan_ptr;
  struct RadarScanCycl *last_ptr;
};

/* struct MultRadarScan *MultRadarScanMake();
 * void MultRadarScanFree(struct MultRadarScan *ptr);
 * int MultRadarScanReset(struct MultRadarScan *ptr);
 * int MultRadarScanResetScan(struct MultRadarScan *ptr, int *scan_ptr);
 * struct RadarScan *MultRadarScanAddScan(struct MultRadarScan *ptr);
 */

#endif
