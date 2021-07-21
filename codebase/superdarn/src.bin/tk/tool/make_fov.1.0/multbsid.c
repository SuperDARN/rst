/* multbisd.c
   ===========
   Author A.G.Burrell
*/

/*
 LICENSE AND DISCLAIMER

 Copyright (c) 2019 NRL

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "multbsid.h"


struct MultFitBSID *MultFitBSIDMake()
{
  struct MultFitBSID *ptr;

  ptr = (struct MultFitBSID *)malloc(sizeof(struct MultFitBSID));
  memset(ptr, 0, sizeof(struct MultFitBSID));
  ptr->num_scans = 0;
  ptr->scan_ptr  = (struct FitBSIDScan *)(NULL);
  ptr->last_ptr  = ptr->scan_ptr;

  return ptr;
}

void MultFitBSIDFree(struct MultFitBSID *ptr)
{
    if(MultFitBSIDReset(ptr) == 0) return;

    free(ptr->scan_ptr);
    free(ptr->last_ptr);
    free(ptr);
}

int MultFitBSIDReset(struct MultFitBSID *ptr)
{
  int n;
  struct FitBSIDScan *scan;

  if(ptr == NULL) return(0);

  scan = ptr->scan_ptr;
  for(n = 0; n < ptr->num_scans; n++)
    FitBSIDScanFreeNext(scan);

  free(scan);
  free(ptr->scan_ptr);
  free(ptr->last_ptr);

  ptr->num_scans = 0;
  ptr->scan_ptr = (struct FitBSIDScan *)(NULL);
  ptr->last_ptr = (struct FitBSIDScan *)(NULL);
  return(1);
}

void FitBSIDScanFreeNext(struct FitBSIDScan *ptr)
{
  struct FitBSIDScan *prev_ptr;

  /* Free the current pointer and cycle to the next one */
  FitBSIDBeamFree(ptr->bm);
  free(ptr->prev_scan);
  prev_ptr = ptr;
  ptr      = ptr->next_scan;
  free(prev_ptr);

  /* If this is not the last pointer, close the reverse cycle */
  if(ptr != (struct FitBSIDScan *)(NULL))
    ptr->prev_scan = (struct FitBSIDScan *)(NULL);

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
  free(ptr);
  return;
}
