/* multscan.h
   ==========
   Author: A.G. Burrell, NRL
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

#ifndef _FITBLK_H
#include "fitblk.h"
#endif

struct FitBeam
{
  int nrang;
  double time;
  unsigned char *sct;
  struct FitPrm prm;
  struct FitNoise noise;
  struct FitRange *rng;
  struct FitRange *xrng;
  struct FitElv *elv;
};

struct FitScan
{
  double st_time;
  double ed_time;
  int num_bms;
  struct FitBeam *bm;
  struct FitScan *next_scan;
  struct FitScan *prev_scan;
};

struct MultFitScan
{
  int stid;
  struct {
    int major;
    int minor;
  } version;

  double st_time;
  double ed_time;
  int num_scans;

  struct FitScan scan_data;
  struct FitScan *scan_ptr;
  struct FitScan *prev_ptr;
};

struct MultFitScan *MultRadarScanMake();
void MultFitScanFree(struct MultRadarScan *ptr);
int MultFitScanReset(struct MultRadarScan *ptr);
void FitScanFreeNext(struct FitScan *ptr);
void FitBeamFree(struct FitBeam *ptr);
struct FitScan *FitScanMake(int num_bms);

#endif
