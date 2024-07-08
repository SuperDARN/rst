/* cnvtcoord.c
   ===========
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
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
    Comments: E.G.Thomas (2016)
    2020-03-11 Marina Schmidt removed earth's radius defined constant 
    E.G.Thomas 2021-08: added support for new hdw file fields
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "radar.h"
#include "rpos.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"
#include "rmath.h"
/*
 *rmath.h provides the earth's radius (RE)
 */

/**
 * Calculates the slant range to a range gate.
 **/
double slant_range(int frang, int rsep,
                   double rxris, double range_edge,
                   int range_gate) {

    int lagfr,smsep;

    /* Calculate the lag to first range gate in microseconds */
    lagfr=frang*20/3;

    /* Calculate the sample separation in microseconds */
    smsep=rsep*20/3;

    /* Return the calculated slant range [km] */
    return (lagfr-rxris+(range_gate-1)*smsep+range_edge)*0.15;

}



/**
 * Converts from geodetic coordinates (gdlat,gdlon) to geocentric spherical
 * coordinates (glat,glon). The radius of the Earth (gdrho) and the deviation
 * off the vertical (del) are calculated. The WGS 84 oblate spheroid model
 * of the Earth is adopted.
 **/
void geodtgc(int iopt, double *gdlat, double *gdlon,
             double *grho, double *glat,
             double *glon, double *del) {

    /* WGS 84 oblate spheroid defining parameters */
    double a=6378.137;
    double f=1.0/298.257223563;
    double b,e2;

    /* Ellipsoid semi-minor axis */
    b=a*(1.0-f);

    /* Second eccentricity squared */
    e2=(a*a)/(b*b)-1;

    if (iopt>0) {

        /* Convert geodetic latitude/longitude to geocentric [deg] */
        *glat=atand( (b*b)/(a*a)*tand(*gdlat));
        *glon=*gdlon;

        /* Convert geocentric longitude to +/- 180 degrees */
        if (*glon > 180) *glon=*glon-360;

    } else {

        /* Convert geocentric latitude/longitude to geodetic [deg] */
        *gdlat=atand( (a*a)/(b*b)*tand(*glat));
        *gdlon=*glon;

    }

    /* Calculate the geocentric Earth radius at the geodetic latitude [km] */
    *grho=a/sqrt(1.0+e2*sind(*glat)*sind(*glat));

    /* Calculate the deviation of the vertical [deg] */
    *del=*gdlat-*glat;

}



/**
 * Calculates the geocentric coordinates (frho,flat,flon) of a field point given
 * the angular geocentric coordinates (rrho,rlat,rlon) of the point of origin,
 * the azimuth (ral), elevation (rel), and slant range (r).
 **/
void fldpnt(double rrho, double rlat, double rlon, double ral,
            double rel, double r, double *frho, double *flat,
            double *flon) {

    double rx,ry,rz,sx,sy,sz,tx,ty,tz;
    double sinteta;

    /* Convert from global spherical (rrho,rlat,rlon) to global Cartesian
     * (rx,ry,rz: Earth centered) */
    sinteta=sind(90.0-rlat);
    rx=rrho*sinteta*cosd(rlon);
    ry=rrho*sinteta*sind(rlon);
    rz=rrho*cosd(90.0-rlat);

    /* Convert from local spherical (r,ral,rel) to local Cartesian
     * (sx,sy,sz: south,east,up) */
    sx=-r*cosd(rel)*cosd(ral);
    sy=r*cosd(rel)*sind(ral);
    sz=r*sind(rel);

    /* Convert from local Cartesian to global Cartesian */
    tx  =  cosd(90.0-rlat)*sx + sind(90.0-rlat)*sz;
    ty  =  sy;
    tz  = -sind(90.0-rlat)*sx + cosd(90.0-rlat)*sz;
    sx  =  cosd(rlon)*tx - sind(rlon)*ty;
    sy  =  sind(rlon)*tx + cosd(rlon)*ty;
    sz  =  tz;

    /* Find global Cartesian coordinates of new point by vector addition */
    tx=rx+sx;
    ty=ry+sy;
    tz=rz+sz;

    /* Convert from global Cartesian to global spherical */
    *frho=sqrt((tx*tx)+(ty*ty)+(tz*tz));
    *flat=90.0-acosd(tz/(*frho));
    if ((tx==0) && (ty==0)) *flon=0;
    else *flon=atan2d(ty,tx);

}



/**
 * 
 **/
