/* magcmp.c
   ========
   Author: R.J.Barnes
   Comments: E.G.Thomas (2016)
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>
#include <stdio.h>
#include "igrfcall.h"



/**
 * Calculates the IGRF magnetic field components at a given location in
 * local south/east/vertical coordinates.
 **/
int IGRFMagCmp(double date, double frho, double flat, double flon, 
               double *bx, double *by, double *bz, double *b) {

    int s;

    /* Calculate magnetic field components at field point in local
     * north/east/down coordinates [nT] */
    s=IGRFCall(date, flat, flon, frho, bx, by, bz);

    /* Calculate magnitude of magnetic field vector [nT] */
    *b = sqrt(*bx * *bx + *by * *by + *bz * *bz);

    /* Convert to local south/vertical coordinates (rather than north/down) */
    *bx = -*bx;
    *bz = -*bz;

    return s;

}
