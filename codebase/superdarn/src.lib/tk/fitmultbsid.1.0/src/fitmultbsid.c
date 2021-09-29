/* fitmultbsid.c
   ===========
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "fitmultbsid.h"


struct FitMultBSID *FitMultBSIDMake()
{
  struct FitMultBSID *ptr;

  ptr = (struct FitMultBSID *)malloc(sizeof(struct FitMultBSID));
  memset(ptr, 0, sizeof(struct FitMultBSID));
  ptr->num_scans = 0;
  ptr->scan_ptr  = (struct FitBSIDScan *)(NULL);
  ptr->last_ptr  = ptr->scan_ptr;

  return ptr;
}

void FitMultBSIDFree(struct FitMultBSID *ptr)
{
  if(FitMultBSIDReset(ptr) == 0) return;
  free(ptr);
  return;
}

int FitMultBSIDReset(struct FitMultBSID *ptr)
{
  int n;
  struct FitBSIDScan *scan;

  if(ptr == NULL) return(0);

  scan = ptr->scan_ptr;
  for(n = 0; n < ptr->num_scans; n++)
    FitBSIDScanFreeNext(scan);

  ptr->num_scans = 0;
  ptr->scan_ptr = (struct FitBSIDScan *)(NULL);
  ptr->last_ptr = (struct FitBSIDScan *)(NULL);
  return(1);
}

void FitBSIDScanFreeNext(struct FitBSIDScan *ptr)
{
  int ibm;
  struct FitBSIDBeam *bm;
  
  /* Free the current pointer and cycle to the next one */
  bm = ptr->bm;
  for(ibm = 0; ibm < ptr->num_bms; ibm++)
      FitBSIDBeamFree(&bm[ibm]);

  if(ptr->num_bms > 0)
    {
      free(ptr->bm);
      ptr->num_bms = 0;
    }

  ptr = ptr->next_scan;

  return;
}

void FitBSIDBeamFree(struct FitBSIDBeam *ptr)
{
  free(ptr->rng);
  free(ptr->med_rng);
  free(ptr->front_elv);
  free(ptr->back_elv);
  free(ptr->rng_flgs);
  free(ptr->front_loc);
  free(ptr->back_loc);
  return;
}

/**
 * @brief Cycle through scan, returning beam by beam number
 *
 * @param[in] ibm  - Zero-index beam number
 *            scan - Scan data structure
 *
 * @param[out] bm - Pointer to the desired beam structure
 **/

int get_bm_by_bmnum(int ibm, struct FitBSIDScan *scan)
{
  int i;

  struct FitBSIDBeam bm;

  if(ibm < scan->num_bms)
    {
      /* This scan may be ordered by beam number */
      i  = ibm;
      bm = scan->bm[i];

      /* This scan may be ordered by reverse beam order */
      if(bm.bm != ibm)
	{
	  i  = scan->num_bms - (ibm + 1);
	  bm = scan->bm[i];
	}

      if(bm.bm == ibm) return(i);
    }

  /* Not a clear relationship between beam index and beam number */
  i  = 0;
  bm = scan->bm[i];

  while(bm.bm != ibm && i < scan->num_bms)
    bm = scan->bm[++i];

  if(i >= scan->num_bms)
    {
      fprintf(stderr, "can't find beam number [%d] in scan with time [%f]\n",
	      ibm, scan->st_time);
      exit(1);
    }

  return(i);
}
