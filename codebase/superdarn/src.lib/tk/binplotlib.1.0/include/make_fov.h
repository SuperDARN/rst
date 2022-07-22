/* make_fov.h
   ========== 
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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

#ifndef _MAKEFOV_H
#define _MAKEFOV_H

struct PolygonData *make_fov(double tval,struct RadarNetwork *network,
                             float alt,int chisham);

struct PolygonData *make_field_fov(double tval,struct RadarNetwork *network,
                                   int id,int chisham);

struct PolygonData *make_grid_fov(double tval,struct RadarNetwork *network,
                                  int chisham,int old_aacgm);

struct PolygonData *make_grid_fov_data(struct GridData *ptr,struct RadarNetwork *network,
                                       int chisham,int old_aacgm);

#endif
