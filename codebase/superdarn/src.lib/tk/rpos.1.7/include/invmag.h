/* inv_mag.h
   =========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _INVMAG_H
#define _INVMAG_H


int RPosRngBmAzmElv(int bm,int rn,int year,
                  struct RadarSite *hdw,double frang,
                  double rsep,double rx,double height,
									double *azm,double *elv);

int RPosInvMag(int bm,int rn,int year,struct RadarSite *hdw,double frang,
             double rsep,double rx,double height,
             double *mlat,double *mlon,double *azm);

#endif
