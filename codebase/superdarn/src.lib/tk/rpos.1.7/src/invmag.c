/* invmag.c
   ========
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
   Comments: E.G.Thomas (2016)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rmath.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"
#include "igrflib.h"
#include "magcmp.h"
#include "radar.h"
#include "rpos.h"
#include "geodtgc.h"

#include "radar.h" 



/**
 * This function normalizes the components of a vector.
 **/
void norm_vec(double *x, double *y, double *z) {

    double r;

    /* Calculate magnitude of vector */
    r=sqrt(*x**x+*y**y+*z**z);

    /* Normalize each vector component to create unit vector */
    *x=*x/r;
    *y=*y/r;
    *z=*z/r;

}



/**
 * Converts from global geocentric spherical coordinates (r,theta,phi) to
 * global Cartesian coordinates (x,y,z), with input values in degrees.
 **/
void sphtocar(double r, double theta, double phi,
              double *x, double *y, double *z) {

    *x=r*sind(90.0-theta)*cosd(phi);
    *y=r*sind(90.0-theta)*sind(phi);
    *z=r*cosd(90.0-theta);

}



/**
 * This function uses the Haversine formula to calculate a destination
 * point (xlat,xlon) given distance (r) and bearing (az) from a start
 * point (frho,flat,flon).
 **/
void fldpnt_sph(double frho, double flat, double flon, double az,
                double r, double *xlat, double *xlon) {

    double api,aside,bside,cside;
    double Aangl,Bangl,arg;

    api=4*atan(1.0);
    cside=90.0-flat;
    Aangl=az;

    if (Aangl > 180) Aangl=Aangl-360;

    bside=r/frho*(180.0/api);

    arg=cosd(bside)*cosd(cside)+sind(bside)*sind(cside)*cosd(Aangl);

    if (arg <= -1.0) arg=-1.0;
    if (arg >= 1.0) arg=1.0;

    aside=acosd(arg);
    arg=(cosd(bside)-cosd(aside)*cosd(cside))/(sind(aside)*sind(cside));

    if (arg <= -1.0) arg=-1.0;
    if (arg >= 1.0) arg=1.0;

    Bangl=acosd(arg);
    if (Aangl <0) Bangl=-Bangl;

    *xlat=90-aside;
    *xlon=flon+Bangl;

    if (*xlon < 0) *xlon+=360;
    if (*xlon > 360) *xlon-=360;

}



/**
 * This function uses the Haversine formula to calculate bearing (az)
 * from a start point (mlat,mlon) to an end point (nlat,nlon) assuming
 * a spherical Earth.
 **/
void fldpnt_azm(double mlat, double mlon, double nlat, double nlon, double *az) {

    double aside,bside,cside;
    double Aangl,Bangl,arg;

    aside=90-nlat;
    cside=90-mlat;

    Bangl=nlon-mlon;

    arg=cosd(aside)*cosd(cside)+sind(aside)*sind(cside)*cosd(Bangl);
    bside=acosd(arg);

    arg=(cosd(aside)-cosd(bside)*cosd(cside))/
        (sind(bside)*sind(cside));

    Aangl=acosd(arg);

    if (Bangl<0) Aangl=-Aangl;
    *az=Aangl;

    /* Check for case when *az=nan rather than zero */
    if (*az!=*az) *az=0.0;

}



/**
 * Converts a vector from global Cartesian coordinates (rx,ry,rz) to
 * local south/east/vertical (horizontal) coordinates (tx,ty,tz) at a
 * given geocentric latitude and longitude (lat,lon).
 **/
void glbthor(int iopt, double lat, double lon,
             double *rx, double *ry, double *rz,
             double *tx, double *ty, double *tz) {

    double sx,sy,sz,lax;

    if (iopt>0) {
        /* Convert the input vector from Cartesian XYZ to local south/east/vertical */

        /* Rotate the input vector about the z-axis by the longitude */
        sx=cosd(lon)**rx+sind(lon)**ry;
        sy=-sind(lon)**rx+cosd(lon)**ry;
        sz=*rz;

        /* Calculate the colatitude */
        lax=90-lat;

        /* Rotate the vector about the east-axis by the colatitude */
        *tx=cosd(lax)*sx-sind(lax)*sz;
        *ty=sy;
        *tz=sind(lax)*sx+cosd(lax)*sz;

    } else {
        /* Convert the input vector from local south/east/vertical to Cartesian XYZ */

        /* Calculate the colatitude */
        lax=90-lat;

        /* Rotate the input vector about the east-axis by the latitude */
        sx=cosd(lax)**tx+sind(lax)**tz;
        sy=*ty;
        sz=-sind(lax)**tx+cosd(lax)**tz;

        /* Rotate the vector about the z-axis by the longitude */
        *rx=cosd(lon)*sx-sind(lon)*sy;
        *ry=sind(lon)*sx+cosd(lon)*sy;
        *rz=sz;

    }

}



/**
 * Calculates the geographic azimuth of a radar measurement in a given beam
 * and range gate cell (used to apply a velocity correction when gridding in
 * an inertial reference frame).
 **/
