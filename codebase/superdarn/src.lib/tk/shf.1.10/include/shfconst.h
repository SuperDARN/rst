/* shfconst.h
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



#ifndef _SHFCONST_H
#define _SHFCONST_H

/* radius of the earth (polar radius) */
#define Re (6357.0*1000.0)      

/* assummed altitude of scattering region */
#define Altitude (300.0*1000.0)

/* Magnetic field strength at N pole */
#define BNorth (-0.58e-4)

/* Magnetic field strength at S pole */
#define BSouth (-0.68e-4)
/* note: we use a negative value even for S pole, because the code for handling
   the S pole data does its own switching */

#define Radial_Dist (Re + Altitude)

#endif
