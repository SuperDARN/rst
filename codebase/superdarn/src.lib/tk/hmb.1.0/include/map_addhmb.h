/*
 * function prototypes
 * Copyright (C) <year>  <name of author>
 *

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/
#ifndef _HMB_H
#define _HMB_H

/* the boundary array sizes correspond to the grid within which the HMB boundary
 * is searched for when determining the HMB from the data: [24 MLT hours]
 * and [36 Geomagnetic Latitudes, ranging from 40 to 76 degrees] */
extern float bndarr[24][36];
extern float lathmb[36];

struct hmbtab *load_hmb(FILE *fp);
void make_hmb(void);
void add_hmb_grd(float latmin,int yr,int yrsec,struct CnvMapData *map,
                 int magflg);
void map_addhmb(int yr, int yrsec, struct CnvMapData *map, int bndnp,
                float bndstep, float latref, float latmin, int magflg);
int latcmp(const void *a,const void *b);

#endif