int RPosRngBmAzmElv(int bm, int rn, int year,
                    struct RadarSite *hdw, double frang,
                    double rsep, double rx, double height,
                    double *azm, double *elv, int chisham) {

    double flat,flon,frho;
    double fx,fy,fz;

    double gx,gy,gz;
    double glat,glon;
    double gdlat,gdlon,gdrho;
    double gbx,gby,gbz; 
    double ghx,ghy,ghz;
    double bx,by,bz,b;
    double dummy;
    int s;

    /* Get geodetic latitude/longitude from radar hardware info [deg] */
    gdlat=hdw->geolat;
    gdlon=hdw->geolon;

    /* Get receiver rise time from site information if not provided */
    if (rx==0) rx=hdw->recrise;

    /* Convert center of range/beam cell to geocentric spherical
     * latitude/longitude (flat,flon) and distance from the center of the
     * surface of the oblate spheroid (ie not constant with latitude) plus
     * virtual height (frho) */
    RPosGeo(1,bm,rn,hdw,frang,rsep,rx,
            height,&frho,&flat,&flon,chisham);

    /* Convert range/beam position from geocentric spherical coordinates
     * (frho,flat,flon) to global Cartesian coordinates (fx,fy,fz) */
    sphtocar(frho,flat,flon,&fx,&fy,&fz);

    /* Convert radar site geodetic latitude/longitude (gdlat,gdlon) to
     * geocentric spherical coordinates (glat,glon) and distance from the
     * center to the surface of the oblate spheroid (gdrho) */
    geodtgc(1,&gdlat,&gdlon,&gdrho,&glat,&glon,&dummy);

    /* Convert radar geocentric coordinates (gdrho,glat,glon) to global
     * Cartesian coordinates (gbx,gby,gbz) */
    sphtocar(gdrho,glat,glon,&gbx,&gby,&gbz);

    /* Calculate vector from the radar to center of range/beam cell (gx,gy,gz) */
    gx=fx-gbx;
    gy=fy-gby;
    gz=fz-gbz;

    /* Normalize the vector from the radar to center of range/beam cell */
    norm_vec(&gx,&gy,&gz);

    /* Convert the normalized vector from the radar-to-range/beam cell into
     * local south/east/vertical (horizontal) coordinates (ghx,ghy,ghz) */
    glbthor(1,flat,flon,&gx,&gy,&gz,&ghx,&ghy,&ghz);

    /* Normalize the local horizontal radar-to-range/beam cell vector */
    norm_vec(&ghx,&ghy,&ghz);

    /* Calculate the magnetic field vector (bx,by,bz) at the geocentric spherical
     * range/beam position (frho,flat,flon) in local south/east/vertical coordinates */
    s=IGRFMagCmp(year,frho,flat,flon,&bx,&by,&bz,&b);
    if (s==-1) return -1;

    /* Normalize the magnetic field vector */
    norm_vec(&bx,&by,&bz);

    /* Calculate a new local vertical component such that the radar-to-range/beam
     * vector becomes orthogonal to the magnetic field at the range/beam position
     * (gh dot b = 0) */
    ghz=-(bx*ghx+by*ghy)/bz;

    /* Normalize the new radar-to-range/beam vector (which is now orthogonal to B) */
    norm_vec(&ghx,&ghy,&ghz);

    /* Calculate the elevation angle of the orthogonal radar-to-range/beam vector */
    *elv=atan2d(ghz,sqrt(ghx*ghx+ghy*ghy));

    /* Calculate the azimuth of the orthogonal radar-to-range/beam vector */
    *azm=atan2d(ghy,-ghx);

    /* Return zero if successful */
    return 0;

}



/**
 * Calculates the magnetic latitude, longitude, and azimuth of a radar measurement
 * in a given beam and range gate cell.
 **/
