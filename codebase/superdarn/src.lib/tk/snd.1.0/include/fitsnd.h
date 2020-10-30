/* fitsnd.h
   ========
   Author: E.G.Thomas
*/


#ifndef _FITSND_H
#define _FITSND_H

int FitToSnd(struct SndData *ptr, struct RadarParm *prm,
             struct FitData *fit, int scan);

#endif
