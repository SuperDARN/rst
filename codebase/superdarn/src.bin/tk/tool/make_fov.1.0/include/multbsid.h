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

#ifndef _FITBLK_H
#include "fitblk.h"
#endif

#ifndef _RPRM_H
#include "rprm.h"
#endif


struct CellBSIDFlgs
{
  int fov;
  int fov_past;
  int grpflg;
  int grpnum;
  char *grpid;
};

struct CellBSIDLoc
{
  float vh;
  float vh_e;
  char *vh_m;
  char *region;
  float hop;
  float dist;
};  

struct FitBSIDBeam
{
  int nrang;
  double time;
  struct FitPrm prm;
  struct FitNoise noise;
  unsigned char *sct;
  struct FitRange *rng;
  struct FitRange *med_rng;
  struct FitElv *front_elv;
  struct FitElv *back_elv;
  struct CellBSIDFlgs *rng_flgs;
  struct CellBSIDLoc *front_loc;
  struct CellBSIDLoc *back_loc;
};

struct FitBSIDScan
{
  double st_time;
  double ed_time;
  int num_bms;
  struct FitBSIDBeam *bm;
  struct FitBSIDScan *next_scan;
  struct FitBSIDScan *prev_scan;
};

struct MultFitBSID
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

  struct FitBSIDScan scan;
  struct FitBSIDScan *scan_ptr;
  struct FitBSIDScan *last_ptr;
};

struct MultFitBSID *MultFitBSIDMake();
void MultFitBSIDFree(struct MultFitBSID *ptr);
int MultFitBSIDReset(struct MultFitBSID *ptr);
int FitBSIDScanFreeNext(struct FitBSIDScan *ptr);
void FitBSIDBeamFree(struct FitBSIDBeam *ptr);


#endif
