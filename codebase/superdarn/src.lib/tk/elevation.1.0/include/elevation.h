/* elevation.h
   ===========
   Author: R.J.Barnes, K.Baker, S.G.Shepherd & Marina Schmidt
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

#ifndef _ELEVATION_H
#define _ELEVATION_H

/*Due to the differences in data structures and naming conventions between 
 * fitacf 2.5 and 3.0 this structure is implemented to contain what is needed 
 * by the current 3 possible elevation algorithms. In the future, I hope this structure can be removed 
 * and set data structure with consistent names are used so we can just pass the data structure. */

#include "fitdata.h"

double elevation_v2(struct elevation_data *, double psi_obs);

double elevation(struct elevation_data *, double phi0);

double elev_goose(struct elevation_data *, double phi0);

double elevation_error(struct elevation_data *, double sigma_2_a, double slope_a);
#endif
