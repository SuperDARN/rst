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

struct hmbtab *load_hmb(FILE *fp);
void make_hmb(void);
void add_hmb_grd(float latmin,int yr,int yrsec,struct CnvMapData *map,
                 int old_aacgm);
void map_addhmb(int yr, int yrsec, struct CnvMapData *map, int bndnp,
                float bndstep, float latref, float latmin, int old_aacgm);
int latcmp(const void *a,const void *b);

#endif
