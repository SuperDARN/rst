/* geopack_wrap.c
   ============== 
   Author R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/
 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "f2c.h"
#include "rtime.h"

int GeoPackIGRFGSM(double xgsm,double ygsm,double zgsm,
                   double *Hxgsm,double *Hygsm,double *Hzgsm) {
  int s;

  real fxgsm,fygsm,fzgsm,fhxgsm,fhygsm,fhzgsm;

  extern int igrf_gsm__(real *xgsm, real *ygsm, real *zgsm, real *
	       hxgsm, real *hygsm, real *hzgsm);


  fxgsm=xgsm;
  fygsm=ygsm;
  fzgsm=zgsm;
  
  s=igrf_gsm__(&fxgsm,&fygsm,&fzgsm,&fhxgsm,&fhygsm,&fhzgsm);

  *Hxgsm=fhxgsm;
  *Hygsm=fhygsm;
  *Hzgsm=fhzgsm;

  return s;

}

int GeoPackIGRFGEO(double r,double theta,double phi,
                   double *br,double *btheta,double *bphi) {

  int s;
  real fr,ftheta,fphi,fbr,fbtheta,fbphi;
 
  extern int igrf_geo__(real *r__, real *theta, real *phi, real *br, 
	       real *btheta, real *bphi);

  fr=r;
  ftheta=theta;
  fphi=phi;

  s=igrf_geo__(&fr,&ftheta,&fphi,&fbr,&fbtheta,&fbphi);

  *br=fbr;
  *btheta=fbtheta;
  *bphi=fbphi;

  return s;
}


int GeoPackDip(double xgsm,double ygsm,double zgsm,
               double *Bxgsm,double *Bygsm,double *Bzgsm) {
  int s;

  real fxgsm,fygsm,fzgsm,fbxgsm,fbygsm,fbzgsm;
  

  extern int dip_(real *xgsm, real *ygsm, real *zgsm, real *bxgsm, 
	 real *bygsm, real *bzgsm);

  fxgsm=xgsm;
  fygsm=ygsm;
  fzgsm=zgsm;
  
  s=dip_(&fxgsm,&fygsm,&fzgsm,&fbxgsm,&fbygsm,&fbzgsm);

  *Bxgsm=fbxgsm;
  *Bygsm=fbygsm;
  *Bzgsm=fbzgsm;

  return s;

}


int GeoPackSun(int yr,int mo,int dy,int hr,int mt,int sc,
               double *gst,double *slong,double *srasn,double *sdec) {

  int s;
  integer fyr,fdy,fhr,fmt,fsc;
  real fgst,fslong,fsrasn,fsdec;

  extern int sun_(integer *iyear, integer *iday, integer *ihour, 
	integer *min__, integer *isec, real *gst, real *slong, real *srasn, 
	 real *sdec);


  fyr=yr;
  fdy=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc)/(24*3600);
  fhr=hr;
  fmt=mt;
  fsc=sc;

  s=sun_(&fyr,&fdy,&fhr,&fmt,&fsc,&fgst,&fslong,&fsrasn,&fsdec);

  *gst=fgst;
  *slong=fslong;
  *srasn=fsrasn;
  *sdec=fsdec;

  return s;
}

int GeoPackSphCar(double r,double theta,double phi,
                  double *x,double *y,double *z) {
  int s;
  real fr,ftheta,fphi,fx,fy,fz;
  integer fj=1;

  extern int sphcar_(real *r__, real *theta, real *phi, real *x, real 
	    *y, real *z__, integer *j);

  fr=r;
  ftheta=theta;
  fphi=phi;
 
  s=sphcar_(&fr,&ftheta,&fphi,&fx,&fy,&fz,&fj);
  *x=fx;
  *y=fy;
  *z=fz;
  return s;

}


int GeoPackCarSph(double x,double y,double z,
                  double *r,double *theta,double *phi) {

  int s;
  real fr,ftheta,fphi,fx,fy,fz;
  integer fj=-1;

  extern int sphcar_(real *r__, real *theta, real *phi, real *x, real 
	    *y, real *z__, integer *j);

  fx=x;
  fy=y;
  fz=z;
  
  s=sphcar_(&fr,&ftheta,&fphi,&fx,&fy,&fz,&fj);
  *r=fr;
  *theta=ftheta;
  *phi=fphi;
  return s;

}




int GeoPackBSpCar(double theta,double phi,double br,double btheta,double bphi,
                  double *bx,double *by,double *bz) {

  int s;
  real ftheta,fphi,fbr,fbtheta,fbphi,fbx,fby,fbz;

  extern int bspcar_(real *theta, real *phi, real *br, real *btheta, 
	    real *bphi, real *bx, real *by, real *bz);

  ftheta=theta;
  fphi=phi;
  fbr=br;
  fbtheta=btheta;
  fbphi=bphi;
  
  s=bspcar_(&ftheta,&fphi,&fbr,&fbtheta,&fbphi,&fbx,&fby,&fbz);
 
  *bx=fbx;
  *by=fby;
  *bz=fbz;
  return s;
}

int GeoPackBCarSp(double x,double y,double z,double bx,double by,double bz,
                  double *br,double *btheta,double *bphi) {

  int s;

  real fx,fy,fz,fbx,fby,fbz,fbr,fbtheta,fbphi;
  
  extern int bcarsp_(real *x, real *y, real *z__, real *bx, real *by, 
	    real *bz, real *br, real *btheta, real *bphi);

  fx=x;
  fy=y;
  fz=z;
  fbx=bx;
  fby=by;
  fbz=bz;
   
  s=bcarsp_(&fx,&fy,&fz,&fbx,&fby,&fbz,&fbr,&fbtheta,&fbphi); 

  *br=fbr;
  *btheta=fbtheta;
  *bphi=fbphi;

  return s;

}

int GeoPackRecalc(int yr,int mo,int dy,int hr,int mt,int sc) {

   integer fyr,fdy,fhr,fmt,fsc;

   extern int recalc_(integer *iyear, integer *iday, integer *ihour, 
	    integer *min__, integer *isec);

   fyr=yr;
   fdy=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc)/(24*3600)+1;
   fhr=hr;
   fmt=mt;
   fsc=sc;
   return recalc_(&fyr,&fdy,&fhr,&fmt,&fsc);
}

int GeoPackGeoMag(double xgeo,double ygeo,double zgeo,
                  double *xmag,double *ymag,double *zmag) {
  int s;
  integer fj=1;
  real fxgeo,fygeo,fzgeo,fxmag,fymag,fzmag;
  
  extern int geomag_(real *xgeo, real *ygeo, real *zgeo, real *xmag, 
	    real *ymag, real *zmag, integer *j);

  fxgeo=xgeo;
  fygeo=ygeo;
  fzgeo=zgeo;

  s=geomag_(&fxgeo,&fygeo,&fzgeo,&fxmag,&fymag,&fzmag,&fj);

  *xmag=fxmag;
  *ymag=fymag;
  *zmag=fzmag;

  return s;

}


int GeoPackMagGeo(double xmag,double ymag,double zmag,
                  double *xgeo,double *ygeo,double *zgeo) {
  int s;
  integer fj=-1;
  real fxgeo,fygeo,fzgeo,fxmag,fymag,fzmag;
  
  extern int geomag_(real *xgeo, real *ygeo, real *zgeo, real *xmag, 
	    real *ymag, real *zmag, integer *j);

  fxmag=xmag;
  fymag=ymag;
  fzmag=zmag;

  s=geomag_(&fxgeo,&fygeo,&fzgeo,&fxmag,&fymag,&fzmag,&fj);

  *xgeo=fxgeo;
  *ygeo=fygeo;
  *zgeo=fzgeo;

  return s;

}


int GeoPackGeiGeo(double xgei,double ygei,double zgei,
                  double *xgeo,double *ygeo,double *zgeo) {
  int s;
  integer fj=1;
  real fxgei,fygei,fzgei,fxgeo,fygeo,fzgeo;

  extern int geigeo_(real *xgei, real *ygei, real *zgei, real *xgeo, 
	    real *ygeo, real *zgeo, integer *j);

  fxgei=xgei;
  fygei=ygei;
  fzgei=zgei;
  
  s=geigeo_(&fxgei,&fygei,&fzgei,&fxgeo,&fygeo,&fzgeo,&fj);

  *xgeo=fxgeo;
  *ygeo=fygeo;
  *zgeo=fzgeo;

  return s;

}



int GeoPackGeoGei(double xgeo,double ygeo,double zgeo,
                  double *xgei,double *ygei,double *zgei) {
  int s;
  integer fj=-1;
  real fxgei,fygei,fzgei,fxgeo,fygeo,fzgeo;

  extern int geigeo_(real *xgei, real *ygei, real *zgei, real *xgeo, 
	    real *ygeo, real *zgeo, integer *j);

  fxgeo=xgeo;
  fygeo=ygeo;
  fzgeo=zgeo;
  
  s=geigeo_(&fxgei,&fygei,&fzgei,&fxgeo,&fygeo,&fzgeo,&fj);

  *xgei=fxgei;
  *ygei=fygei;
  *zgei=fzgei;

  return s;

}




int GeoPackMagSm(double xmag,double ymag,double zmag,
                  double *xsm,double *ysm,double *zsm) {

  int s;

  integer fj=1;
  real fxmag,fymag,fzmag,fxsm,fysm,fzsm;
  
  extern int magsm_(real *xmag, real *ymag, real *zmag, real *xsm, 
	   real *ysm, real *zsm, integer *j);


  fxmag=xmag;
  fymag=ymag;
  fzmag=zmag;

  s=magsm_(&fxmag,&fymag,&fzmag,&fxsm,&fysm,&fzsm,&fj);
 
  *xsm=fxsm;
  *ysm=fysm;
  *zsm=fzsm;

  return s;
}


int GeoPackSmMag(double xsm,double ysm,double zsm,
                 double *xmag,double *ymag,double *zmag) {

  int s;

  integer fj=-1;
  real fxmag,fymag,fzmag,fxsm,fysm,fzsm;
  
  extern int magsm_(real *xmag, real *ymag, real *zmag, real *xsm, 
	   real *ysm, real *zsm, integer *j);


  fxsm=xsm;
  fysm=ysm;
  fzsm=zsm;

  s=magsm_(&fxmag,&fymag,&fzmag,&fxsm,&fysm,&fzsm,&fj);
 
  *xmag=fxmag;
  *ymag=fymag;
  *zmag=fzmag;

  return s;
}





int GeoPackGsmGse(double xgsm,double ygsm,double zgsm,
                  double *xgse,double *ygse,double *zgse) {
  int s;
  integer fj=1;
  real fxgsm,fygsm,fzgsm,fxgse,fygse,fzgse;

  extern int gsmgse_(real *xgsm, real *ygsm, real *zgsm, real *xgse, 
	    real *ygse, real *zgse, integer *j);


  fxgsm=xgsm;
  fygsm=ygsm;
  fzgsm=zgsm;

  s=gsmgse_(&fxgsm,&fygsm,&fzgsm,&fxgse,&fygse,&fzgse,&fj);

  *xgse=fxgse;
  *ygse=fygse;
  *zgse=fzgse;

  return s;
}



int GeoPackGseGsm(double xgse,double ygse,double zgse,
                  double *xgsm,double *ygsm,double *zgsm) {
  int s;
  integer fj=-1;
  real fxgsm,fygsm,fzgsm,fxgse,fygse,fzgse;

  extern int gsmgse_(real *xgsm, real *ygsm, real *zgsm, real *xgse, 
	    real *ygse, real *zgse, integer *j);

  fxgse=xgse;
  fygse=ygse;
  fzgse=zgse;

  s=gsmgse_(&fxgsm,&fygsm,&fzgsm,&fxgse,&fygse,&fzgse,&fj);

  *xgsm=fxgsm;
  *ygsm=fygsm;
  *zgsm=fzgsm;

  return s;
}





int GeoPackSmGsm(double xsm,double ysm,double zsm,
                  double *xgsm,double *ygsm,double *zgsm) {
  int s;

  integer fj=1;
  real fxsm,fysm,fzsm,fxgsm,fygsm,fzgsm;

  extern int smgsm_(real *xsm, real *ysm, real *zsm, real *xgsm, real 
	   *ygsm, real *zgsm, integer *j);


  fxsm=xsm;
  fysm=ysm;
  fzsm=zsm;

  s=smgsm_(&fxsm,&fysm,&fzsm,&fxgsm,&fygsm,&fzgsm,&fj);

  *xgsm=fxgsm;
  *ygsm=fygsm;
  *zgsm=fzgsm;

  return s;
}

int GeoPackGsmSM(double xgsm,double ygsm,double zgsm,
                  double *xsm,double *ysm,double *zsm) {
  int s;

  integer fj=-1;
  real fxsm,fysm,fzsm,fxgsm,fygsm,fzgsm;

  extern int smgsm_(real *xsm, real *ysm, real *zsm, real *xgsm, real 
	   *ygsm, real *zgsm, integer *j);

  fxgsm=xgsm;
  fygsm=ygsm;
  fzgsm=zgsm;

  s=smgsm_(&fxsm,&fysm,&fzsm,&fxgsm,&fygsm,&fzgsm,&fj);

  *xsm=fxsm;
  *ysm=fysm;
  *zsm=fzsm;

  return s;
}






int GeoPackGeoGsm(double xgeo,double ygeo,double zgeo,
                  double *xgsm,double *ygsm,double *zgsm) {

  int s;
  integer fj=1;
  real fxgeo,fygeo,fzgeo,fxgsm,fygsm,fzgsm;

  extern int geogsm_(real *xgeo, real *ygeo, real *zgeo, real *xgsm, 
	    real *ygsm, real *zgsm, integer *j);

  fxgeo=xgeo;
  fygeo=ygeo;
  fzgeo=zgeo;

  s=geogsm_(&fxgeo,&fygeo,&fzgeo,&fxgsm,&fygsm,&fzgsm,&fj);

  *xgsm=fxgsm;
  *ygsm=fygsm;
  *zgsm=fzgsm;

  return s;
}



int GeoPackGsmGeo(double xgsm,double ygsm,double zgsm,
                  double *xgeo,double *ygeo,double *zgeo) {

  int s;
  integer fj=-1;
  real fxgeo,fygeo,fzgeo,fxgsm,fygsm,fzgsm;

  extern int geogsm_(real *xgeo, real *ygeo, real *zgeo, real *xgsm, 
	    real *ygsm, real *zgsm, integer *j);

  fxgsm=xgsm;
  fygsm=ygsm;
  fzgsm=zgsm;

  s=geogsm_(&fxgeo,&fygeo,&fzgeo,&fxgsm,&fygsm,&fzgsm,&fj);

  *xgeo=fxgeo;
  *ygeo=fygeo;
  *zgeo=fzgeo;

  return s;
}









/*
int rhand_(real *x, real *y, real *z__, real *r1, real *r2, 
	   real *r3, integer *iopt, real *parmod, S_fp exname, S_fp inname__);

int step_(real *x, real *y, real *z__, real *ds, real *errin,
	  integer *iopt, real *parmod, S_fp exname, S_fp inname__);


int trace_(real *xi, real *yi, real *zi, real *dir, real *
	rlim, real *r0, integer *iopt, real *parmod, S_fp exname, S_fp 
	inname__, integer *nobd, real *xf, real *yf, real *zf, real *xx, real 
	   *yy, real *zz, integer *l);

int shuetal_mgnp__(real *xn_pd__, real *vel, real *bzimf, 
	real *xgsm, real *ygsm, real *zgsm, real *xmgnp, real *ymgnp, real *
		   zmgnp, real *dist, integer *id);

int t96_mgnp__(real *xn_pd__, real *vel, real *xgsm, real *
	ygsm, real *zgsm, real *xmgnp, real *ymgnp, real *zmgnp, real *dist, 
				integer *id);
*/











