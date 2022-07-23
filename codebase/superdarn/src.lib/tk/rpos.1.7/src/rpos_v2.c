/* rpos_v2.c
   =========
   Author: E.G.Thomas

   Copyright (C) 2022  Evan G. Thomas

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
#include <stdlib.h>
#include "radar.h"
#include "rpos.h"
#include "rpos_v2.h"
#include "geodtgc.h"
#include "rmath.h"


/**
 * This function converts a gate/beam coordinate to geographic
 * position. The height of the transformation is given by height -
 * if this value is less than 90 then it is assumed to be the
 * elevation angle from the radar. If center is not equal to zero, then
 * the calculation is assumed to be for the center of the cell, not the
 * edge. The calculated values are returned in geocentric spherical
 * coordinates (rho,lat,long). If model is equal to
 *   1 : the Chisham et al [2008] virtual height model is used;
 *   2 : the Thomas and Shepherd [2022] virtual height model is used;
 *   otherwise, the standard virtual height model is used.
 **/
void RPosGeo_v2(int center, int bcrd, int rcrd, struct RadarSite *pos,
                int frang, int rsep, int rxrise, double height,
                double *rho, double *lat, double *lng,
                double hop, int model, int gs, int rear) {

  double d,rx;
  double azi,elv,vh,grng;
  double rrad,rlat,rlon,del;
  double range_edge=0.0;
  int pseudo=0;

  if (center==0) range_edge=-0.5*rsep*20.0/3.0;

  if (rxrise==0) rx=pos->recrise;
  else rx=rxrise;

  /* Calculate the slant range to the range gate [km] */
  d = slant_range(frang,rsep,rx,range_edge,rcrd+1);

  /* Calculate virtual height from elevation angle or a model [km] */
  if (height<90.0) {
    elv = height;
    vh = calc_virtual_height(d,elv,hop);
  } else {
    if (model==1) {
      vh = calc_chisham_vhm(d,&hop);
      if (hop==1.5) pseudo = 1;
    } else if (model==2) {
      vh = calc_cv_vhm(d,gs,&hop);
    } else {
      vh = calc_standard_vhm(d,height);
    }
  
    /* Calculate elevation angle for slant range, virtual height, and number of hops [deg] */
    elv = calc_elevation_angle(d,vh,hop,pseudo);

    /* If using the Chisham pseudo virtual height, reset the number of hops back to 0.5
     * to get the correct ground range */
    if (pseudo) hop=0.5;
  }

  /* Calculate ground range for slant range, virtual height, and number of hops [km] */
  grng = calc_ground_range(d,vh,hop);

  /* Correct beam azimuth for coning angle and elevation angle [deg] */
  azi = calc_coning_angle(pos,bcrd,elv,center,rear);

  /* Convert radar location from geodetic to geocentric latitude/longitude coordinates */
  geodtgc(1,&pos->geolat,&pos->geolon,&rrad,&rlat,&rlon,&del);

  /* Calculate final latitude and longitude from site for ground range and beam azimuth
   * for a great-circle path along the Earth's surface */
  gc_destination(rlat,rlon,grng,azi,lat,lng);

}


/* Standard virtual height model */
double calc_standard_vhm(double r, double fh) {

  double xh;

  if (fh<=150) xh=fh;
  else {
    if (r<=600) xh=115;
    else if ((r>600) & (r<800)) xh=(r-600)/200*(fh-115)+115;
    else xh=fh;
  }
  if (r<150) xh=(r/150.0)*115.0;

  return xh;
}


/* Chisham et al [2008] Saskatoon virtual height model */
double calc_chisham_vhm(double r, double *hop) {

  double xh;

  double A_const[3]={108.974,384.416,1098.28};
  double B_const[3]={0.0191271,-0.178640,-0.354557};
  double C_const[3]={6.68283e-5,1.81405e-4,9.39961e-5};

  *hop=0.5;

  if (r<787.5) xh=A_const[0]+B_const[0]*r+C_const[0]*r*r;
  else if (r<=2137.5) xh=A_const[1]+B_const[1]*r+C_const[1]*r*r;
  else {
    xh=A_const[2]+B_const[2]*r+C_const[2]*r*r;
    *hop=1.5;
  }

  if (r<115) xh=(r/115.0)*112.0;

  return xh;
}


