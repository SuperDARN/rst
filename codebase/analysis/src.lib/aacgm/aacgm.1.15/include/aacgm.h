/* aacgm.h
   =======
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _AACGM_H
#define _AACGM_H

int AACGMLoadCoefFP(FILE  *fp);
int AACGMLoadCoef(char *fname);
int AACGMInit(int year);
int AACGMConvert(double in_lat,double in_lon,double height,
              double *out_lat,double *out_lon,double *r,
              int flag);
#endif


