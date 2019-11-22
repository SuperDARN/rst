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
#include "scandata.h"
#include "multscan.h"


struct MultRadarScan *MultRadarScanMake()
{
  struct MultRadarScan *ptr = NULL;

  ptr = (struct MultRadarScan *)malloc(sizeof(struct MultRadarScan));
  if(ptr == NULL) return NULL;

  memset(ptr, 0, sizeof(struct MultRadarScan));
  ptr->num_scans = 0;
  ptr->scan = &ptr->scan_ptr;
  ptr->scan_ptr = (struct RadarScanCycl *)(NULL);
  ptr->last_ptr = (struct RadarScanCycl *)(NULL);

  return ptr;
}

struct RadarScanCycl *RadarScanCyclMake()
{
  struct RadarScanCycl *ptr = NULL;

  ptr = (struct RadarScanCycl *)malloc(sizeof(struct RadarScanCycl));
  if(ptr == NULL) return NULL;

  memset(ptr, 0, sizeof(struct RadarScanCycl));
  ptr->scan_data = RadarScanMake();
  ptr->prev_scan = (struct RadarScanCycl *)(NULL);
  ptr->next_scan = (struct RadarScanCycl *)(NULL);

  return ptr;
}

void MultRadarScanFree(struct MultRadarScan *ptr)
{
    if(ptr == NULL) return;
    MultRadarScanReset(ptr);
    free(ptr);
}

void RadarScanCyclFree(struct RadarScanCycl *ptr)
{
    if(ptr == NULL) return;
    RadarScanCyclReset(ptr);
    free(ptr);
}

int MultRadarScanReset(struct MultRadarScan *ptr)
{
  int n;

  if(ptr == NULL) return -1;

  if(ptr->scan_ptr != NULL)
    {
      for (n=0; n<ptr->num_scans; n++)
	RadarScanCyclFree(ptr->scan_ptr);
      
      RadarScanCycleFree(ptr->last_ptr);
      free(ptr->scan_ptr);
      free(ptr->last_ptr);
    }

  ptr->num_scans = 0;
  ptr->scan_ptr = (struct RadarScanCycl *)(NULL);
  ptr->last_ptr = (struct RadarScanCycl *)(NULL);
  return 0;
}


int RadarScanCyclReset(struct RadarScanCycl *ptr)
{
  int n;

  if(ptr == NULL) return -1;

  ptr->scan_data = RadarScanFree();
  free(ptr->prev_scan);
  free(ptr->next_scan);
  ptr->prev_scan = (struct RadarScanCycl *)(NULL);
  ptr->next_scan = (struct RadarScanCycl *)(NULL);

  return 0;
}