/* Thomas and Shepherd [2022] Christmas Valley virtual height model */
double calc_cv_vhm(double r, int gs, double *hop) {

  double xh;

  if (gs) {
    /* Ground scatter virtual height model (1E, 1F, 2F) */
    double A_const[3]={111.393,377.059,-98.3996};
    double B_const[3]={-1.65773e-4,-0.14654,0.08004};
    double C_const[3]={4.26675e-5,6.97736e-5,1.08429e-5};

    *hop=1.0;

    if (r<1140.0) xh=A_const[0]+B_const[0]*r+C_const[0]*r*r;
    else if (r<=3262.5) xh=A_const[1]+B_const[1]*r+C_const[1]*r*r;
    else {
      xh=A_const[2]+B_const[2]*r+C_const[2]*r*r;
      *hop=2.0;
    }
  } else {
    /* Ionospheric scatter virtual height model (0.5E, 0.5F, 1.5F) */
    double A_const[3]={107.946,344.032,93.6802};
    double B_const[3]={-0.01041,-0.18124,0.04002};
    double C_const[3]={1.53713e-4,2.02043e-4,1.57375e-5};

    *hop=0.5;

    if (r<675.0) xh=A_const[0]+B_const[0]*r+C_const[0]*r*r;
    else if (r<=2272.5) xh=A_const[1]+B_const[1]*r+C_const[1]*r*r;
    else {
      xh=A_const[2]+B_const[2]*r+C_const[2]*r*r;
      *hop=1.5;
    }
  }

  if (r<115) xh=(r/115.0)*95.0;

  return xh;
}


/* Calculate the virtual height for a given slant range, elevation angle,
 * and number of hops */
double calc_virtual_height(double r, double elv, double hop) {

  return sqrt(RE*RE + (r*0.5/hop)*(r*0.5/hop) + 2.0*r*(0.5/hop)*RE*sind(elv))-RE;
}


/* Calculate the elevation angle for a given slant range, virtual height,
 * and number of hops */
double calc_elevation_angle(double r, double xh, double hop, int pseudo) {

  double gmma,beta;

  if (pseudo) {
    /* If using a pseudo virtual height from the Chisham et al VHM, need to calculate the
     * elevation angle to the true virtual height for a 1.5-hop propagation mode */
    gmma = acosd((RE*RE + (RE+xh)*(RE+xh) - r*r)/(2.0*RE*(RE+xh)));
    beta = asind(RE*sind(gmma/3.0)/(r/3.0));
    return 90.0 - beta - (gmma/3.0);
  } else {
    return asind(((RE+xh)*(RE+xh) - (r*0.5/hop)*(r*0.5/hop) - RE*RE)/(2.0*r*(0.5/hop)*RE));
  }
}


/* Calculate the coning angle correction for a given radar, beam
 * number, elevation angle, and FOV direction (ie front vs rear) */
double calc_coning_angle(struct RadarSite *pos, int bmnum, double elv,
                         int center, int rear) {

  double azi,offset,psi;
  double bm_edge=0.0;

  if (center==0) bm_edge=-pos->bmsep*0.5;

  offset=pos->maxbeam/2.0-0.5;

  /* Beam direction relative to boresite at zero elevation angle */
  psi=pos->bmsep*(bmnum-offset)+bm_edge+pos->bmoff;

  azi=asind(sind(psi)/cosd(elv));

  /* Return the corresponding value of pointing azimuth */
  if (rear) {
    return -1.0*azi+pos->boresite-180.0;
  } else {
    return azi+pos->boresite;
  }
}


/* Calculate the ground range for a given slant range, virtual height,
 * and number of hops */
double calc_ground_range(double r, double xh, double hop) {

  return (hop/0.5)*RE*acos((RE*RE + (RE+xh)*(RE+xh) - (r*(0.5/hop))*(r*(0.5/hop)))/(2.0*RE*(RE+xh)));
}


/* Calculate the destination latitude and longitude for a great circle
 * path with a given start latitude/longitude position, distance, and
 * azimuth */
void gc_destination(double lat1, double lon1, double d, double azi,
                    double *lat2, double *lon2) {

  *lat2 = asind(sind(lat1)*cos(d/RE) + cosd(lat1)*sin(d/RE)*cosd(azi));
  *lon2 = lon1 + atan2d(sind(azi)*sin(d/RE)*cosd(lat1), cos(d/RE)-sind(lat1)*sind(*lat2));
}
