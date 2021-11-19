/* fitmultbsid.h
   =============
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#ifndef _FITMULTBSID_H
#define _FITMULTBSID_H

/* Ensure the neccessary library headers are included */
#ifndef _ZLIB_H
#include <zlib.h>
#endif

#ifndef _RTYPES_H
#include "rtypes.h"
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
  char grpid[2];
};

struct CellBSIDLoc
{
  float vh;
  float vh_e;
  char vh_m[2];
  char region[2];
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

struct FitMultBSID
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

struct FitMultBSID *FitMultBSIDMake();
void FitMultBSIDFree(struct FitMultBSID *ptr);
int FitMultBSIDReset(struct FitMultBSID *ptr);
void FitBSIDScanFreeNext(struct FitBSIDScan *ptr);
void FitBSIDBeamFree(struct FitBSIDBeam *ptr);
int get_bm_by_bmnum(int ibm, struct FitBSIDScan *scan);

int FitMultBSIDHeaderDecode(struct DataMap *ptr, struct FitMultBSID *mult_scan);
int FitBSIDScanDecode(struct DataMap *ptr, struct FitBSIDScan *scan);
int FitBSIDBeamDecode(struct DataMap *ptr, struct FitBSIDBeam *bm);
int ReadFitMultBSIDBin(FILE *fp, struct FitMultBSID *mult_scan);

void WriteFitMultBSIDASCII(FILE *fp, struct FitMultBSID *mult_scan);
void WriteFitBSIDScanASCII(FILE *fp, int stid, struct FitBSIDScan *scan);
int WriteFitMultBSIDBin(FILE *fp, int grp_flg, int med_flg,
			struct FitMultBSID *mult_scan);
int WriteFitBSIDBeamBin(int fid, int grp_flg, int med_flg,
			struct FitBSIDBeam bm);
int FitBSIDBeamEncode(int grp_flg, int med_flg, struct DataMap *ptr,
		      struct FitBSIDBeam bm);

#endif
