/* ground_scatter.c
   ================
   Author: R.J.Barnes & K.Baker
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



#include <math.h>
#include <stdio.h>
#include "fitblk.h"


/* 
   This version of the ground-scatter detection algorithm is based
   on a statistical analysis of ground-scatter vs ionospheric scatter
   done by Stefan Sundeen, Gerard Blanchard and Kile Baker.

   The analysis inluded all data from 12 days (with large amounts of both
   ionospheric scatter and ground scatter) distributed over all seasons of
   the year.  Two radars were used in the analysis, Saskatoon and Kapuskasing.

   The result of the initial statistical analysis showed that scatter was 
   most likely ground scatter if

           abs(v) < GS_VMAX - (GS_VMAX/GS_WMAX)*abs(width)

   Let g(vabs,wabs) = (abs(v) - GS_VMAX - (GS_VMAX/GS_WMAX)*abs(width))

   Then, if was assume the errors in v and width are independent,
   we can estimate the error in the function g to be:

   g_err/g = sqrt((v_err/vabs)^2 + (width_err/width)^2)

   Then the final condition for flagging data as ground scatter is:

   if g - g_err <= 0 then flag as ground scatter.

*/

/* the next two lines define the maximum values of
	velocity and spectral width that ground scatter can have */
	
#define GS_VMAX 30.
#define GS_WMAX 90.

int ground_scatter (struct FitRange *ptr) {

  double vabs,wabs,g,g_err;
  /* double dv, dw; */

  /*
    This is the old version.  No one seems to remember why
    it was done this way.

  vabs = ((fabs(ptr->v) - ptr->v_err) > 0.0) ? 
	      (fabs(ptr->v) - ptr->v_err) : 0.0;

  wabs = ((fabs(ptr->w_l) - ptr->w_l_err) > 0.0) ? 
	      (fabs(ptr->w_l) - ptr->w_l_err) : 0.0;

  if ((vabs < GS_VMAX) && (wabs < GS_WMAX)) return 1;

  */

  /* Here is the Sundeen-Blanchard-Baker version */

  vabs = fabs(ptr->v);
  wabs = fabs(ptr->w_l);
  g = (vabs - (GS_VMAX - (GS_VMAX/GS_WMAX)*wabs));

  /* don't allow the error to get too extreme.  If v_err/v or
     w_err/w is greater than 1 (100% error) just use 1.0 */

  /*
  dv = ((ptr->v_err/vabs) > 1.0) ? 1.0 :
    (ptr->v_err/vabs);

  dw = ((ptr->w_l_err/wabs) > 1.0) ? 1.0 :
    (ptr->w_l_err/wabs);
  */
  /*  g_err = fabs(g)*sqrt(dv*dv + dw*dw);*/
  g_err = 0; /* turn off any correction for the error */

  if ( (g - g_err) < 0.0) return 1;
  else return 0;
}



