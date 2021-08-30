/* multbsid.h
   ==========
   Author: A.G. Burrell
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

#ifndef _ZLIB_H
#include <zlib.h>
#endif

#ifndef _RTYPES_H
#include "rtypes.h"
#endif

#ifndef _DMAP_H
#include "dmap.h"
#endif

#ifndef _DMAP_H
#include "dmap.h"
#endif

#ifndef _RPRM_H
#include "rprm.h"
#endif

#ifndef _FITBLK_H
#include "scandata.h"
#endif

#ifndef _FITBLK_H
#include "fitblk.h"
#endif


struct CellBSIDFlgs
{
  int fov;
  int fov_past;
  int grpflg;
  int grpnum;
  char grpid[100];
};

struct CellBSIDLoc
{
  float vh;
  float vh_e;
  char vh_m[5];
  char region[20];
  float hop;
  float dist;
};  

struct FitBSIDBeam
{
  /* Set the beam constants */
  int cpid;
  int bm;
  float bmazm;
  double time;
  struct {
    int sc;
    int us;
  } intt;

  /* Set the beam parameter values */
  int nave;
  int frang;
  int rsep;
  int rxrise;
  int freq;
  int noise;
  int atten;
  int channel;

  /* Set the beam range-gate information */
  int nrang;
  unsigned char *sct;
  struct RadarCell *rng;
  struct RadarCell *med_rng;
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

  struct FitBSIDScan *scan_ptr;
  struct FitBSIDScan *last_ptr;
};

struct MultFitBSID *MultFitBSIDMake();
void MultFitBSIDFree(struct MultFitBSID *ptr);
int MultFitBSIDReset(struct MultFitBSID *ptr);
void FitBSIDScanFreeNext(struct FitBSIDScan *ptr);
void FitBSIDBeamFree(struct FitBSIDBeam *ptr);


#endif