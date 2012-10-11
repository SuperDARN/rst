/* rpos.h
   ======
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




#ifndef _RPOS_H
#define _RPOS_H


struct RPos {
  double gdlat;
  double gdlon;
  double boresite;
  double bmwidth;
  double rxris;
};

void RPosGeo(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);

void RPosMag(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);

void RPosCubic(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *x,double *y,double *z);

void RPosGeoGS(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);

void RPosMagGS(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);

void RPosCubicGS(int center,int bcrd,int rcrd,
              struct RadarSite *pos,
              int lagfr,int smsep,int rxrise,double height,
              double *x,double *y,double *z);



#endif
