/* ground_scatter.c
   ================
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
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
  double dv, dw;

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

  dv = ((ptr->v_err/vabs) > 1.0) ? 1.0 :
    (ptr->v_err/vabs);

  dw = ((ptr->w_l_err/wabs) > 1.0) ? 1.0 :
    (ptr->w_l_err/wabs);

  /*  g_err = fabs(g)*sqrt(dv*dv + dw*dw);*/
  g_err = 0; /* turn off any correction for the error */

  if ( (g - g_err) < 0.0) return 1;
  else return 0;
}



