/* invmag.c
   ========
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rmath.h"
#include "aacgm.h"
#include "magcmp.h"
#include "radar.h"
#include "rpos.h"
#include "geodtgc.h"

#include "radar.h" 




void norm_vec(double *x,double *y,double *z) {

  double r;
  r=sqrt(*x**x+*y**y+*z**z);
  *x=*x/r;
  *y=*y/r;
  *z=*z/r;
}

void sphtocar(double r,double theta,double phi,double *x,double *y,double *z) {
  *x=r*sind(90.0-theta)*cosd(phi);
  *y=r*sind(90.0-theta)*sind(phi);
  *z=r*cosd(90.0-theta);
}


void fldpnt_sph(double frho,double flat,double flon,double az,
                double r,double *xlat,double *xlon) {
   
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


void fldpnt_azm(double mlat,double mlon,double nlat,double nlon,double *az) {
   
  double api;
  double aside,bside,cside;
  double Aangl,Bangl,arg;

  api=4*atan(1.0);
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
 
} 


void glbthor(int iopt,double lat,double lon,
             double *rx,double *ry,double *rz,
             double *tx,double *ty,double *tz) {
   double sx,sy,sz,lax;
   if (iopt>0) {
     sx=cosd(lon)**rx+sind(lon)**ry;
     sy=-sind(lon)**rx+cosd(lon)**ry;
     sz=*rz;
     lax=90-lat;
     *tx=cosd(lax)*sx-sind(lax)*sz;
     *ty=sy;
     *tz=sind(lax)*sx+cosd(lax)*sz;
   } else {
     lax=90-lat;
     sx=cosd(lax)**tx+sind(lax)**tz;
     sy=*ty;
     sz=-sind(lax)**tx+cosd(lax)**tz;
     *rx=cosd(lon)*sx-sind(lon)*sy;
     *ry=sind(lon)*sx+cosd(lon)*sy;
     *rz=sz;
   }

}


int RPosRngBmAzmElv(int bm,int rn,int year,
                     struct RadarSite *hdw,double frang,
                     double rsep,double rx,double height,
                     double *azm,double *elv) {

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
 
  gdlat=hdw->geolat;
  gdlon=hdw->geolon;

  if (rx==0) rx=hdw->recrise;

  RPosGeo(1,bm,rn,hdw,frang,rsep,rx,
             height,&frho,&flat,&flon);

  sphtocar(frho,flat,flon,&fx,&fy,&fz);       
  geodtgc(1,&gdlat,&gdlon,&gdrho,&glat,&glon,&dummy);
  sphtocar(gdrho,glat,glon,&gbx,&gby,&gbz);       
  gx=fx-gbx;
  gy=fy-gby;
  gz=fz-gbz;     
  norm_vec(&gx,&gy,&gz);
  glbthor(1,flat,flon,&gx,&gy,&gz,&ghx,&ghy,&ghz);
  norm_vec(&ghx,&ghy,&ghz);
            
  s=IGRFMagCmp(year,frho,flat,flon,&bx,&by,&bz,&b);
  if (s==-1) return -1;
  
  norm_vec(&bx,&by,&bz);
  ghz=-(bx*ghx+by*ghy)/bz;
  norm_vec(&ghx,&ghy,&ghz);
  *elv=atan2d(ghz,sqrt(ghx*ghx+ghy*ghy));
  *azm=atan2d(ghy,-ghx);
  return 0;
}





int RPosInvMag(int bm,int rn,int year,struct RadarSite *hdw,double frang,
             double rsep,double rx,double height,
             double *mlat,double *mlon,double *azm) {

  double flat,flon,frho;
  double fx,fy,fz;

  double gx,gy,gz;
  double glat,glon;
  double gdlat,gdlon,gdrho;
  double gbx,gby,gbz; 
  double ghx,ghy,ghz;
  double bx,by,bz,b;
  double dummy,elv,azc;

  double tmp_ht;
  double xlat,xlon,nlat,nlon;
  int s;

  gdlat=hdw->geolat;
  gdlon=hdw->geolon;

  if (rx==0) rx=hdw->recrise;

  RPosGeo(1,bm,rn,hdw,frang,rsep,rx,
             height,&frho,&flat,&flon);

  sphtocar(frho,flat,flon,&fx,&fy,&fz);       
  geodtgc(1,&gdlat,&gdlon,&gdrho,&glat,&glon,&dummy);
  sphtocar(gdrho,glat,glon,&gbx,&gby,&gbz);       
  gx=fx-gbx;
  gy=fy-gby;
  gz=fz-gbz;     
  norm_vec(&gx,&gy,&gz);
  glbthor(1,flat,flon,&gx,&gy,&gz,&ghx,&ghy,&ghz);
  norm_vec(&ghx,&ghy,&ghz);

  s=IGRFMagCmp(year,frho,flat,flon,&bx,&by,&bz,&b);
  if (s==-1) return -1;
  
  norm_vec(&bx,&by,&bz);
  ghz=-(bx*ghx+by*ghy)/bz;
  norm_vec(&ghx,&ghy,&ghz);
  elv=atan2d(ghz,sqrt(ghx*ghx+ghy*ghy));
  azc=atan2d(ghy,-ghx);
      
  geodtgc(-1,&gdlat,&gdlon,&gdrho,&flat,&flon,&dummy);
  tmp_ht=frho-gdrho;
   
  AACGMConvert(flat,flon,tmp_ht,mlat,mlon,&dummy,0);
        
  fldpnt_sph(frho,flat,flon,azc,rsep,&xlat,&xlon);
      
  s=AACGMConvert(xlat,xlon,tmp_ht,&nlat,&nlon,&dummy,0);
  if (s==-1) return -1;
       
  if ((nlon-*mlon) > 180)  nlon=nlon-360;
  if ((nlon-*mlon) < -180) nlon=nlon+360;

  fldpnt_azm(*mlat,*mlon,nlat,nlon,azm);      
  return 0;
}