void geocnvrt(double gdlat, double gdlon,
              double xal, double xel, double *ral, double *rel) {

    double kxg,kyg,kzg,kxr,kyr,kzr;
    double rrad,rlat,rlon,del;

    kxg=cosd(xel)*sind(xal);
    kyg=cosd(xel)*cosd(xal);
    kzg=sind(xel);
    geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
    kxr=kxg;
    kyr=kyg*cosd(del)+kzg*sind(del);
    kzr=-kyg*sind(del)+kzg*cosd(del);

    *ral=atan2d(kxr,kyr);
    *rel=atand(kzr/sqrt((kxr*kxr)+(kyr*kyr)));

}



/**
 * Calculates the geocentric coordinates (frho,flat,flon) of a radar field point,
 * using either the standard or Chisham virtual height model.
 **/
void fldpnth(double gdlat, double gdlon, double psi, double bore,
             double fh, double r, double *frho, double *flat,
             double *flon, int chisham) {

    double rrad,rlat,rlon,del;
    double tan_azi,azi,rel,xel,fhx,xal,rrho,ral,xh;
    double dum,dum1,dum2,dum3;
    double frad;
    double gmma,beta;

    if (chisham) {
        /* Chisham virtual height model */
        double A_const[3]={108.974,384.416,1098.28};
        double B_const[3]={0.0191271,-0.178640,-0.354557};
        double C_const[3]={6.68283e-5,1.81405e-4,9.39961e-5};

        if (r<787.5) xh=A_const[0]+B_const[0]*r+C_const[0]*r*r;
        else if (r<=2137.5) xh=A_const[1]+B_const[1]*r+C_const[1]*r*r;
        else xh=A_const[2]+B_const[2]*r+C_const[2]*r*r;
        
        if (r<115) xh=(r/115.0)*112.0;
    } else {
        /* Standard virtual height model */
        if (fh<=150) xh=fh;
        else {
            if (r<=600) xh=115;
            else if ((r>600) && (r<800)) xh=(r-600)/200*(fh-115)+115;
            else xh=fh;
        }
        if (r<150) xh=(r/150.0)*115.0;
    }

    /* Calculate radius of the Earth beneath the radar */
    geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);

    /* Radius of the Earth beneath the radar */
    rrho=rrad;

    /* Radius of the Earth beneath the field point (updates) */
    frad=rrad;

    /* Check for zero slant range which will cause an error in the
     * elevation angle calculation below, leading to a NAN result */
    if (r==0) r=0.1;

    /* Iterate until the altitude corresponding to the calculated elevation
     * matches the desired altitude (within 0.5 km) */
    do {

        /* Distance from center of Earth to field point location */
        *frho=frad+xh;

        /* Elevation angle relative to local horizon [deg] */
        rel=asind(((*frho**frho) - (rrad*rrad) - (r*r))/(2.0*rrad*r));

        /* Need to calculate actual elevation angle for 1.5-hop propagation
         * when using Chisham model for coning angle correction */
        if ((chisham) && (r>2137.5)) {
            gmma = acosd((rrad*rrad + *frho**frho - r*r )/(2.0*rrad**frho));
            beta = asind(rrad*sind(gmma/3.0)/(r/3.0));
            xel = 90.0 - beta - (gmma/3.0);
        } else {
            /* Elevation angle used for estimating off-array-normal azimuth */
            xel=rel;
        }

        /* Estimate the off-array-normal azimuth */
        if (((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel)))<0) tan_azi=1e32;
        else tan_azi=sqrt( (sind(psi)*sind(psi))/
                ((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel))));
        if (psi>0) azi=atand(tan_azi)*1.0;
        else azi=atand(tan_azi)*-1.0;

        /* Obtain the corresponding value of pointing azimuth */  
        xal=azi+bore;

        /* Adjust azimuth and elevation for the oblateness of the Earth */
        geocnvrt(gdlat,gdlon,xal,xel,&ral,&dum);

        /* Obtain the global spherical coordinates of the field point */
        fldpnt(rrho,rlat,rlon,ral,rel,r,frho,flat,flon);

        /* Recalculate the radius of the Earth beneath the field point (frad) */
        geodtgc(-1,&dum1,&dum2,&frad,flat,flon,&dum3);

        /* Check altitude */
        fhx=*frho-frad; 
    
    } while(fabs(fhx-xh) > 0.5);

} 