int RPosInvMag(int bm, int rn, int year, struct RadarSite *hdw, double frang,
               double rsep, double rx, double height,
               double *mlat, double *mlon, double *azm, 
               int chisham, int magflg) {

    double flat,flon,frho;
    double fx,fy,fz;

    double gx,gy,gz;
    double glat,glon;
    double gdlat,gdlon,gdrho;
    double gbx,gby,gbz; 
    double ghx,ghy,ghz;
    double bx,by,bz,b;
    double dummy,azc;
    // double elv; 

    double tmp_ht;
    double xlat,xlon,nlat,nlon;
    int s;

    double out[3];

    /* Get geodetic latitude/longitude from radar hardware info [deg] */
    gdlat=hdw->geolat;
    gdlon=hdw->geolon;

    /* Get receiver rise time from site information if not provided */
    if (rx==0) rx=hdw->recrise;

    /* Convert center of range/beam cell to geocentric spherical
     * latitude/longitude (flat,flon) and distance from the center of the
     * surface of the oblate spheroid (ie not constant with latitude) plus
     * virtual height (frho) */
    RPosGeo(1,bm,rn,hdw,frang,rsep,rx,
            height,&frho,&flat,&flon,chisham);

    /* Convert range/beam position from geocentric spherical coordinates
     * (frho,flat,flon) to global Cartesian coordinates (fx,fy,fz) */
    sphtocar(frho,flat,flon,&fx,&fy,&fz);

    /* Convert radar site geodetic latitude/longitude (gdlat,gdlon) to
     * geocentric spherical coordinates (glat,glon) and distance from the
     * center to the surface of the oblate spheroid (gdrho) */
    geodtgc(1,&gdlat,&gdlon,&gdrho,&glat,&glon,&dummy);

    /* Convert radar geocentric coordinates (gdrho,glat,glon) to global
     * Cartesian coordinates (gbx,gby,gbz) */
    sphtocar(gdrho,glat,glon,&gbx,&gby,&gbz);

    /* Calculate vector from the radar to center of range/beam cell (gx,gy,gz) */
    gx=fx-gbx;
    gy=fy-gby;
    gz=fz-gbz;

    /* Normalize the vector from the radar to center of range/beam cell */
    norm_vec(&gx,&gy,&gz);

    /* Convert the normalized vector from the radar-to-range/beam cell into
     * local south/east/vertical (horizontal) coordinates (ghx,ghy,ghz) */
    glbthor(1,flat,flon,&gx,&gy,&gz,&ghx,&ghy,&ghz);

    /* Normalize the local horizontal radar-to-range/beam cell vector */
    norm_vec(&ghx,&ghy,&ghz);

    /* Calculate the magnetic field vector (bx,by,bz) at the geocentric spherical
     * range/beam position (frho,flat,flon) in local south/east/vertical coordinates */
    s=IGRFMagCmp(year,frho,flat,flon,&bx,&by,&bz,&b);
    if (s==-1) return -1;

    /* Normalize the magnetic field vector */
    norm_vec(&bx,&by,&bz);

    /* Calculate a new local vertical component such that the radar-to-range/beam
     * vector becomes orthogonal to the magnetic field at the range/beam position
     * (gh dot b = 0) */
    ghz=-(bx*ghx+by*ghy)/bz;

    /* Normalize the new radar-to-range/beam vector (which is now orthogonal to B) */
    norm_vec(&ghx,&ghy,&ghz);

    /* Calculate the elevation angle of the orthogonal radar-to-range/beam vector */
    // elv=atan2d(ghz,sqrt(ghx*ghx+ghy*ghy));

    /* Calculate the azimuth of the orthogonal radar-to-range/beam vector */
    azc=atan2d(ghy,-ghx);

    /* Convert range/beam position from geocentric (flat,flon) to geodetic
     * latitude/longitude (gdlat,gdlon) and calculate distance from the center
     * of the oblate spheroid (fdrho) */
    geodtgc(-1,&gdlat,&gdlon,&gdrho,&flat,&flon,&dummy);

    /* Calculate virtual height of range/beam position */
    tmp_ht=frho-gdrho;

    /* Convert range/beam position from geocentric latitude/longitude (flat,flon)
     * at virtual height (tmp_ht) to AACGM magnetic latitude/longitude
     * coordinates (mlat,mlon) */
    if (magflg == 2) {
      s=geod2ecdip(gdlat,gdlon,tmp_ht,out);
      *mlat = out[0];
      *mlon = out[1];
    } else if (magflg == 1) s=AACGMConvert(flat,flon,tmp_ht,mlat,mlon,&dummy,0);
    else s=AACGM_v2_Convert(flat,flon,tmp_ht,mlat,mlon,&dummy,0);
    if (s==-1) return -1;

    /* Calculate pointing direction latitude/longitude (xlat,xlon) given
     * distance (rsep) and bearing (azc) from the radar position (flat,flon)
     * at the field point radius (frho) */
    fldpnt_sph(frho,flat,flon,azc,rsep,&xlat,&xlon);

    /* Convert pointing direction position from geocentric latitude/longitude
     * (xlat,xlon) at virtual height (tmp_height) to AACGM magnetic
     * latitude/longitude coordinates (nlat,nlon) */
    if (magflg == 2) {
      geodtgc(-1,&gdlat,&gdlon,&gdrho,&xlat,&xlon,&dummy);
      s=geod2ecdip(gdlat,gdlon,tmp_ht,out);
      nlat = out[0];
      nlon = out[1];
    } else if (magflg == 1) s=AACGMConvert(xlat,xlon,tmp_ht,&nlat,&nlon,&dummy,0);
    else s=AACGM_v2_Convert(xlat,xlon,tmp_ht,&nlat,&nlon,&dummy,0);
    if (s==-1) return -1;

    /* Make sure nlon varies between +/- 180 degrees */
    if ((nlon-*mlon) > 180)  nlon=nlon-360;
    if ((nlon-*mlon) < -180) nlon=nlon+360;

    /* Calculate bearing (azm) to pointing direction latitude/longitude
     * (nlat,nlon) from the radar position (mlat,mlon) in magnetic
     * coordinates */
    fldpnt_azm(*mlat,*mlon,nlat,nlon,azm);

    /* Return zero if successful */
    return 0;

}
