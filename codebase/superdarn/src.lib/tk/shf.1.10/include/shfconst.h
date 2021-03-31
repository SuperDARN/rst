/* shfconst.h
   ==========
   Author: R.J.Barnes
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