void fldpnth_gs(double gdlat,double gdlon,double psi,double bore,
                double fh,double r,double *frho,double *flat,
                double *flon) {

    double rrad,rlat,rlon,del;
    double tan_azi,azi,rel,xel,fhx,xal,rrho,ral,xh;
    double dum,dum1,dum2,dum3;
    double frad;  

    if (fh<=150) xh=fh;
    else {
        if (r<=300) xh=115;
        else if ((r>300) && (r<500)) xh=(r-300)/200*(fh-115)+115;
        else xh=fh;
    }

    if (r<150) xh=(r/150.0)*115.0;
    geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
    rrho=rrad;
    frad=rrad;

    do {
        *frho=frad+xh;

        rel=asind( ((*frho**frho) - (rrad*rrad) - (r*r)) / (2*rrad*r));
        xel=rel;
        if (((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel)))<0) tan_azi=1e32;
        else tan_azi=sqrt( (sind(psi)*sind(psi))/
                          ((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel))));
        if (psi>0) azi=atand(tan_azi)*1.0;
        else azi=atand(tan_azi)*-1.0;
        xal=azi+bore;
        geocnvrt(gdlat,gdlon,xal,xel,&ral,&dum);

        fldpnt(rrho,rlat,rlon,ral,rel,r,frho,flat,flon);
        geodtgc(-1,&dum1,&dum2,&frad,flat,flon,&dum3);
        fhx=*frho-frad; 
    } while(fabs(fhx-xh) > 0.5);
}



/**
 * This function converts a gate/beam coordinate to geographic
 * position. The height of the transformation is given by height -
 * if this value is less than 90 then it is assumed to be the
 * elevation angle from the radar. If center is not equal to zero, then
 * the calculation is assumed to be for the center of the cell, not the
 * edge. The calculated values are returned in geocentric spherical
 * coordinates (rho,lat,long).
 **/
void RPosGeo(int center, int bcrd, int rcrd,
             struct RadarSite *pos,
             int frang, int rsep,
             int rxrise, double height,
             double *rho, double *lat, double *lng,
             double *srng, int chisham) {

    double rx;
    double psi,d;
    double offset;
    double bm_edge=0;
    double range_edge=0;

    /* If not calculating center position of range-beam cell then calculate
     * position of near-left corner instead */
    if (center==0) {
        bm_edge=-pos->bmsep*0.5;
        range_edge=-0.5*rsep*20/3;
    }

    if (rxrise==0) rx=pos->recrise;
    else rx=rxrise;

    offset=pos->maxbeam/2.0-0.5;

    /* Calculate deviation from boresite [deg] */
    psi=pos->bmsep*(bcrd-offset)+bm_edge+pos->bmoff;

    /* Calculate the slant range to the range gate [km] */
    d=slant_range(frang,rsep,rx,range_edge,rcrd+1);
    *srng=d;

    /* If the input height is less than 90 then it is actually an input
     * elevation angle [deg], so we calculat the field point height */
    if (height < 90) height=-RE+sqrt((RE*RE)+2*d*RE*sind(height)+(d*d));

    /* Calculate the geocentric coordinates of the field point */
    fldpnth(pos->geolat,pos->geolon,psi,pos->boresite,
            height,d,rho,lat,lng,chisham);

}



void RPosMag(int center,int bcrd,int rcrd,
             struct RadarSite *pos,
             int frang,int rsep,int rxrise,double height,
             double *rho,double *lat,double *lng,
             double *srng, int chisham,int old_aacgm) {

    double rx;
    double radius;
    double psi,d;

    double bm_edge=0;
    double range_edge=0;
    double offset=0;

    if (center==0) {
        bm_edge=-pos->bmsep*0.5;
        range_edge=-0.5*rsep*20/3;
    }

    if (rxrise==0) rx=pos->recrise;
    else rx=rxrise;

    offset=pos->maxbeam/2.0-0.5;
    psi=pos->bmsep*(bcrd-offset)+bm_edge+pos->bmoff;
    d=slant_range(frang,rsep,rx,range_edge,rcrd+1);
    *srng=d;
    if (height < 90) height=-RE+sqrt((RE*RE)+2*d*RE*sind(height)+(d*d));

    fldpnth(pos->geolat,pos->geolon,psi,pos->boresite,
            height,d,rho,lat,lng,chisham);

    if (old_aacgm) AACGMConvert(*lat,*lng,(double) height,lat,lng,&radius,0);
    else           AACGM_v2_Convert(*lat,*lng,(double) height,lat,lng,&radius,0);

}



