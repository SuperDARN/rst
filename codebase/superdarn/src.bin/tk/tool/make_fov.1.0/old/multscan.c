/* multscan.c
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
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "multscan.h"


struct MultFitScan *MultFitScanMake()
{
  struct MultFitScan *ptr;

  ptr = (struct MultFitScan *)malloc(sizeof(struct MultFitScan));
  memset(ptr, 0, sizeof(struct MultFitScan));
  ptr->num_scans = 0;
  ptr->scan_data = (struct FitScan)(NULL);
  ptr->scan_ptr  = &ptr->scan_data;
  ptr->prev_ptr  = (struct FitScan *)(NULL);

  return ptr;
}

void MultFitScanFree(struct MultFitScan *ptr)
{
    if(MultFitScanReset(ptr) == 0) return;

    free(ptr->scan_ptr);
    free(ptr->prev_ptr);
    free(ptr);
}

int MultFitScanReset(struct MultFitScan *ptr)
{
  int n;
  struct FitScan *scan;

  if(ptr == NULL) return 0;

  scan = ptr->scan_ptr;
  for (n = 0; n < ptr->num_scans; n++)
    FitScanFreeNext(scan);

  free(scan);
  free(ptr->scan_ptr);
  free(ptr->prev_ptr);

  ptr->num_scans = 0;
  ptr->scan_data = (struct FitScan)(NULL);
  ptr->scan_ptr  = &ptr->scan_data;
  ptr->prev_ptr  = (struct FitScan *)(NULL);
  return 1;
}

void FitScanFreeNext(struct FitScan *ptr)
{
  struct FitScan *prev_ptr;

  /* Free the current pointer and cycle to the next one */
  free(ptr->prev_ptr);
  prev_ptr = ptr;
  ptr      = ptr->next_ptr;
  free(prev_ptr);

  /* If this is not the last pointer, close the reverse cycle */
  if(ptr != (struct FitScan *)(NULL))
    ptr->prev_ptr = (struct FitScan *)(NULL);

  return;
}

void FitBeamFree(struct FitBeam *ptr)
{
  free(ptr->block);
  free(ptr->elv);
  free(ptr->rng);
  free(ptr);
  return;
}

struct FitScan *FitScanMake(int num_bms)
{
  struct FitScan *ptr;

  ptr = (struct FitScan *)malloc(sizeof(struct FitScan));
  memset(ptr, 0, sizeof(struct FitScan));
  ptr->num_bms = num_bms;

  if(num_bms == 0)
    ptr->bm = (struct FitBeam *)(NULL);
  else
    {
      ptr->bm = (struct FitBeam *)malloc(sizeof(struct FitBeam) * num_bms);
      memset(ptr->bm, 0, sizeof(struct FitBeam));
    }
      
  ptr->next_scan = (struct FitScan *)(NULL);
  ptr->prev_scan = (struct FitScan *)(NULL);

  return ptr;
}
