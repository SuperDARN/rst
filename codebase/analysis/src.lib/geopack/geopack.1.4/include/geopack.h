/* geopack.h
   ========== 
   Author R.J.Barnes
*/

/*
   See license.txt
*/

/* wrappers to the GEOPACK fortran routines. Note geopack is translated
   using f2c.
*/
 
int GeoPackRecalc(int yr,int mo,int dy,int hr,int mt,int sc);

int GeoPackIGRFGSM(double xgsm,double ygsm,double zgsm,
                   double *Hxgsm,double *Hygsm,double *Hzgsm);

int GeoPackIGRFGEO(double r,double theta,double phi,
                   double *br,double *btheta,double *bphi);

int GeoPackDip(double xgsm,double ygsm,double zgsm,
               double *Bxgsm,double *Bygsm,double *Bzgsm);

int GeoPackSun(int yr,int mo,int dy,int hr,int mt,int sc,
               double *gst,double *slong,double *srasn,double *sdec);

int GeoPackSphCar(double r,double theta,double phi,
                  double *x,double *y,double *z);

int GeoPackCarSph(double x,double y,double z,
                  double *r,double *theta,double *phi);

int GeoPackBSpCar(double theta,double phi,double br,double btheta,double bphi,
                  double *bx,double *by,double *bz);

int GeoPackBCarSp(double x,double y,double z,double bx,double by,double bz,
                  double *br,double *btheta,double *bphi);

int GeoPackRecalc(int yr,int mo,int dy,int hr,int mt,int sc);

int GeoPackGeoMag(double xgeo,double ygeo,double zgeo,
                  double *xmag,double *ymag,double *zmag);

int GeoPackMagGeo(double xmag,double ymag,double zmag,
                  double *xgeo,double *ygeo,double *zgeo);

int GeoPackGeiGeo(double xgei,double ygei,double zgei,
                  double *xgeo,double *ygeo,double *zgeo);

int GeoPackGeoGei(double xgeo,double ygeo,double zgeo,
                  double *xgei,double *ygei,double *zgei);

int GeoPackMagSm(double xmag,double ymag,double zmag,
		 double *xsm,double *ysm,double *zsm);

int GeoPackSmMag(double xsm,double ysm,double zsm,
                 double *xmag,double *ymag,double *zmag);

int GeoPackGsmGse(double xgsm,double ygsm,double zgsm,
                  double *xgse,double *ygse,double *zgse);

int GeoPackGseGsm(double xgse,double ygse,double zgse,
                  double *xgsm,double *ygsm,double *zgsm);

int GeoPackSmGsm(double xsm,double ysm,double zsm,
		 double *xgsm,double *ygsm,double *zgsm);

int GeoPackGsmSM(double xgsm,double ygsm,double zgsm,
		 double *xsm,double *ysm,double *zsm);

int GeoPackGeoGsm(double xgeo,double ygeo,double zgeo,
                  double *xgsm,double *ygsm,double *zgsm);

int GeoPackGsmGeo(double xgsm,double ygsm,double zgsm,
                  double *xgeo,double *ygeo,double *zgeo);



