void RPosCubic(int center,int bcrd,int rcrd,
               struct RadarSite *pos,
               int frang,int rsep,int rxrise,double height,
               double *x,double *y,double *z) {

    /* returns cartesian cubic co-ordinates */
    double rx;
    double psi,d;
    double rho,lat,lng;
    double offset=0;
    double bm_edge=0;
    double range_edge=0;

    int chisham=0;

    if (center==0) {
        bm_edge=-pos->bmsep*0.5;
        range_edge=-0.5*rsep*20/3;
    }

    if (rxrise==0) rx=pos->recrise;
    else rx=rxrise;

    offset=pos->maxbeam/2.0-0.5;
    psi=pos->bmsep*(bcrd-offset)+bm_edge+pos->bmoff;

    d=slant_range(frang,rsep,rx,range_edge,rcrd+1);
    if (height < 90) height=-RE+sqrt((RE*RE)+2*d*RE*sind(height)+(d*d));
    fldpnth(pos->geolat,pos->geolon,psi,pos->boresite,
            height,d,&rho,&lat,&lng,chisham);

    /* convert to x,y,z (normalized to the unit sphere) */

    lng=90-lng;
    *x=rho*cos(lng*PI/180.0)*cos(lat*PI/180.0)/RE;
    *y=rho*sin(lat*PI/180.0)/RE;
    *z=rho*sin(lng*PI/180.0)*cos(lat*PI/180.0)/RE;

}



void RPosGeoGS(int center,int bcrd,int rcrd,
               struct RadarSite *pos,
               int frang,int rsep,int rxrise,double height,
               double *rho,double *lat,double *lng) {

    double rx;
    double psi,d;
    double offset=0;
    double bm_edge=0;
    double range_edge=0;

    if (center==0) {
        bm_edge=-pos->bmsep*0.5;
        range_edge=-0.5*rsep*20/3;
    }

    if (rxrise==0) rx=pos->recrise;
    else rx=rxrise;

    offset=pos->maxbeam/2.0-0.5;
    psi=pos->bmsep*(bcrd-offset)+bm_edge+pos->bmoff;

    d=slant_range(frang,rsep,rx,range_edge,rcrd+1)/2;
    if (height < 90) height=-RE+sqrt((RE*RE)+2*d*RE*sind(height)+(d*d));

    fldpnth_gs(pos->geolat,pos->geolon,psi,pos->boresite,
               height,d,rho,lat,lng);
}



void RPosMagGS(int center,int bcrd,int rcrd,
               struct RadarSite *pos,
               int frang,int rsep,int rxrise,double height,
               double *rho,double *lat,double *lng) {

    double rx;
    double radius;
    double psi,d;
    double offset=0;
    double bm_edge=0;
    double range_edge=0;

    if (center==0) {
        bm_edge=-pos->bmsep*0.5;
        range_edge=-0.5*rsep*20/3;
    }

    if (rxrise==0) rx=pos->recrise;
    else rx=rxrise;

    offset=pos->maxbeam/2.0-0.5;
    psi=pos->bmsep*(bcrd-offset)+bm_edge+pos->bmoff;

    d=slant_range(frang,rsep,rx,range_edge,rcrd+1)/2;
    if (height < 90) height=-RE+sqrt((RE*RE)+2*d*RE*sind(height)+(d*d));

    fldpnth_gs(pos->geolat,pos->geolon,psi,pos->boresite,
               height,d,rho,lat,lng);

    AACGMConvert(*lat,*lng,(double) height,lat,lng,&radius,0);

}



void RPosCubicGS(int center,int bcrd,int rcrd,
                 struct RadarSite *pos,
                 int frang,int rsep,int rxrise,double height,
                 double *x,double *y,double *z) {

    /* returns cartesian cubic co-ordinates */
    double rx;
    double psi,d; 
    double rho,lat,lng;
    double offset=0;
    double bm_edge=0;
    double range_edge=0;

    if (center==0) {
        bm_edge=-pos->bmsep*0.5;
        range_edge=-0.5*rsep*20/3;
    }

    if (rxrise==0) rx=pos->recrise;
    else rx=rxrise;

    offset=pos->maxbeam/2.0-0.5;
    psi=pos->bmsep*(bcrd-offset)+bm_edge+pos->bmoff;

    d=slant_range(frang,rsep,rx,range_edge,rcrd+1)/2;
    if (height < 90) height=-RE+sqrt((RE*RE)+2*d*RE*sind(height)+(d*d));
    fldpnth_gs(pos->geolat,pos->geolon,psi,pos->boresite,
               height,d,&rho,&lat,&lng);

    /* convert to x,y,z (normalized to the unit sphere) */

    lng=90-lng;
    *x=rho*cos(lng*PI/180.0)*cos(lat*PI/180.0)/RE;
    *y=rho*sin(lat*PI/180.0)/RE;
    *z=rho*sin(lng*PI/180.0)*cos(lat*PI/180.0)/RE;

}
