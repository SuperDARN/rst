/* geopack_2005.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include <stdlib.h>
#include "f2c.h"

/* Common Block Declarations */

struct {
    real g[105], h__[105], rec[105];
} geopack2_;

#define geopack2_1 geopack2_

union {
    struct {
	real aaa[10], sps, cps, bbb[23];
    } _1;
    struct {
	real st0, ct0, sl0, cl0, ctcl, stcl, ctsl, stsl, sfi, cfi, sps, cps, 
		shi, chi, hi, psi, xmut, a11, a21, a31, a12, a22, a32, a13, 
		a23, a33, ds3, cgst, sgst, ba[6];
    } _2;
    struct {
	real st0, ct0, sl0, cl0, ctcl, stcl, ctsl, stsl, ab[19], bb[8];
    } _3;
    struct {
	real a[27], cgst, sgst, b[6];
    } _4;
    struct {
	real a[8], sfi, cfi, b[7], ab[10], ba[8];
    } _5;
    struct {
	real a[12], shi, chi, ab[13], ba[8];
    } _6;
    struct {
	real a[10], sps, cps, b[15], ab[8];
    } _7;
    struct {
	real aa[17], a11, a21, a31, a12, a22, a32, a13, a23, a33, d__, b[8];
    } _8;
    struct {
	real a[15], psi, aa[10], ds3, bb[8];
    } _9;
    struct {
	real a[26], ds3, b[8];
    } _10;
    struct {
	real aa[26], dd, bb[8];
    } _11;
} geopack1_;

#define geopack1_1 (geopack1_._1)
#define geopack1_2 (geopack1_._2)
#define geopack1_3 (geopack1_._3)
#define geopack1_4 (geopack1_._4)
#define geopack1_5 (geopack1_._5)
#define geopack1_6 (geopack1_._6)
#define geopack1_7 (geopack1_._7)
#define geopack1_8 (geopack1_._8)
#define geopack1_9 (geopack1_._9)
#define geopack1_10 (geopack1_._10)
#define geopack1_11 (geopack1_._11)

/* Table of constant values */

static integer c_n1 = -1;
static integer c__1 = 1;
static doublereal c_b22 = 360.;
static doublereal c_b81 = -.15151515;
static real c_b82 = -1.f;
static integer c__9 = 9;
static doublereal c_b88 = .14;



/*          ########################################################################## */
/*          #                                                                        # */
/*          #                             GEOPACK-2005                               # */
/*          #                     (MAIN SET OF FORTRAN CODES)                        # */
/*          #                                                                        # */
/*          ########################################################################## */


/*  This collection of subroutines is a result of several upgrades of the original package */
/*  written by N. A. Tsyganenko in 1978-1979. This version is dated May 04, 2005. On that */
/*  date, the IGRF coefficients were updated according to the recently published table of */
/*  IGRF-10 coefficients, so that the main field model now extends through 2010 (a linear */
/*  extrapolation is used for 2005 - 2010, based on the table of secular velocities). For */
/*  more details, see */
/*  http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html <http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html> (revision of 0
3/22/2005). */


/*  Prefatory notes to the version of April 22, 2003: */

/*  This package represents an in-depth revision of the previous version, with significant */
/*  changes in the format of calling statements. Users should familiarize themselves with */
/*  the new formats and rules, and accordingly adjust their source codes, as specified */
/*  below. Please consult the documentation file */

/*  http://modelweb.gsfc.nasa.gov/magnetos/data-based/Geopack-2005.doc <http://modelweb.gsfc.nasa.gov/magnetos
/data-based/Geopack-2005.doc> for detailed */
/*  descriptions of individual subroutines. */

/*  The following changes were made to the previous release of GEOPACK (of Jan 5, 2001). */

/* (1) Subroutine IGRF, calculating the Earth's main field: */
/*   (a) Two versions of this subroutine are provided here. In the first one (IGRF_GSM) */
/*     both input (position) and output (field components) are in the Geocentric Solar- */
/*     Magnetospheric Cartesian coordinates, while the second one (IGRF_GEO) uses sphe- */
/*     rical geographical (geocentric) coordinates, as in the older releases. */
/*   (b) updating of all expansion coefficients is now made separately in the s/r RECALC, */
/*     which also takes into account the secular change of the coefficients within */
/*     a given year (at the Earth's surface, the rate of the change can reach 7 nT/month). */
/*   (c) the optimal length of spherical harmonic expansions is now automatically set */
/*     inside the code, based on the radial distance, so that the deviation from the */
/*     full-length approximation does not exceed 0.01 nT. (In the previous versions, */
/*     the upper limit NM of the order of harmonics had to be specified by users), */

/*  (2) Subroutine DIP, calculating the Earth's field in the dipole approximation: */
/*   (a) no longer accepts the tilt angle via the list of formal parameters. Instead, */
/*     the sine SPS and cosine CPS of that angle are now forwarded into DIP via the */
/*     first common block /GEOPACK1/.  Accordingly, there are two options: (i) to */
/*     calculate SPS and CPS by calling RECALC before calling DIP, or (ii) to specify */
/*     them explicitly. In the last case, SPS and CPS should be specified AFTER the */
/*     invocation of RECALC (otherwise they will be overridden by those returned by */
/*     RECALC). */
/*   (b) the Earth's dipole moment is now calculated by RECALC, based on the table of */
/*     the IGRF coefficients and their secular variation rates, for a given year and */
/*     the day of the year, and the obtained value of the moment is forwarded into DIP */
/*     via the second common block /GEOPACK2/. (In the previous versions, only a single */
/*     fixed value was provided for the geodipole moment, corresponding to the most */
/*     recent epoch). */

/*  (3) Subroutine RECALC now consolidates in one module all calculations needed to */
/*     initialize and update the values of coefficients and quantities that vary in */
/*     time, either due to secular changes of the main geomagnetic field or as a result */
/*     of Earth's diurnal rotation and orbital motion around Sun. That allowed us to */
/*     simplify the codes and make them more compiler-independent. */

/*  (4) Subroutine GEOMAG is now identical in its structure to other coordinate trans- */
/*     formation subroutines. It no longer invokes RECALC from within GEOMAG, but uses */
/*     precalculated values of the rotation matrix elements, obtained by a separate */
/*     external invocation of RECALC. This eliminates possible interference of the */
/*     two subroutines in the old version of the package. */

/*  (5) Subroutine TRACE (and the subsidiary modules STEP and RHAND): */

/*   (a) no longer needs to specify the highest order of spherical harmonics in the */
/*     main geomagnetic field expansion - it is now calculated automatically inside the */
/*     IGRF_GSM (or IGRF_GEO) subroutine. */

/*   (b) the internal field model can now be explicitly chosen by specifying the para- */
/*      meter INNAME (either IGRF_GSM or DIP). */

/*  (6) A new subroutine BCARSP was added, providing a conversion of Cartesian field */
/*     components into spherical ones (operation, inverse to that performed by the sub- */
/*     routine  BSPCAR). */

/*  (7) Two new subroutines were added, SHUETAL_MGNP and T96_MGNP, providing the position */
/*     of the magnetopause, according to the model of Shue et al. [1998] and the one */
/*     used in the T96 magnetospheric magnetic field model. */


/* ---------------------------------------------------------------------------------- */

/* Subroutine */ int igrf_gsm__(real *xgsm, real *ygsm, real *zgsm, real *
	hxgsm, real *hygsm, real *hzgsm)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real a[14], b[14], c__, d__, e;
    static integer k, m, n;
    static real p, q, r__, s, w, x, y, z__, d2, p2, bf, cf, bi, he, an, hh, 
	    dp, br, bt, sf;
    static integer mm, nm, mn;
    static real pm, pp, qq, xk, bbf, bbr, bbt, rho, rho2;
    static integer irp3;
    static real xgeo, ygeo, zgeo, hxgeo, hygeo, hzgeo;
    extern /* Subroutine */ int geogsm_(real *, real *, real *, real *, real *
	    , real *, integer *);


/*  CALCULATES COMPONENTS OF THE MAIN (INTERNAL) GEOMAGNETIC FIELD IN THE GEOCENTRIC SOLAR */
/*  MAGNETOSPHERIC COORDINATE SYSTEM, USING IAGA INTERNATIONAL GEOMAGNETIC REFERENCE MODEL */
/*  COEFFICIENTS (e.g., http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html Revised: 22 March, 2005) */


/*  BEFORE THE FIRST CALL OF THIS SUBROUTINE, OR IF THE DATE/TIME (IYEAR,IDAY,IHOUR,MIN,ISEC) */
/*  WAS CHANGED, THE MODEL COEFFICIENTS AND GEO-GSM ROTATION MATRIX ELEMENTS SHOULD BE UPDATED */
/*  BY CALLING THE SUBROUTINE RECALC */

/* -----INPUT PARAMETERS: */

/*     XGSM,YGSM,ZGSM - CARTESIAN GSM COORDINATES (IN UNITS RE=6371.2 KM) */

/* -----OUTPUT PARAMETERS: */

/*     HXGSM,HYGSM,HZGSM - CARTESIAN GSM COMPONENTS OF THE MAIN GEOMAGNETIC FIELD IN NANOTESLA */

/*     LAST MODIFICATION:  MAY 4, 2005. */
/*     THIS VERSION OF THE CODE ACCEPTS DATES FROM 1965 THROUGH 2010. */

/*     AUTHOR: N. A. TSYGANENKO */


    geogsm_(&xgeo, &ygeo, &zgeo, xgsm, ygsm, zgsm, &c_n1);
/* Computing 2nd power */
    r__1 = xgeo;
/* Computing 2nd power */
    r__2 = ygeo;
    rho2 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = zgeo;
    r__ = sqrt(rho2 + r__1 * r__1);
    c__ = zgeo / r__;
    rho = sqrt(rho2);
    s = rho / r__;
    if (s < 1e-5f) {
	cf = 1.f;
	sf = 0.f;
    } else {
	cf = xgeo / rho;
	sf = ygeo / rho;
    }

    pp = 1.f / r__;
    p = pp;

/*  IN THIS NEW VERSION, THE OPTIMAL VALUE OF THE PARAMETER NM (MAXIMAL ORDER OF THE SPHERICAL */
/*    HARMONIC EXPANSION) IS NOT USER-PRESCRIBED, BUT CALCULATED INSIDE THE SUBROUTINE, BASED */
/*      ON THE VALUE OF THE RADIAL DISTANCE R: */

    irp3 = r__ + 2;
    nm = 30 / irp3 + 3;
    if (nm > 13) {
	nm = 13;
    }
    k = nm + 1;
    i__1 = k;
    for (n = 1; n <= i__1; ++n) {
	p *= pp;
	a[n - 1] = p;
/* L150: */
	b[n - 1] = p * n;
    }
    p = 1.f;
    d__ = 0.f;
    bbr = 0.f;
    bbt = 0.f;
    bbf = 0.f;
    i__1 = k;
    for (m = 1; m <= i__1; ++m) {
	if (m == 1) {
	    goto L160;
	}
	mm = m - 1;
	w = x;
	x = w * cf + y * sf;
	y = y * cf - w * sf;
	goto L170;
L160:
	x = 0.f;
	y = 1.f;
L170:
	q = p;
	z__ = d__;
	bi = 0.f;
	p2 = 0.f;
	d2 = 0.f;
	i__2 = k;
	for (n = m; n <= i__2; ++n) {
	    an = a[n - 1];
	    mn = n * (n - 1) / 2 + m;
	    e = geopack2_1.g[mn - 1];
	    hh = geopack2_1.h__[mn - 1];
	    w = e * y + hh * x;
	    bbr += b[n - 1] * w * q;
	    bbt -= an * w * z__;
	    if (m == 1) {
		goto L180;
	    }
	    qq = q;
	    if (s < 1e-5f) {
		qq = z__;
	    }
	    bi += an * (e * x - hh * y) * qq;
L180:
	    xk = geopack2_1.rec[mn - 1];
	    dp = c__ * z__ - s * q - xk * d2;
	    pm = c__ * q - xk * p2;
	    d2 = z__;
	    p2 = q;
	    z__ = dp;
/* L190: */
	    q = pm;
	}
	d__ = s * d__ + c__ * p;
	p = s * p;
	if (m == 1) {
	    goto L200;
	}
	bi *= mm;
	bbf += bi;
L200:
	;
    }

    br = bbr;
    bt = bbt;
    if (s < 1e-5f) {
	goto L210;
    }
    bf = bbf / s;
    goto L211;
L210:
    if (c__ < 0.f) {
	bbf = -bbf;
    }
    bf = bbf;
L211:
    he = br * s + bt * c__;
    hxgeo = he * cf - bf * sf;
    hygeo = he * sf + bf * cf;
    hzgeo = br * c__ - bt * s;
    geogsm_(&hxgeo, &hygeo, &hzgeo, hxgsm, hygsm, hzgsm, &c__1);
    return 0;
} /* igrf_gsm__ */


/* ========================================================================================== */


/* Subroutine */ int igrf_geo__(real *r__, real *theta, real *phi, real *br, 
	real *btheta, real *bphi)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double cos(doublereal), sin(doublereal);

    /* Local variables */
    static real a[14], b[14], c__, d__, e;
    static integer k, m, n;
    static real p, q, s, w, x, y, z__, d2, p2, cf, bi, an, hh, dp, sf;
    static integer mm, nm, mn;
    static real pm, pp, qq, xk, bbf, bbr, bbt;
    static integer irp3;


/*  CALCULATES COMPONENTS OF THE MAIN (INTERNAL) GEOMAGNETIC FIELD IN THE SPHERICAL GEOGRAPHIC */
/*  (GEOCENTRIC) COORDINATE SYSTEM, USING IAGA INTERNATIONAL GEOMAGNETIC REFERENCE MODEL */
/*  COEFFICIENTS  (e.g., http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html, revised: 22 March, 2005) */

/*  BEFORE THE FIRST CALL OF THIS SUBROUTINE, OR IF THE DATE (IYEAR AND IDAY) WAS CHANGED, */
/*  THE MODEL COEFFICIENTS SHOULD BE UPDATED BY CALLING THE SUBROUTINE RECALC */

/* -----INPUT PARAMETERS: */

/*   R, THETA, PHI - SPHERICAL GEOGRAPHIC (GEOCENTRIC) COORDINATES: */
/*   RADIAL DISTANCE R IN UNITS RE=6371.2 KM, COLATITUDE THETA AND LONGITUDE PHI IN RADIANS */

/* -----OUTPUT PARAMETERS: */

/*     BR, BTHETA, BPHI - SPHERICAL COMPONENTS OF THE MAIN GEOMAGNETIC FIELD IN NANOTESLA */
/*      (POSITIVE BR OUTWARD, BTHETA SOUTHWARD, BPHI EASTWARD) */

/*     LAST MODIFICATION:  MAY 4, 2005. */
/*     THIS VERSION OF THE  CODE ACCEPTS DATES FROM 1965 THROUGH 2010. */

/*     AUTHOR: N. A. TSYGANENKO */


    c__ = cos(*theta);
    s = sin(*theta);
    cf = cos(*phi);
    sf = sin(*phi);

    pp = 1.f / *r__;
    p = pp;

/*  IN THIS NEW VERSION, THE OPTIMAL VALUE OF THE PARAMETER NM (MAXIMAL ORDER OF THE SPHERICAL */
/*    HARMONIC EXPANSION) IS NOT USER-PRESCRIBED, BUT CALCULATED INSIDE THE SUBROUTINE, BASED */
/*      ON THE VALUE OF THE RADIAL DISTANCE R: */

    irp3 = *r__ + 2;
    nm = 30 / irp3 + 3;
    if (nm > 13) {
	nm = 13;
    }
    k = nm + 1;
    i__1 = k;
    for (n = 1; n <= i__1; ++n) {
	p *= pp;
	a[n - 1] = p;
/* L150: */
	b[n - 1] = p * n;
    }
    p = 1.f;
    d__ = 0.f;
    bbr = 0.f;
    bbt = 0.f;
    bbf = 0.f;
    i__1 = k;
    for (m = 1; m <= i__1; ++m) {
	if (m == 1) {
	    goto L160;
	}
	mm = m - 1;
	w = x;
	x = w * cf + y * sf;
	y = y * cf - w * sf;
	goto L170;
L160:
	x = 0.f;
	y = 1.f;
L170:
	q = p;
	z__ = d__;
	bi = 0.f;
	p2 = 0.f;
	d2 = 0.f;
	i__2 = k;
	for (n = m; n <= i__2; ++n) {
	    an = a[n - 1];
	    mn = n * (n - 1) / 2 + m;
	    e = geopack2_1.g[mn - 1];
	    hh = geopack2_1.h__[mn - 1];
	    w = e * y + hh * x;
	    bbr += b[n - 1] * w * q;
	    bbt -= an * w * z__;
	    if (m == 1) {
		goto L180;
	    }
	    qq = q;
	    if (s < 1e-5f) {
		qq = z__;
	    }
	    bi += an * (e * x - hh * y) * qq;
L180:
	    xk = geopack2_1.rec[mn - 1];
	    dp = c__ * z__ - s * q - xk * d2;
	    pm = c__ * q - xk * p2;
	    d2 = z__;
	    p2 = q;
	    z__ = dp;
/* L190: */
	    q = pm;
	}
	d__ = s * d__ + c__ * p;
	p = s * p;
	if (m == 1) {
	    goto L200;
	}
	bi *= mm;
	bbf += bi;
L200:
	;
    }

    *br = bbr;
    *btheta = bbt;
    if (s < 1e-5f) {
	goto L210;
    }
    *bphi = bbf / s;
    return 0;
L210:
    if (c__ < 0.f) {
	bbf = -bbf;
    }
    *bphi = bbf;
    return 0;
} /* igrf_geo__ */


/* ========================================================================================== */

/* Subroutine */ int dip_(real *xgsm, real *ygsm, real *zgsm, real *bxgsm, 
	real *bygsm, real *bzgsm)
{
    /* System generated locals */
    real r__1, r__2, r__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real p, q, t, u, v, dipmom;


/*  CALCULATES GSM COMPONENTS OF A GEODIPOLE FIELD WITH THE DIPOLE MOMENT */
/*  CORRESPONDING TO THE EPOCH, SPECIFIED BY CALLING SUBROUTINE RECALC (SHOULD BE */
/*  INVOKED BEFORE THE FIRST USE OF THIS ONE AND IN CASE THE DATE/TIME WAS CHANGED). */

/* --INPUT PARAMETERS: XGSM,YGSM,ZGSM - GSM COORDINATES IN RE (1 RE = 6371.2 km) */

/* --OUTPUT PARAMETERS: BXGSM,BYGSM,BZGSM - FIELD COMPONENTS IN GSM SYSTEM, IN NANOTESLA. */

/*  LAST MODIFICATION: MAY 4, 2005 */

/*  AUTHOR: N. A. TSYGANENKO */

/* Computing 2nd power */
    r__1 = geopack2_1.g[1];
/* Computing 2nd power */
    r__2 = geopack2_1.g[2];
/* Computing 2nd power */
    r__3 = geopack2_1.h__[2];
    dipmom = sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);
/* Computing 2nd power */
    r__1 = *xgsm;
    p = r__1 * r__1;
/* Computing 2nd power */
    r__1 = *zgsm;
    u = r__1 * r__1;
    v = *zgsm * 3.f * *xgsm;
/* Computing 2nd power */
    r__1 = *ygsm;
    t = r__1 * r__1;
/* Computing 5th power */
    r__1 = sqrt(p + t + u), r__2 = r__1, r__1 *= r__1;
    q = dipmom / (r__2 * (r__1 * r__1));
    *bxgsm = q * ((t + u - p * 2.f) * geopack1_1.sps - v * geopack1_1.cps);
    *bygsm = *ygsm * -3.f * q * (*xgsm * geopack1_1.sps + *zgsm * 
	    geopack1_1.cps);
    *bzgsm = q * ((p + t - u * 2.f) * geopack1_1.cps - v * geopack1_1.sps);
    return 0;
} /* dip_ */

/* ******************************************************************* */

/* Subroutine */ int sun_(integer *iyear, integer *iday, integer *ihour, 
	integer *min__, integer *isec, real *gst, real *slong, real *srasn, 
	real *sdec)
{
    /* Initialized data */

    static real rad = 57.295779513f;

    /* System generated locals */
    real r__1;
    doublereal d__1;

    /* Builtin functions */
    double f2c_d_mod(doublereal *, doublereal *), sin(doublereal), sqrt(
	    doublereal), atan(doublereal), cos(doublereal), atan2(doublereal, 
	    doublereal);

    /* Local variables */
    static real g, t;
    static doublereal dj;
    static real sc, vl, sob, slp;
    static doublereal fday;
    static real cosd, sind, obliq;


/*  CALCULATES FOUR QUANTITIES NECESSARY FOR COORDINATE TRANSFORMATIONS */
/*  WHICH DEPEND ON SUN POSITION (AND, HENCE, ON UNIVERSAL TIME AND SEASON) */

/* -------  INPUT PARAMETERS: */
/*  IYR,IDAY,IHOUR,MIN,ISEC -  YEAR, DAY, AND UNIVERSAL TIME IN HOURS, MINUTES, */
/*    AND SECONDS  (IDAY=1 CORRESPONDS TO JANUARY 1). */

/* -------  OUTPUT PARAMETERS: */
/*  GST - GREENWICH MEAN SIDEREAL TIME, SLONG - LONGITUDE ALONG ECLIPTIC */
/*  SRASN - RIGHT ASCENSION,  SDEC - DECLINATION  OF THE SUN (RADIANS) */
/*  ORIGINAL VERSION OF THIS SUBROUTINE HAS BEEN COMPILED FROM: */
/*  RUSSELL, C.T., COSMIC ELECTRODYNAMICS, 1971, V.2, PP.184-196. */

/*  LAST MODIFICATION:  MARCH 31, 2003 (ONLY SOME NOTATION CHANGES) */

/*     ORIGINAL VERSION WRITTEN BY:    Gilbert D. Mead */


    if (*iyear < 1901 || *iyear > 2099) {
	return 0;
    }
    fday = (doublereal) (*ihour * 3600 + *min__ * 60 + *isec) / 86400.;
    dj = (*iyear - 1900) * 365 + (*iyear - 1901) / 4 + *iday - .5 + fday;
    t = dj / 36525.f;
    d__1 = dj * .9856473354f + 279.696678f;
    vl = f2c_d_mod(&d__1, &c_b22);
    d__1 = dj * .9856473354f + 279.690983f + fday * 360.f + 180.f;
    *gst = f2c_d_mod(&d__1, &c_b22) / rad;
    d__1 = dj * .985600267f + 358.475845f;
    g = f2c_d_mod(&d__1, &c_b22) / rad;
    *slong = (vl + (1.91946f - t * .004789f) * sin(g) + sin(g * 2.f) * 
	    .020094f) / rad;
    if (*slong > 6.2831853f) {
	*slong += -6.2831853f;
    }
    if (*slong < 0.f) {
	*slong += 6.2831853f;
    }
    obliq = (23.45229f - t * .0130125f) / rad;
    sob = sin(obliq);
    slp = *slong - 9.924e-5f;

/*   THE LAST CONSTANT IS A CORRECTION FOR THE ANGULAR ABERRATION  DUE TO */
/*   THE ORBITAL MOTION OF THE EARTH */

    sind = sob * sin(slp);
/* Computing 2nd power */
    r__1 = sind;
    cosd = sqrt(1.f - r__1 * r__1);
    sc = sind / cosd;
    *sdec = atan(sc);
    *srasn = 3.141592654f - atan2(cos(obliq) / sob * sc, -cos(slp) / cosd);
    return 0;
} /* sun_ */


/* ================================================================================ */

/* Subroutine */ int sphcar_(real *r__, real *theta, real *phi, real *x, real 
	*y, real *z__, integer *j)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal), atan2(doublereal, doublereal), sin(doublereal), 
	    cos(doublereal);

    /* Local variables */
    static real sq;


/*   CONVERTS SPHERICAL COORDS INTO CARTESIAN ONES AND VICA VERSA */
/*    (THETA AND PHI IN RADIANS). */

/*                  J>0            J<0 */
/* -----INPUT:   J,R,THETA,PHI     J,X,Y,Z */
/* ----OUTPUT:      X,Y,Z        R,THETA,PHI */

/*  NOTE: AT THE POLES (X=0 AND Y=0) WE ASSUME PHI=0 (WHEN CONVERTING */
/*        FROM CARTESIAN TO SPHERICAL COORDS, I.E., FOR J<0) */

/*   LAST MOFIFICATION:  APRIL 1, 2003 (ONLY SOME NOTATION CHANGES AND MORE */
/*                         COMMENTS ADDED) */

/*   AUTHOR:  N. A. TSYGANENKO */

    if (*j > 0) {
	goto L3;
    }
/* Computing 2nd power */
    r__1 = *x;
/* Computing 2nd power */
    r__2 = *y;
    sq = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = *z__;
    *r__ = sqrt(sq + r__1 * r__1);
    if (sq != 0.f) {
	goto L2;
    }
    *phi = 0.f;
    if (*z__ < 0.f) {
	goto L1;
    }
    *theta = 0.f;
    return 0;
L1:
    *theta = 3.141592654f;
    return 0;
L2:
    sq = sqrt(sq);
    *phi = atan2(*y, *x);
    *theta = atan2(sq, *z__);
    if (*phi < 0.f) {
	*phi += 6.28318531f;
    }
    return 0;
L3:
    sq = *r__ * sin(*theta);
    *x = sq * cos(*phi);
    *y = sq * sin(*phi);
    *z__ = *r__ * cos(*theta);
    return 0;
} /* sphcar_ */


/* =========================================================================== */

/* Subroutine */ int bspcar_(real *theta, real *phi, real *br, real *btheta, 
	real *bphi, real *bx, real *by, real *bz)
{
    /* Builtin functions */
    double sin(doublereal), cos(doublereal);

    /* Local variables */
    static real c__, s, be, cf, sf;


/*   CALCULATES CARTESIAN FIELD COMPONENTS FROM SPHERICAL ONES */
/* -----INPUT:   THETA,PHI - SPHERICAL ANGLES OF THE POINT IN RADIANS */
/*              BR,BTHETA,BPHI -  SPHERICAL COMPONENTS OF THE FIELD */
/* -----OUTPUT:  BX,BY,BZ - CARTESIAN COMPONENTS OF THE FIELD */

/*   LAST MOFIFICATION:  APRIL 1, 2003 (ONLY SOME NOTATION CHANGES) */

/*   WRITTEN BY:  N. A. TSYGANENKO */

    s = sin(*theta);
    c__ = cos(*theta);
    sf = sin(*phi);
    cf = cos(*phi);
    be = *br * s + *btheta * c__;
    *bx = be * cf - *bphi * sf;
    *by = be * sf + *bphi * cf;
    *bz = *br * c__ - *btheta * s;
    return 0;
} /* bspcar_ */


/* ============================================================================== */

/* Subroutine */ int bcarsp_(real *x, real *y, real *z__, real *bx, real *by, 
	real *bz, real *br, real *btheta, real *bphi)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real r__, ct, st, rho, rho2, cphi, sphi;


/* ALCULATES SPHERICAL FIELD COMPONENTS FROM THOSE IN CARTESIAN SYSTEM */

/* -----INPUT:   X,Y,Z  - CARTESIAN COMPONENTS OF THE POSITION VECTOR */
/*              BX,BY,BZ - CARTESIAN COMPONENTS OF THE FIELD VECTOR */
/* -----OUTPUT:  BR,BTHETA,BPHI - SPHERICAL COMPONENTS OF THE FIELD VECTOR */

/*  NOTE: AT THE POLES (THETA=0 OR THETA=PI) WE ASSUME PHI=0, */
/*        AND HENCE BTHETA=BX, BPHI=BY */

/*   WRITTEN AND ADDED TO THIS PACKAGE:  APRIL 1, 2003, */
/*   AUTHOR:   N. A. TSYGANENKO */

/* Computing 2nd power */
    r__1 = *x;
/* Computing 2nd power */
    r__2 = *y;
    rho2 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = *z__;
    r__ = sqrt(rho2 + r__1 * r__1);
    rho = sqrt(rho2);
    if (rho != 0.f) {
	cphi = *x / rho;
	sphi = *y / rho;
    } else {
	cphi = 1.f;
	sphi = 0.f;
    }
    ct = *z__ / r__;
    st = rho / r__;
    *br = (*x * *bx + *y * *by + *z__ * *bz) / r__;
    *btheta = (*bx * cphi + *by * sphi) * ct - *bz * st;
    *bphi = *by * cphi - *bx * sphi;
    return 0;
} /* bcarsp_ */


/* ===================================================================================== */

/* Subroutine */ int recalc_(integer *iyear, integer *iday, integer *ihour, 
	integer *min__, integer *isec)
{
    /* Initialized data */

    static real g65[105] = { 0.f,-30334.f,-2119.f,-1662.f,2997.f,1594.f,
	    1297.f,-2038.f,1292.f,856.f,957.f,804.f,479.f,-390.f,252.f,-219.f,
	    358.f,254.f,-31.f,-157.f,-62.f,45.f,61.f,8.f,-228.f,4.f,1.f,
	    -111.f,75.f,-57.f,4.f,13.f,-26.f,-6.f,13.f,1.f,13.f,5.f,-4.f,
	    -14.f,0.f,8.f,-1.f,11.f,4.f,8.f,10.f,2.f,-13.f,10.f,-1.f,-1.f,5.f,
	    1.f,-2.f,-2.f,-3.f,2.f,-5.f,-2.f,4.f,4.f,0.f,2.f,2.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f };
    static real h65[105] = { 0.f,0.f,5776.f,0.f,-2016.f,114.f,0.f,-404.f,
	    240.f,-165.f,0.f,148.f,-269.f,13.f,-269.f,0.f,19.f,128.f,-126.f,
	    -97.f,81.f,0.f,-11.f,100.f,68.f,-32.f,-8.f,-7.f,0.f,-61.f,-27.f,
	    -2.f,6.f,26.f,-23.f,-12.f,0.f,7.f,-12.f,9.f,-16.f,4.f,24.f,-3.f,
	    -17.f,0.f,-22.f,15.f,7.f,-4.f,-5.f,10.f,10.f,-4.f,1.f,0.f,2.f,1.f,
	    2.f,6.f,-4.f,0.f,-2.f,3.f,0.f,-6.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static real g70[105] = { 0.f,-30220.f,-2068.f,-1781.f,3e3f,1611.f,1287.f,
	    -2091.f,1278.f,838.f,952.f,800.f,461.f,-395.f,234.f,-216.f,359.f,
	    262.f,-42.f,-160.f,-56.f,43.f,64.f,15.f,-212.f,2.f,3.f,-112.f,
	    72.f,-57.f,1.f,14.f,-22.f,-2.f,13.f,-2.f,14.f,6.f,-2.f,-13.f,-3.f,
	    5.f,0.f,11.f,3.f,8.f,10.f,2.f,-12.f,10.f,-1.f,0.f,3.f,1.f,-1.f,
	    -3.f,-3.f,2.f,-5.f,-1.f,6.f,4.f,1.f,0.f,3.f,-1.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f };
    static real h70[105] = { 0.f,0.f,5737.f,0.f,-2047.f,25.f,0.f,-366.f,251.f,
	    -196.f,0.f,167.f,-266.f,26.f,-279.f,0.f,26.f,139.f,-139.f,-91.f,
	    83.f,0.f,-12.f,100.f,72.f,-37.f,-6.f,1.f,0.f,-70.f,-27.f,-4.f,8.f,
	    23.f,-23.f,-11.f,0.f,7.f,-15.f,6.f,-17.f,6.f,21.f,-6.f,-16.f,0.f,
	    -21.f,16.f,6.f,-4.f,-5.f,10.f,11.f,-2.f,1.f,0.f,1.f,1.f,3.f,4.f,
	    -4.f,0.f,-1.f,3.f,1.f,-4.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static real g75[105] = { 0.f,-30100.f,-2013.f,-1902.f,3010.f,1632.f,
	    1276.f,-2144.f,1260.f,830.f,946.f,791.f,438.f,-405.f,216.f,-218.f,
	    356.f,264.f,-59.f,-159.f,-49.f,45.f,66.f,28.f,-198.f,1.f,6.f,
	    -111.f,71.f,-56.f,1.f,16.f,-14.f,0.f,12.f,-5.f,14.f,6.f,-1.f,
	    -12.f,-8.f,4.f,0.f,10.f,1.f,7.f,10.f,2.f,-12.f,10.f,-1.f,-1.f,4.f,
	    1.f,-2.f,-3.f,-3.f,2.f,-5.f,-2.f,5.f,4.f,1.f,0.f,3.f,-1.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f };
    static real h75[105] = { 0.f,0.f,5675.f,0.f,-2067.f,-68.f,0.f,-333.f,
	    262.f,-223.f,0.f,191.f,-265.f,39.f,-288.f,0.f,31.f,148.f,-152.f,
	    -83.f,88.f,0.f,-13.f,99.f,75.f,-41.f,-4.f,11.f,0.f,-77.f,-26.f,
	    -5.f,10.f,22.f,-23.f,-12.f,0.f,6.f,-16.f,4.f,-19.f,6.f,18.f,-10.f,
	    -17.f,0.f,-21.f,16.f,7.f,-4.f,-5.f,10.f,11.f,-3.f,1.f,0.f,1.f,1.f,
	    3.f,4.f,-4.f,-1.f,-1.f,3.f,1.f,-5.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static real g80[105] = { 0.f,-29992.f,-1956.f,-1997.f,3027.f,1663.f,
	    1281.f,-2180.f,1251.f,833.f,938.f,782.f,398.f,-419.f,199.f,-218.f,
	    357.f,261.f,-74.f,-162.f,-48.f,48.f,66.f,42.f,-192.f,4.f,14.f,
	    -108.f,72.f,-59.f,2.f,21.f,-12.f,1.f,11.f,-2.f,18.f,6.f,0.f,-11.f,
	    -7.f,4.f,3.f,6.f,-1.f,5.f,10.f,1.f,-12.f,9.f,-3.f,-1.f,7.f,2.f,
	    -5.f,-4.f,-4.f,2.f,-5.f,-2.f,5.f,3.f,1.f,2.f,3.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f };
    static real h80[105] = { 0.f,0.f,5604.f,0.f,-2129.f,-200.f,0.f,-336.f,
	    271.f,-252.f,0.f,212.f,-257.f,53.f,-297.f,0.f,46.f,150.f,-151.f,
	    -78.f,92.f,0.f,-15.f,93.f,71.f,-43.f,-2.f,17.f,0.f,-82.f,-27.f,
	    -5.f,16.f,18.f,-23.f,-10.f,0.f,7.f,-18.f,4.f,-22.f,9.f,16.f,-13.f,
	    -15.f,0.f,-21.f,16.f,9.f,-5.f,-6.f,9.f,10.f,-6.f,2.f,0.f,1.f,0.f,
	    3.f,6.f,-4.f,0.f,-1.f,4.f,0.f,-6.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static real g85[105] = { 0.f,-29873.f,-1905.f,-2072.f,3044.f,1687.f,
	    1296.f,-2208.f,1247.f,829.f,936.f,780.f,361.f,-424.f,170.f,-214.f,
	    355.f,253.f,-93.f,-164.f,-46.f,53.f,65.f,51.f,-185.f,4.f,16.f,
	    -102.f,74.f,-62.f,3.f,24.f,-6.f,4.f,10.f,0.f,21.f,6.f,0.f,-11.f,
	    -9.f,4.f,4.f,4.f,-4.f,5.f,10.f,1.f,-12.f,9.f,-3.f,-1.f,7.f,1.f,
	    -5.f,-4.f,-4.f,3.f,-5.f,-2.f,5.f,3.f,1.f,2.f,3.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f };
    static real h85[105] = { 0.f,0.f,5500.f,0.f,-2197.f,-306.f,0.f,-310.f,
	    284.f,-297.f,0.f,232.f,-249.f,69.f,-297.f,0.f,47.f,150.f,-154.f,
	    -75.f,95.f,0.f,-16.f,88.f,69.f,-48.f,-1.f,21.f,0.f,-83.f,-27.f,
	    -2.f,20.f,17.f,-23.f,-7.f,0.f,8.f,-19.f,5.f,-23.f,11.f,14.f,-15.f,
	    -11.f,0.f,-21.f,15.f,9.f,-6.f,-6.f,9.f,9.f,-7.f,2.f,0.f,1.f,0.f,
	    3.f,6.f,-4.f,0.f,-1.f,4.f,0.f,-6.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static real g90[105] = { 0.f,-29775.f,-1848.f,-2131.f,3059.f,1686.f,
	    1314.f,-2239.f,1248.f,802.f,939.f,780.f,325.f,-423.f,141.f,-214.f,
	    353.f,245.f,-109.f,-165.f,-36.f,61.f,65.f,59.f,-178.f,3.f,18.f,
	    -96.f,77.f,-64.f,2.f,26.f,-1.f,5.f,9.f,0.f,23.f,5.f,-1.f,-10.f,
	    -12.f,3.f,4.f,2.f,-6.f,4.f,9.f,1.f,-12.f,9.f,-4.f,-2.f,7.f,1.f,
	    -6.f,-3.f,-4.f,2.f,-5.f,-2.f,4.f,3.f,1.f,3.f,3.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f };
    static real h90[105] = { 0.f,0.f,5406.f,0.f,-2279.f,-373.f,0.f,-284.f,
	    293.f,-352.f,0.f,247.f,-240.f,84.f,-299.f,0.f,46.f,154.f,-153.f,
	    -69.f,97.f,0.f,-16.f,82.f,69.f,-52.f,1.f,24.f,0.f,-80.f,-26.f,0.f,
	    21.f,17.f,-23.f,-4.f,0.f,10.f,-19.f,6.f,-22.f,12.f,12.f,-16.f,
	    -10.f,0.f,-20.f,15.f,11.f,-7.f,-7.f,9.f,8.f,-7.f,2.f,0.f,2.f,1.f,
	    3.f,6.f,-4.f,0.f,-2.f,3.f,-1.f,-6.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static real g95[105] = { 0.f,-29692.f,-1784.f,-2200.f,3070.f,1681.f,
	    1335.f,-2267.f,1249.f,759.f,940.f,780.f,290.f,-418.f,122.f,-214.f,
	    352.f,235.f,-118.f,-166.f,-17.f,68.f,67.f,68.f,-170.f,-1.f,19.f,
	    -93.f,77.f,-72.f,1.f,28.f,5.f,4.f,8.f,-2.f,25.f,6.f,-6.f,-9.f,
	    -14.f,9.f,6.f,-5.f,-7.f,4.f,9.f,3.f,-10.f,8.f,-8.f,-1.f,10.f,-2.f,
	    -8.f,-3.f,-6.f,2.f,-4.f,-1.f,4.f,2.f,2.f,5.f,1.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f };
    static real h95[105] = { 0.f,0.f,5306.f,0.f,-2366.f,-413.f,0.f,-262.f,
	    302.f,-427.f,0.f,262.f,-236.f,97.f,-306.f,0.f,46.f,165.f,-143.f,
	    -55.f,107.f,0.f,-17.f,72.f,67.f,-58.f,1.f,36.f,0.f,-69.f,-25.f,
	    4.f,24.f,17.f,-24.f,-6.f,0.f,11.f,-21.f,8.f,-23.f,15.f,11.f,-16.f,
	    -4.f,0.f,-20.f,15.f,12.f,-6.f,-8.f,8.f,5.f,-8.f,3.f,0.f,1.f,0.f,
	    4.f,5.f,-5.f,-1.f,-2.f,1.f,-2.f,-7.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	    0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static real g00[105] = { 0.f,-29619.4f,-1728.2f,-2267.7f,3068.4f,1670.9f,
	    1339.6f,-2288.f,1252.1f,714.5f,932.3f,786.8f,250.f,-403.f,111.3f,
	    -218.8f,351.4f,222.3f,-130.4f,-168.6f,-12.9f,72.3f,68.2f,74.2f,
	    -160.9f,-5.9f,16.9f,-90.4f,79.f,-74.f,0.f,33.3f,9.1f,6.9f,7.3f,
	    -1.2f,24.4f,6.6f,-9.2f,-7.9f,-16.6f,9.1f,7.f,-7.9f,-7.f,5.f,9.4f,
	    3.f,-8.4f,6.3f,-8.9f,-1.5f,9.3f,-4.3f,-8.2f,-2.6f,-6.f,1.7f,-3.1f,
	    -.5f,3.7f,1.f,2.f,4.2f,.3f,-1.1f,2.7f,-1.7f,-1.9f,1.5f,-.1f,.1f,
	    -.7f,.7f,1.7f,.1f,1.2f,4.f,-2.2f,-.3f,.2f,.9f,-.2f,.9f,-.5f,.3f,
	    -.3f,-.4f,-.1f,-.2f,-.4f,-.2f,-.9f,.3f,.1f,-.4f,1.3f,-.4f,.7f,
	    -.4f,.3f,-.1f,.4f,0.f,.1f };
    static real h00[105] = { 0.f,0.f,5186.1f,0.f,-2481.6f,-458.f,0.f,-227.6f,
	    293.4f,-491.1f,0.f,272.6f,-231.9f,119.8f,-303.8f,0.f,43.8f,171.9f,
	    -133.1f,-39.3f,106.3f,0.f,-17.4f,63.7f,65.1f,-61.2f,.7f,43.8f,0.f,
	    -64.6f,-24.2f,6.2f,24.f,14.8f,-25.4f,-5.8f,0.f,11.9f,-21.5f,8.5f,
	    -21.5f,15.5f,8.9f,-14.9f,-2.1f,0.f,-19.7f,13.4f,12.5f,-6.2f,-8.4f,
	    8.4f,3.8f,-8.2f,4.8f,0.f,1.7f,0.f,4.f,4.9f,-5.9f,-1.2f,-2.9f,.2f,
	    -2.2f,-7.4f,0.f,.1f,1.3f,-.9f,-2.6f,.9f,-.7f,-2.8f,-.9f,-1.2f,
	    -1.9f,-.9f,0.f,-.4f,.3f,2.5f,-2.6f,.7f,.3f,0.f,0.f,.3f,-.9f,-.4f,
	    .8f,0.f,-.9f,.2f,1.8f,-.4f,-1.f,-.1f,.7f,.3f,.6f,.3f,-.2f,-.5f,
	    -.9f };
    static real g05[105] = { 0.f,-29554.6f,-1669.1f,-2337.2f,3047.7f,1657.8f,
	    1336.3f,-2305.8f,1246.4f,672.5f,920.6f,798.f,210.7f,-379.9f,100.f,
	    -227.f,354.4f,209.f,-136.5f,-168.1f,-13.6f,73.6f,69.6f,76.7f,
	    -151.3f,-14.6f,14.6f,-86.4f,79.9f,-74.5f,-1.7f,38.7f,12.3f,9.4f,
	    5.4f,1.9f,24.8f,7.6f,-11.7f,-6.9f,-18.1f,10.2f,9.4f,-11.3f,-4.9f,
	    5.6f,9.8f,3.6f,-6.9f,5.f,-10.8f,-1.3f,8.8f,-6.7f,-9.2f,-2.2f,
	    -6.1f,1.4f,-2.4f,-.2f,3.1f,.3f,2.1f,3.8f,-.2f,-2.1f,3.f,-1.6f,
	    -1.9f,1.4f,-.3f,.3f,-.8f,.5f,1.8f,.2f,1.f,4.f,-2.2f,-.3f,.2f,.9f,
	    -.4f,1.f,-.3f,.5f,-.4f,-.4f,.1f,-.5f,-.1f,-.2f,-.9f,.3f,.3f,-.4f,
	    1.2f,-.4f,.8f,-.3f,.4f,-.1f,.4f,-.1f,-.2f };
    static real h05[105] = { 0.f,0.f,5078.f,0.f,-2594.5f,-515.4f,0.f,-198.9f,
	    269.7f,-524.7f,0.f,282.1f,-225.2f,145.2f,-305.4f,0.f,42.7f,180.3f,
	    -123.5f,-19.6f,103.9f,0.f,-20.3f,54.8f,63.6f,-63.5f,.2f,50.9f,0.f,
	    -61.1f,-22.6f,6.8f,25.4f,10.9f,-26.3f,-4.6f,0.f,11.2f,-20.9f,9.8f,
	    -19.7f,16.2f,7.6f,-12.8f,-.1f,0.f,-20.1f,12.7f,12.7f,-6.7f,-8.2f,
	    8.1f,2.9f,-7.7f,6.f,0.f,2.2f,.1f,4.5f,4.8f,-6.6f,-1.f,-3.5f,-.9f,
	    -2.3f,-7.9f,0.f,.3f,1.4f,-.8f,-2.3f,.9f,-.6f,-2.7f,-1.1f,-1.6f,
	    -1.9f,-1.4f,0.f,-.6f,.2f,2.4f,-2.6f,.6f,.4f,0.f,0.f,.3f,-.9f,-.3f,
	    .9f,0.f,-.8f,.3f,1.7f,-.5f,-1.1f,-0.f,.6f,.2f,.5f,.4f,-.2f,-.6f,
	    -.8f };
    static real g10[105] = { 0.f,-29496.5f,-1585.9f,-2396.6f,3026.f,1668.6f,
	    1339.7f,-2326.3f,1231.7f,634.2f,912.6f,809.f,166.6f,-357.1f,89.7f,
	    -231.1f,357.2f,200.3f,-141.2f,-163.1f,-7.7f,72.8f,68.6f,76.f,
	    -141.4f,-22.9f,13.1f,-77.9f,80.4f,-75.f,-4.7f,45.3f,14.f,10.4f,
	    1.6f,4.9f,24.3f,8.2f,-14.5f,-5.7f,-19.3f,11.6f,10.9f,-14.1f,-3.7f,
	    5.4f,9.4f,3.4f,-5.3f,3.1f,-12.4f,-.8f,8.4f,-8.4f,-10.1f,-2.f,
	    -6.3f,.9f,-1.1f,-.2f,2.5f,-.3f,2.2f,3.1f,-1.f,-2.8f,3.f,-1.5f,
	    -2.1f,1.6f,-.5f,.5f,-.8f,.4f,1.8f,.2f,.8f,3.8f,-2.1f,-.2f,.3f,1.f,
	    -.7f,.9f,-.1f,.5f,-.4f,-.4f,.2f,-.8f,0.f,-.2f,-.9f,.3f,.4f,-.4f,
	    1.1f,-.3f,.8f,-.2f,.4f,0.f,.4f,-.3f,-.3f };
    static real h10[105] = { 0.f,0.f,4945.1f,0.f,-2707.7f,-575.4f,0.f,-160.5f,
	    251.7f,-536.8f,0.f,286.4f,-211.2f,164.4f,-309.2f,0.f,44.7f,188.9f,
	    -118.1f,.1f,100.9f,0.f,-20.8f,44.2f,61.5f,-66.3f,3.1f,54.9f,0.f,
	    -57.8f,-21.2f,6.6f,24.9f,7.f,-27.7f,-3.4f,0.f,10.9f,-20.f,11.9f,
	    -17.4f,16.7f,7.1f,-10.8f,1.7f,0.f,-20.5f,11.6f,12.8f,-7.2f,-7.4f,
	    8.f,2.2f,-6.1f,7.f,0.f,2.8f,-.1f,4.7f,4.4f,-7.2f,-1.f,-4.f,-2.f,
	    -2.f,-8.3f,0.f,.1f,1.7f,-.6f,-1.8f,.9f,-.4f,-2.5f,-1.3f,-2.1f,
	    -1.9f,-1.8f,0.f,-.8f,.3f,2.2f,-2.5f,.5f,.6f,0.f,.1f,.3f,-.9f,-.2f,
	    .8f,0.f,-.8f,.3f,1.7f,-.6f,-1.2f,-.1f,.5f,.1f,.5f,.4f,-.2f,-.5f,
	    -.8f };
    static real dg10[45] = { 0.f,11.4f,16.7f,-11.3f,-3.9f,2.7f,1.3f,-3.9f,
	    -2.9f,-8.1f,-1.4f,2.f,-8.9f,4.4f,-2.3f,-.5f,.5f,-1.5f,-.7f,1.3f,
	    1.4f,-.3f,-.3f,-.3f,1.9f,-1.6f,-.2f,1.8f,.2f,-.1f,-.6f,1.4f,.3f,
	    .1f,-.8f,.4f,-.1f,.1f,-.5f,.3f,-.3f,.3f,.2f,-.5f,.2f };
    static real dh10[45] = { 0.f,0.f,-28.8f,0.f,-23.f,-12.9f,0.f,8.6f,-2.9f,
	    -2.1f,0.f,.4f,3.2f,3.6f,-.8f,0.f,.5f,1.5f,.9f,3.7f,-.6f,0.f,-.1f,
	    -2.1f,-.4f,-.5f,.8f,.5f,0.f,.6f,.3f,-.2f,-.1f,-.8f,-.3f,.2f,0.f,
	    0.f,.2f,.5f,.4f,.1f,-.1f,.4f,.4f };

    /* Format strings */
    static char fmt_10[] = "(//1x,\002**** RECALC WARNS: YEAR IS OUT OF INTE"
	    "RVAL 1965-2015: IYEAR=\002,i4,/,6x,\002CALCULATIONS WILL BE DONE"
	    " FOR IYEAR=\002,i4,/)";

    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    integer f2c_s_wsfe(cilist *), f2c_do_fio(integer *, char *, ftnlen), f2c_e_wsfe(void);
    double sqrt(doublereal), cos(doublereal), sin(doublereal), asin(
	    doublereal), atan2(doublereal, doublereal);

    /* Local variables */
    static integer m, n;
    static real p, s, t, y, f1, f2;
    static integer n2;
    static real s1, s2, s3, y1, y2, y3, z1, z2, z3, aa;
    static integer iy, mn;
    static real dt, sq, dj, dy1, dz1, dz2, dz3, dy2, dy3, gg10, gg11, hh11;
    static integer mnn;
    static real gst, sqq, sqr;
    extern /* Subroutine */ int sun_(integer *, integer *, integer *, integer 
	    *, integer *, real *, real *, real *, real *);
    static real dip1, dip2, dip3, sdec, obliq, slong, srasn, exmagx, exmagy, 
	    exmagz, eymagx, eymagy;

    /* Fortran I/O blocks */
    static cilist io___136 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___137 = { 0, 6, 0, fmt_10, 0 };



/*  1. PREPARES ELEMENTS OF ROTATION MATRICES FOR TRANSFORMATIONS OF VECTORS BETWEEN */
/*     SEVERAL COORDINATE SYSTEMS, MOST FREQUENTLY USED IN SPACE PHYSICS. */

/*  2. PREPARES COEFFICIENTS USED IN THE CALCULATION OF THE MAIN GEOMAGNETIC FIELD */
/*      (IGRF MODEL) */

/*  THIS SUBROUTINE SHOULD BE INVOKED BEFORE USING THE FOLLOWING SUBROUTINES: */
/*    IGRF_GEO, IGRF_GSM, DIP, GEOMAG, GEOGSM, MAGSM, SMGSM, GSMGSE, GEIGEO. */

/*  THERE IS NO NEED TO REPEATEDLY INVOKE RECALC, IF MULTIPLE CALCULATIONS ARE MADE */
/*    FOR THE SAME DATE AND TIME. */

/* -----INPUT PARAMETERS: */

/*     IYEAR   -  YEAR NUMBER (FOUR DIGITS) */
/*     IDAY  -  DAY OF YEAR (DAY 1 = JAN 1) */
/*     IHOUR -  HOUR OF DAY (00 TO 23) */
/*     MIN   -  MINUTE OF HOUR (00 TO 59) */
/*     ISEC  -  SECONDS OF MINUTE (00 TO 59) */

/* -----OUTPUT PARAMETERS:   NONE (ALL OUTPUT QUANTITIES ARE PLACED */
/*                         INTO THE COMMON BLOCKS /GEOPACK1/ AND /GEOPACK2/) */

/*    OTHER SUBROUTINES CALLED BY THIS ONE: SUN */

/*    AUTHOR:  N.A. TSYGANENKO */
/*    DATE:    DEC.1, 1991 */

/*    CORRECTION OF MAY 9, 2006:  INTERPOLATION OF THE COEFFICIENTS (BETWEEN */
/*     LABELS 50 AND 105) IS NOW MADE THROUGH THE LAST ELEMENT OF THE ARRAYS */
/*     G(105)  AND H(105) (PREVIOUSLY MADE ONLY THROUGH N=66, WHICH IN SOME */
/*     CASES CAUSED RUNTIME ERRORS) */

/*    REVISION OF MAY 3, 2005: */
/*     The table of IGRF coefficients was extended to include those for the epoch 2005 */
/*       the maximal order of spherical harmonics was also increased up to 13 */
/*         (for details, see http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html) */

/*    REVISION OF APRIL 3, 2003: */
/*     The code now includes preparation of the model coefficients for the subroutines */
/*       IGRF and GEOMAG. This eliminates the need for the SAVE statements, used in the */
/*        old versions, making the codes easier and more compiler-independent. */


/*  THE COMMON BLOCK /GEOPACK1/ CONTAINS ELEMENTS OF THE ROTATION MATRICES AND OTHER */
/*   PARAMETERS RELATED TO THE COORDINATE TRANSFORMATIONS PERFORMED BY THIS PACKAGE */


/*  THE COMMON BLOCK /GEOPACK2/ CONTAINS COEFFICIENTS OF THE IGRF FIELD MODEL, CALCULATED */
/*    FOR A GIVEN YEAR AND DAY FROM THEIR STANDARD EPOCH VALUES. THE ARRAY REC CONTAINS */
/*    COEFFICIENTS USED IN THE RECURSION RELATIONS FOR LEGENDRE ASSOCIATE POLYNOMIALS. */







/*      DATA G05/0.,-29556.8, -1671.8, -2340.5,   3047.,  1656.9,  1335.7, */
/*     *    -2305.3,  1246.8,   674.4,   919.8,   798.2,   211.5,  -379.5, */
/*     *      100.2,  -227.6,   354.4,   208.8,  -136.6,  -168.3,   -14.1, */
/*     *       72.9,    69.6,    76.6,  -151.1,   -15.0,    14.7,   -86.4, */
/*     *       79.8,   -74.4,    -1.4,    38.6,    12.3,     9.4,     5.5, */
/*     *        2.0,    24.8,     7.7,   -11.4,    -6.8,   -18.0,    10.0, */
/*     *        9.4,   -11.4,    -5.0,     5.6,     9.8,     3.6,    -7.0, */
/*     *        5.0,   -10.8,    -1.3,     8.7,    -6.7,    -9.2,    -2.2, */
/*     *       -6.3,     1.6,    -2.5,    -0.1,     3.0,     0.3,     2.1, */
/*     *        3.9,    -0.1,    -2.2,     2.9,    -1.6,    -1.7,     1.5, */
/*     *       -0.2,     0.2,    -0.7,     0.5,     1.8,     0.1,     1.0, */
/*     *        4.1,    -2.2,    -0.3,     0.3,     0.9,    -0.4,     1.0, */
/*     *       -0.4,     0.5,    -0.3,    -0.4,     0.0,    -0.4,     0.0, */
/*     *       -0.2,    -0.9,     0.3,     0.3,    -0.4,     1.2,    -0.4, */
/*     *        0.7,    -0.3,     0.4,    -0.1,     0.4,    -0.1,    -0.3/ */
/*      DATA H05/0.,     0.0,  5080.0,     0.0, -2594.9,  -516.7,     0.0, */
/*     *     -200.4,   269.3,  -524.5,     0.0,   281.4,  -225.8,   145.7, */
/*     *     -304.7,     0.0,    42.7,   179.8,  -123.0,   -19.5,   103.6, */
/*     *        0.0,   -20.2,    54.7,    63.7,   -63.4,     0.0,    50.3, */
/*     *        0.0,   -61.4,   -22.5,     6.9,    25.4,    10.9,   -26.4, */
/*     *       -4.8,     0.0,    11.2,   -21.0,     9.7,   -19.8,    16.1, */
/*     *        7.7,   -12.8,    -0.1,     0.0,   -20.1,    12.9,    12.7, */
/*     *       -6.7,    -8.1,     8.1,     2.9,    -7.9,     5.9,     0.0, */
/*     *        2.4,     0.2,     4.4,     4.7,    -6.5,    -1.0,    -3.4, */
/*     *       -0.9,    -2.3,    -8.0,     0.0,     0.3,     1.4,    -0.7, */
/*     *       -2.4,     0.9,    -0.6,    -2.7,    -1.0,    -1.5,    -2.0, */
/*     *       -1.4,     0.0,    -0.5,     0.3,     2.3,    -2.7,     0.6, */
/*     *        0.4,     0.0,     0.0,     0.3,    -0.8,    -0.4,     1.0, */
/*     *        0.0,    -0.7,     0.3,     1.7,    -0.5,    -1.0,     0.0, */
/*     *        0.7,     0.2,     0.6,     0.4,    -0.2,    -0.5,    -1.0/ */
/*      DATA DG05/0.0,   8.8,    10.8,   -15.0,    -6.9,    -1.0,    -0.3, */
/*     *         -3.1,  -0.9,    -6.8,    -2.5,     2.8,    -7.1,     5.9, */
/*     *         -3.2,  -2.6,     0.4,    -3.0,    -1.2,     0.2,    -0.6, */
/*     *         -0.8,   0.2,    -0.2,     2.1,    -2.1,    -0.4,     1.3, */
/*     *         -0.4,   0.0,    -0.2,     1.1,     0.6,     0.4,    -0.5, */
/*     *          0.9,  -0.2,     0.2,    -0.2,     0.2,    -0.2,     0.2, */
/*     *          0.5,  -0.7,     0.5/ */
/*      DATA DH05/0.0,   0.0,   -21.3,     0.0,   -23.3,   -14.0,     0.0, */
/*     *          5.4,  -6.5,    -2.0,     0.0,     2.0,     1.8,     5.6, */
/*     *          0.0,   0.0,     0.1,     1.8,     2.0,     4.5,    -1.0, */
/*     *          0.0,  -0.4,    -1.9,    -0.4,    -0.4,    -0.2,     0.9, */
/*     *          0.0,   0.8,     0.4,     0.1,     0.2,    -0.9,    -0.3, */
/*     *          0.3,   0.0,    -0.2,     0.2,     0.2,     0.4,     0.2, */
/*     *         -0.3,   0.5,     0.4/ */
/*     HK 20100510: IGRF coefficient for 2005 updated and for 2010 added. */


    iy = *iyear;

/*  WE ARE RESTRICTED BY THE INTERVAL 1965-2010, FOR WHICH THE IGRF COEFFICIENTS */
/*    ARE KNOWN; IF IYEAR IS OUTSIDE THIS INTERVAL, THEN THE SUBROUTINE USES THE */
/*      NEAREST LIMITING VALUE AND PRINTS A WARNING: */

    if (iy < 1965) {
	iy = 1965;
	f2c_s_wsfe(&io___136);
	f2c_do_fio(&c__1, (char *)&(*iyear), (ftnlen)sizeof(integer));
	f2c_do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
	f2c_e_wsfe();
    }
    if (iy > 2015) {
	iy = 2015;
	f2c_s_wsfe(&io___137);
	f2c_do_fio(&c__1, (char *)&(*iyear), (ftnlen)sizeof(integer));
	f2c_do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
	f2c_e_wsfe();
    }

/*  CALCULATE THE ARRAY REC, CONTAINING COEFFICIENTS FOR THE RECURSION RELATIONS, */
/*  USED IN THE IGRF SUBROUTINE FOR CALCULATING THE ASSOCIATE LEGENDRE POLYNOMIALS */
/*  AND THEIR DERIVATIVES: */

    for (n = 1; n <= 14; ++n) {
	n2 = (n << 1) - 1;
	n2 *= n2 - 2;
	i__1 = n;
	for (m = 1; m <= i__1; ++m) {
	    mn = n * (n - 1) / 2 + m;
/* L20: */
	    geopack2_1.rec[mn - 1] = (real) ((n - m) * (n + m - 2)) / (real) 
		    n2;
	}
    }

    if (iy < 1970) {
	goto L50;
    }
/* INTERPOLATE BETWEEN 1965 - 1970 */
    if (iy < 1975) {
	goto L60;
    }
/* INTERPOLATE BETWEEN 1970 - 1975 */
    if (iy < 1980) {
	goto L70;
    }
/* INTERPOLATE BETWEEN 1975 - 1980 */
    if (iy < 1985) {
	goto L80;
    }
/* INTERPOLATE BETWEEN 1980 - 1985 */
    if (iy < 1990) {
	goto L90;
    }
/* INTERPOLATE BETWEEN 1985 - 1990 */
    if (iy < 1995) {
	goto L100;
    }
/* INTERPOLATE BETWEEN 1990 - 1995 */
    if (iy < 2000) {
	goto L110;
    }
/* INTERPOLATE BETWEEN 1995 - 2000 */
    if (iy < 2005) {
	goto L120;
    }
/* INTERPOLATE BETWEEN 2000 - 2005 */
    if (iy < 2010) {
	goto L130;
    }

/*       EXTRAPOLATE BEYOND 2010: */

/* INTERPOLATE BETWEEN 2005 - 2010 */
    dt = (real) iy + (real) (*iday - 1) / 365.25f - 2010.f;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g10[n - 1];
	geopack2_1.h__[n - 1] = h10[n - 1];
	if (n > 45) {
	    goto L40;
	}
	geopack2_1.g[n - 1] += dg10[n - 1] * dt;
	geopack2_1.h__[n - 1] += dh10[n - 1] * dt;
L40:
	;
    }
    goto L300;

/*       INTERPOLATE BETWEEEN 1965 - 1970: */

L50:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 1965) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g65[n - 1] * f1 + g70[n - 1] * f2;
/* L55: */
	geopack2_1.h__[n - 1] = h65[n - 1] * f1 + h70[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 1970 - 1975: */

L60:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 1970) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g70[n - 1] * f1 + g75[n - 1] * f2;
/* L65: */
	geopack2_1.h__[n - 1] = h70[n - 1] * f1 + h75[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 1975 - 1980: */

L70:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 1975) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g75[n - 1] * f1 + g80[n - 1] * f2;
/* L75: */
	geopack2_1.h__[n - 1] = h75[n - 1] * f1 + h80[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 1980 - 1985: */

L80:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 1980) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g80[n - 1] * f1 + g85[n - 1] * f2;
/* L85: */
	geopack2_1.h__[n - 1] = h80[n - 1] * f1 + h85[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 1985 - 1990: */

L90:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 1985) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g85[n - 1] * f1 + g90[n - 1] * f2;
/* L95: */
	geopack2_1.h__[n - 1] = h85[n - 1] * f1 + h90[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 1990 - 1995: */

L100:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 1990) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g90[n - 1] * f1 + g95[n - 1] * f2;
/* L105: */
	geopack2_1.h__[n - 1] = h90[n - 1] * f1 + h95[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 1995 - 2000: */

L110:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 1995) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
/*  THE 2000 COEFFICIENTS (G00) GO THROUGH THE ORD */
	geopack2_1.g[n - 1] = g95[n - 1] * f1 + g00[n - 1] * f2;
/* L115: */
	geopack2_1.h__[n - 1] = h95[n - 1] * f1 + h00[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 2000 - 2005: */

L120:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 2000) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g00[n - 1] * f1 + g05[n - 1] * f2;
/* L125: */
	geopack2_1.h__[n - 1] = h00[n - 1] * f1 + h05[n - 1] * f2;
    }
    goto L300;

/*       INTERPOLATE BETWEEN 2005 - 2010: */

L130:
    f2 = ((real) iy + (real) (*iday - 1) / 365.25f - 2005) / 5.f;
    f1 = 1.f - f2;
    for (n = 1; n <= 105; ++n) {
	geopack2_1.g[n - 1] = g05[n - 1] * f1 + g10[n - 1] * f2;
/* L135: */
	geopack2_1.h__[n - 1] = h05[n - 1] * f1 + h10[n - 1] * f2;
    }
    goto L300;

/*   COEFFICIENTS FOR A GIVEN YEAR HAVE BEEN CALCULATED; NOW MULTIPLY */
/*   THEM BY SCHMIDT NORMALIZATION FACTORS: */

L300:
    s = 1.f;
    for (n = 2; n <= 14; ++n) {
	mn = n * (n - 1) / 2 + 1;
	s = s * (real) ((n << 1) - 3) / (real) (n - 1);
	geopack2_1.g[mn - 1] *= s;
	geopack2_1.h__[mn - 1] *= s;
	p = s;
	i__1 = n;
	for (m = 2; m <= i__1; ++m) {
	    aa = 1.f;
	    if (m == 2) {
		aa = 2.f;
	    }
	    p *= sqrt(aa * (real) (n - m + 1) / (real) (n + m - 2));
	    mnn = mn + m - 1;
	    geopack2_1.g[mnn - 1] *= p;
/* L310: */
	    geopack2_1.h__[mnn - 1] *= p;
	}
    }
    gg10 = -geopack2_1.g[1];
    gg11 = geopack2_1.g[2];
    hh11 = geopack2_1.h__[2];

/*  NOW CALCULATE THE COMPONENTS OF THE UNIT VECTOR EzMAG IN GEO COORD.SYSTEM: */
/*   SIN(TETA0)*COS(LAMBDA0), SIN(TETA0)*SIN(LAMBDA0), AND COS(TETA0) */
/*         ST0 * CL0                ST0 * SL0                CT0 */

/* Computing 2nd power */
    r__1 = gg11;
/* Computing 2nd power */
    r__2 = hh11;
    sq = r__1 * r__1 + r__2 * r__2;
    sqq = sqrt(sq);
/* Computing 2nd power */
    r__1 = gg10;
    sqr = sqrt(r__1 * r__1 + sq);
    geopack1_2.sl0 = -hh11 / sqq;
    geopack1_2.cl0 = -gg11 / sqq;
    geopack1_2.st0 = sqq / sqr;
    geopack1_2.ct0 = gg10 / sqr;
    geopack1_2.stcl = geopack1_2.st0 * geopack1_2.cl0;
    geopack1_2.stsl = geopack1_2.st0 * geopack1_2.sl0;
    geopack1_2.ctsl = geopack1_2.ct0 * geopack1_2.sl0;
    geopack1_2.ctcl = geopack1_2.ct0 * geopack1_2.cl0;

    sun_(&iy, iday, ihour, min__, isec, &gst, &slong, &srasn, &sdec);

/*  S1,S2, AND S3 ARE THE COMPONENTS OF THE UNIT VECTOR EXGSM=EXGSE IN THE */
/*   SYSTEM GEI POINTING FROM THE EARTH'S CENTER TO THE SUN: */

    s1 = cos(srasn) * cos(sdec);
    s2 = sin(srasn) * cos(sdec);
    s3 = sin(sdec);
    geopack1_2.cgst = cos(gst);
    geopack1_2.sgst = sin(gst);

/*  DIP1, DIP2, AND DIP3 ARE THE COMPONENTS OF THE UNIT VECTOR EZSM=EZMAG */
/*   IN THE SYSTEM GEI: */

    dip1 = geopack1_2.stcl * geopack1_2.cgst - geopack1_2.stsl * 
	    geopack1_2.sgst;
    dip2 = geopack1_2.stcl * geopack1_2.sgst + geopack1_2.stsl * 
	    geopack1_2.cgst;
    dip3 = geopack1_2.ct0;

/*  NOW CALCULATE THE COMPONENTS OF THE UNIT VECTOR EYGSM IN THE SYSTEM GEI */
/*   BY TAKING THE VECTOR PRODUCT D x S AND NORMALIZING IT TO UNIT LENGTH: */

    y1 = dip2 * s3 - dip3 * s2;
    y2 = dip3 * s1 - dip1 * s3;
    y3 = dip1 * s2 - dip2 * s1;
    y = sqrt(y1 * y1 + y2 * y2 + y3 * y3);
    y1 /= y;
    y2 /= y;
    y3 /= y;

/*   THEN IN THE GEI SYSTEM THE UNIT VECTOR Z = EZGSM = EXGSM x EYGSM = S x Y */
/*    HAS THE COMPONENTS: */

    z1 = s2 * y3 - s3 * y2;
    z2 = s3 * y1 - s1 * y3;
    z3 = s1 * y2 - s2 * y1;

/*    THE VECTOR EZGSE (HERE DZ) IN GEI HAS THE COMPONENTS (0,-SIN(DELTA), */
/*     COS(DELTA)) = (0.,-0.397823,0.917462); HERE DELTA = 23.44214 DEG FOR */
/*   THE EPOCH 1978 (SEE THE BOOK BY GUREVICH OR OTHER ASTRONOMICAL HANDBOOKS). */
/*    HERE THE MOST ACCURATE TIME-DEPENDENT FORMULA IS USED: */

    dj = (real) ((iy - 1900) * 365 + (iy - 1901) / 4 + *iday) - .5f + (real) (
	    *ihour * 3600 + *min__ * 60 + *isec) / 86400.f;
    t = dj / 36525.f;
    obliq = (23.45229f - t * .0130125f) / 57.2957795f;
    dz1 = 0.f;
    dz2 = -sin(obliq);
    dz3 = cos(obliq);

/*  THEN THE UNIT VECTOR EYGSE IN GEI SYSTEM IS THE VECTOR PRODUCT DZ x S : */

    dy1 = dz2 * s3 - dz3 * s2;
    dy2 = dz3 * s1 - dz1 * s3;
    dy3 = dz1 * s2 - dz2 * s1;

/*   THE ELEMENTS OF THE MATRIX GSE TO GSM ARE THE SCALAR PRODUCTS: */
/*   CHI=EM22=(EYGSM,EYGSE), SHI=EM23=(EYGSM,EZGSE), EM32=(EZGSM,EYGSE)=-EM23, */
/*     AND EM33=(EZGSM,EZGSE)=EM22 */

    geopack1_2.chi = y1 * dy1 + y2 * dy2 + y3 * dy3;
    geopack1_2.shi = y1 * dz1 + y2 * dz2 + y3 * dz3;
    geopack1_2.hi = asin(geopack1_2.shi);

/*    TILT ANGLE: PSI=ARCSIN(DIP,EXGSM) */

    geopack1_2.sps = dip1 * s1 + dip2 * s2 + dip3 * s3;
/* Computing 2nd power */
    r__1 = geopack1_2.sps;
    geopack1_2.cps = sqrt(1.f - r__1 * r__1);
    geopack1_2.psi = asin(geopack1_2.sps);

/*    THE ELEMENTS OF THE MATRIX MAG TO SM ARE THE SCALAR PRODUCTS: */
/* CFI=GM22=(EYSM,EYMAG), SFI=GM23=(EYSM,EXMAG); THEY CAN BE DERIVED AS FOLLOWS: */

/* IN GEO THE VECTORS EXMAG AND EYMAG HAVE THE COMPONENTS (CT0*CL0,CT0*SL0,-ST0) */
/*  AND (-SL0,CL0,0), RESPECTIVELY.    HENCE, IN GEI THE COMPONENTS ARE: */
/*  EXMAG:    CT0*CL0*COS(GST)-CT0*SL0*SIN(GST) */
/*            CT0*CL0*SIN(GST)+CT0*SL0*COS(GST) */
/*            -ST0 */
/*  EYMAG:    -SL0*COS(GST)-CL0*SIN(GST) */
/*            -SL0*SIN(GST)+CL0*COS(GST) */
/*             0 */
/*  THE COMPONENTS OF EYSM IN GEI WERE FOUND ABOVE AS Y1, Y2, AND Y3; */
/*  NOW WE ONLY HAVE TO COMBINE THE QUANTITIES INTO SCALAR PRODUCTS: */

    exmagx = geopack1_2.ct0 * (geopack1_2.cl0 * geopack1_2.cgst - 
	    geopack1_2.sl0 * geopack1_2.sgst);
    exmagy = geopack1_2.ct0 * (geopack1_2.cl0 * geopack1_2.sgst + 
	    geopack1_2.sl0 * geopack1_2.cgst);
    exmagz = -geopack1_2.st0;
    eymagx = -(geopack1_2.sl0 * geopack1_2.cgst + geopack1_2.cl0 * 
	    geopack1_2.sgst);
    eymagy = -(geopack1_2.sl0 * geopack1_2.sgst - geopack1_2.cl0 * 
	    geopack1_2.cgst);
    geopack1_2.cfi = y1 * eymagx + y2 * eymagy;
    geopack1_2.sfi = y1 * exmagx + y2 * exmagy + y3 * exmagz;

    geopack1_2.xmut = (atan2(geopack1_2.sfi, geopack1_2.cfi) + 3.1415926536f) 
	    * 3.8197186342f;

/*  THE ELEMENTS OF THE MATRIX GEO TO GSM ARE THE SCALAR PRODUCTS: */

/*   A11=(EXGEO,EXGSM), A12=(EYGEO,EXGSM), A13=(EZGEO,EXGSM), */
/*   A21=(EXGEO,EYGSM), A22=(EYGEO,EYGSM), A23=(EZGEO,EYGSM), */
/*   A31=(EXGEO,EZGSM), A32=(EYGEO,EZGSM), A33=(EZGEO,EZGSM), */

/*   ALL THE UNIT VECTORS IN BRACKETS ARE ALREADY DEFINED IN GEI: */

/*  EXGEO=(CGST,SGST,0), EYGEO=(-SGST,CGST,0), EZGEO=(0,0,1) */
/*  EXGSM=(S1,S2,S3),  EYGSM=(Y1,Y2,Y3),   EZGSM=(Z1,Z2,Z3) */
/*                                                           AND  THEREFORE: */

    geopack1_2.a11 = s1 * geopack1_2.cgst + s2 * geopack1_2.sgst;
    geopack1_2.a12 = -s1 * geopack1_2.sgst + s2 * geopack1_2.cgst;
    geopack1_2.a13 = s3;
    geopack1_2.a21 = y1 * geopack1_2.cgst + y2 * geopack1_2.sgst;
    geopack1_2.a22 = -y1 * geopack1_2.sgst + y2 * geopack1_2.cgst;
    geopack1_2.a23 = y3;
    geopack1_2.a31 = z1 * geopack1_2.cgst + z2 * geopack1_2.sgst;
    geopack1_2.a32 = -z1 * geopack1_2.sgst + z2 * geopack1_2.cgst;
    geopack1_2.a33 = z3;

    return 0;
} /* recalc_ */


/* ==================================================================== */

/* Subroutine */ int geomag_(real *xgeo, real *ygeo, real *zgeo, real *xmag, 
	real *ymag, real *zmag, integer *j)
{

/*    CONVERTS GEOGRAPHIC (GEO) TO DIPOLE (MAG) COORDINATES OR VICA VERSA. */

/*                    J>0                       J<0 */
/* -----INPUT:  J,XGEO,YGEO,ZGEO           J,XMAG,YMAG,ZMAG */
/* -----OUTPUT:    XMAG,YMAG,ZMAG           XGEO,YGEO,ZGEO */


/*  ATTENTION:  SUBROUTINE  RECALC  MUST BE INVOKED BEFORE GEOMAG IN TWO CASES: */
/*     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES */
/*     /B/  IF THE VALUES OF IYEAR AND/OR IDAY HAVE BEEN CHANGED */


/*   LAST MOFIFICATION:  MARCH 30, 2003 (INVOCATION OF RECALC INSIDE THIS S/R WAS REMOVED) */

/*   AUTHOR:  N. A. TSYGANENKO */

    if (*j > 0) {
	*xmag = *xgeo * geopack1_3.ctcl + *ygeo * geopack1_3.ctsl - *zgeo * 
		geopack1_3.st0;
	*ymag = *ygeo * geopack1_3.cl0 - *xgeo * geopack1_3.sl0;
	*zmag = *xgeo * geopack1_3.stcl + *ygeo * geopack1_3.stsl + *zgeo * 
		geopack1_3.ct0;
    } else {
	*xgeo = *xmag * geopack1_3.ctcl - *ymag * geopack1_3.sl0 + *zmag * 
		geopack1_3.stcl;
	*ygeo = *xmag * geopack1_3.ctsl + *ymag * geopack1_3.cl0 + *zmag * 
		geopack1_3.stsl;
	*zgeo = *zmag * geopack1_3.ct0 - *xmag * geopack1_3.st0;
    }
    return 0;
} /* geomag_ */


/* ========================================================================================= */

/* Subroutine */ int geigeo_(real *xgei, real *ygei, real *zgei, real *xgeo, 
	real *ygeo, real *zgeo, integer *j)
{

/*   CONVERTS EQUATORIAL INERTIAL (GEI) TO GEOGRAPHICAL (GEO) COORDS */
/*   OR VICA VERSA. */
/*                    J>0                J<0 */
/* ----INPUT:  J,XGEI,YGEI,ZGEI    J,XGEO,YGEO,ZGEO */
/* ----OUTPUT:   XGEO,YGEO,ZGEO      XGEI,YGEI,ZGEI */

/*  ATTENTION:  SUBROUTINE  RECALC  MUST BE INVOKED BEFORE GEIGEO IN TWO CASES: */
/*     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES */
/*     /B/  IF THE CURRENT VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC HAVE BEEN CHANGED */

/*     LAST MODIFICATION:  MARCH 31, 2003 */
/*     AUTHOR:  N. A. TSYGANENKO */


    if (*j > 0) {
	*xgeo = *xgei * geopack1_4.cgst + *ygei * geopack1_4.sgst;
	*ygeo = *ygei * geopack1_4.cgst - *xgei * geopack1_4.sgst;
	*zgeo = *zgei;
    } else {
	*xgei = *xgeo * geopack1_4.cgst - *ygeo * geopack1_4.sgst;
	*ygei = *ygeo * geopack1_4.cgst + *xgeo * geopack1_4.sgst;
	*zgei = *zgeo;
    }
    return 0;
} /* geigeo_ */


/* ======================================================================================= */

/* Subroutine */ int magsm_(real *xmag, real *ymag, real *zmag, real *xsm, 
	real *ysm, real *zsm, integer *j)
{

/*  CONVERTS DIPOLE (MAG) TO SOLAR MAGNETIC (SM) COORDINATES OR VICA VERSA */

/*                    J>0              J<0 */
/* ----INPUT: J,XMAG,YMAG,ZMAG     J,XSM,YSM,ZSM */
/* ----OUTPUT:    XSM,YSM,ZSM       XMAG,YMAG,ZMAG */

/*  ATTENTION:  SUBROUTINE  RECALC  MUST BE INVOKED BEFORE MAGSM IN TWO CASES: */
/*     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES */
/*     /B/  IF THE VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC HAVE BEEN CHANGED */

/*     LAST MODIFICATION:  MARCH 31, 2003 */

/*     AUTHOR:  N. A. TSYGANENKO */


    if (*j > 0) {
	*xsm = *xmag * geopack1_5.cfi - *ymag * geopack1_5.sfi;
	*ysm = *xmag * geopack1_5.sfi + *ymag * geopack1_5.cfi;
	*zsm = *zmag;
    } else {
	*xmag = *xsm * geopack1_5.cfi + *ysm * geopack1_5.sfi;
	*ymag = *ysm * geopack1_5.cfi - *xsm * geopack1_5.sfi;
	*zmag = *zsm;
    }
    return 0;
} /* magsm_ */


/* ======================================================================================= */

/* Subroutine */ int gsmgse_(real *xgsm, real *ygsm, real *zgsm, real *xgse, 
	real *ygse, real *zgse, integer *j)
{

/* CONVERTS GEOCENTRIC SOLAR MAGNETOSPHERIC (GSM) COORDS TO SOLAR ECLIPTIC (GSE) ONES */
/*   OR VICA VERSA. */
/*                    J>0                J<0 */
/* -----INPUT: J,XGSM,YGSM,ZGSM    J,XGSE,YGSE,ZGSE */
/* ----OUTPUT:   XGSE,YGSE,ZGSE      XGSM,YGSM,ZGSM */

/*  ATTENTION:  SUBROUTINE  RECALC  MUST BE INVOKED BEFORE GSMGSE IN TWO CASES: */
/*     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES */
/*     /B/  IF THE VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC HAVE BEEN CHANGED */

/*     LAST MODIFICATION:  MARCH 31, 2003 */

/*     AUTHOR:  N. A. TSYGANENKO */


    if (*j > 0) {
	*xgse = *xgsm;
	*ygse = *ygsm * geopack1_6.chi - *zgsm * geopack1_6.shi;
	*zgse = *ygsm * geopack1_6.shi + *zgsm * geopack1_6.chi;
    } else {
	*xgsm = *xgse;
	*ygsm = *ygse * geopack1_6.chi + *zgse * geopack1_6.shi;
	*zgsm = *zgse * geopack1_6.chi - *ygse * geopack1_6.shi;
    }
    return 0;
} /* gsmgse_ */


/* ===================================================================================== */

/* Subroutine */ int smgsm_(real *xsm, real *ysm, real *zsm, real *xgsm, real 
	*ygsm, real *zgsm, integer *j)
{

/* CONVERTS SOLAR MAGNETIC (SM) TO GEOCENTRIC SOLAR MAGNETOSPHERIC */
/*   (GSM) COORDINATES OR VICA VERSA. */
/*                  J>0                 J<0 */
/* -----INPUT: J,XSM,YSM,ZSM        J,XGSM,YGSM,ZGSM */
/* ----OUTPUT:  XGSM,YGSM,ZGSM       XSM,YSM,ZSM */

/*  ATTENTION:  SUBROUTINE RECALC  MUST BE INVOKED BEFORE SMGSM IN TWO CASES: */
/*     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES */
/*     /B/  IF THE VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC HAVE BEEN CHANGED */

/*     LAST MODIFICATION:  MARCH 31, 2003 */

/*     AUTHOR:  N. A. TSYGANENKO */

    if (*j > 0) {
	*xgsm = *xsm * geopack1_7.cps + *zsm * geopack1_7.sps;
	*ygsm = *ysm;
	*zgsm = *zsm * geopack1_7.cps - *xsm * geopack1_7.sps;
    } else {
	*xsm = *xgsm * geopack1_7.cps - *zgsm * geopack1_7.sps;
	*ysm = *ygsm;
	*zsm = *xgsm * geopack1_7.sps + *zgsm * geopack1_7.cps;
    }
    return 0;
} /* smgsm_ */


/* ========================================================================================== */

/* Subroutine */ int geogsm_(real *xgeo, real *ygeo, real *zgeo, real *xgsm, 
	real *ygsm, real *zgsm, integer *j)
{

/* CONVERTS GEOGRAPHIC (GEO) TO GEOCENTRIC SOLAR MAGNETOSPHERIC (GSM) COORDINATES */
/*   OR VICA VERSA. */

/*                   J>0                   J<0 */
/* ----- INPUT:  J,XGEO,YGEO,ZGEO    J,XGSM,YGSM,ZGSM */
/* ---- OUTPUT:    XGSM,YGSM,ZGSM      XGEO,YGEO,ZGEO */

/*  ATTENTION:  SUBROUTINE  RECALC  MUST BE INVOKED BEFORE GEOGSM IN TWO CASES: */
/*     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES */
/*     /B/  IF THE VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC  HAVE BEEN CHANGED */

/*     LAST MODIFICATION: MARCH 31, 2003 */

/*     AUTHOR:  N. A. TSYGANENKO */


    if (*j > 0) {
	*xgsm = geopack1_8.a11 * *xgeo + geopack1_8.a12 * *ygeo + 
		geopack1_8.a13 * *zgeo;
	*ygsm = geopack1_8.a21 * *xgeo + geopack1_8.a22 * *ygeo + 
		geopack1_8.a23 * *zgeo;
	*zgsm = geopack1_8.a31 * *xgeo + geopack1_8.a32 * *ygeo + 
		geopack1_8.a33 * *zgeo;
    } else {
	*xgeo = geopack1_8.a11 * *xgsm + geopack1_8.a21 * *ygsm + 
		geopack1_8.a31 * *zgsm;
	*ygeo = geopack1_8.a12 * *xgsm + geopack1_8.a22 * *ygsm + 
		geopack1_8.a32 * *zgsm;
	*zgeo = geopack1_8.a13 * *xgsm + geopack1_8.a23 * *ygsm + 
		geopack1_8.a33 * *zgsm;
    }
    return 0;
} /* geogsm_ */


/* ===================================================================================== */

/* Subroutine */ int rhand_(real *x, real *y, real *z__, real *r1, real *r2, 
	real *r3, integer *iopt, real *parmod, S_fp exname, S_fp inname__)
{
    /* System generated locals */
    real r__1, r__2, r__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real b, bx, by, bz, bxgsm, bygsm, bzgsm, hxgsm, hygsm, hzgsm;


/*  CALCULATES THE COMPONENTS OF THE RIGHT HAND SIDE VECTOR IN THE GEOMAGNETIC FIELD */
/*    LINE EQUATION  (a subsidiary subroutine for the subroutine STEP) */

/*     LAST MODIFICATION:  MARCH 31, 2003 */

/*     AUTHOR:  N. A. TSYGANENKO */


/*     EXNAME AND INNAME ARE NAMES OF SUBROUTINES FOR THE EXTERNAL AND INTERNAL */
/*     PARTS OF THE TOTAL FIELD */

    /* Parameter adjustments */
    --parmod;

    /* Function Body */
    (*exname)(iopt, &parmod[1], &geopack1_9.psi, x, y, z__, &bxgsm, &bygsm, &
	    bzgsm);
    (*inname__)(x, y, z__, &hxgsm, &hygsm, &hzgsm);
    bx = bxgsm + hxgsm;
    by = bygsm + hygsm;
    bz = bzgsm + hzgsm;
/* Computing 2nd power */
    r__1 = bx;
/* Computing 2nd power */
    r__2 = by;
/* Computing 2nd power */
    r__3 = bz;
    b = geopack1_9.ds3 / sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);
    *r1 = bx * b;
    *r2 = by * b;
    *r3 = bz * b;
    return 0;
} /* rhand_ */


/* =================================================================================== */

/* Subroutine */ int step_(real *x, real *y, real *z__, real *ds, real *errin,
	 integer *iopt, real *parmod, S_fp exname, S_fp inname__)
{
    /* System generated locals */
    real r__1, r__2, r__3;

    /* Local variables */
    static real r11, r12, r13, r21, r22, r23, r31, r32, r33, r41, r42, r43, 
	    r51, r52, r53;
    extern /* Subroutine */ int rhand_(real *, real *, real *, real *, real *,
	     real *, integer *, real *, S_fp, S_fp);
    static real errcur;


/*   RE-CALCULATES {X,Y,Z}, MAKING A STEP ALONG A FIELD LINE. */
/*   DS IS THE STEP SIZE, ERRIN IS PERMISSIBLE ERROR VALUE, IOPT SPECIFIES THE EXTERNAL */
/*   MODEL VERSION, THE ARRAY PARMOD CONTAINS INPUT PARAMETERS FOR THAT MODEL */
/*   EXNAME IS THE NAME OF THE EXTERNAL FIELD SUBROUTINE */
/*   INNAME IS THE NAME OF THE INTERNAL FIELD SUBROUTINE (EITHER DIP OR IGRF) */

/*   ALL THE PARAMETERS ARE INPUT ONES; OUTPUT IS THE RENEWED TRIPLET X,Y,Z */

/*     LAST MODIFICATION:  MARCH 31, 2003 */

/*     AUTHOR:  N. A. TSYGANENKO */

    /* Parameter adjustments */
    --parmod;

    /* Function Body */
L1:
    geopack1_10.ds3 = -(*ds) / 3.f;
    rhand_(x, y, z__, &r11, &r12, &r13, iopt, &parmod[1], (S_fp)exname, (S_fp)
	    inname__);
    r__1 = *x + r11;
    r__2 = *y + r12;
    r__3 = *z__ + r13;
    rhand_(&r__1, &r__2, &r__3, &r21, &r22, &r23, iopt, &parmod[1], (S_fp)
	    exname, (S_fp)inname__);
    r__1 = *x + (r11 + r21) * .5f;
    r__2 = *y + (r12 + r22) * .5f;
    r__3 = *z__ + (r13 + r23) * .5f;
    rhand_(&r__1, &r__2, &r__3, &r31, &r32, &r33, iopt, &parmod[1], (S_fp)
	    exname, (S_fp)inname__);
    r__1 = *x + (r11 + r31 * 3.f) * .375f;
    r__2 = *y + (r12 + r32 * 3.f) * .375f;
    r__3 = *z__ + (r13 + r33 * 3.f) * .375f;
    rhand_(&r__1, &r__2, &r__3, &r41, &r42, &r43, iopt, &parmod[1], (S_fp)
	    exname, (S_fp)inname__);
    r__1 = *x + (r11 - r31 * 3.f + r41 * 4.f) * 1.5f;
    r__2 = *y + (r12 - r32 * 3.f + r42 * 4.f) * 1.5f;
    r__3 = *z__ + (r13 - r33 * 3.f + r43 * 4.f) * 1.5f;
    rhand_(&r__1, &r__2, &r__3, &r51, &r52, &r53, iopt, &parmod[1], (S_fp)
	    exname, (S_fp)inname__);
    errcur = (r__1 = r11 - r31 * 4.5f + r41 * 4.f - r51 * .5f, dabs(r__1)) + (
	    r__2 = r12 - r32 * 4.5f + r42 * 4.f - r52 * .5f, dabs(r__2)) + (
	    r__3 = r13 - r33 * 4.5f + r43 * 4.f - r53 * .5f, dabs(r__3));
    if (errcur < *errin) {
	goto L2;
    }
    *ds *= .5f;
    goto L1;
L2:
    *x += (r11 + r41 * 4.f + r51) * .5f;
    *y += (r12 + r42 * 4.f + r52) * .5f;
    *z__ += (r13 + r43 * 4.f + r53) * .5f;
    if (errcur < *errin * .04f && dabs(*ds) < 1.33f) {
	*ds *= 1.5f;
    }
    return 0;
} /* step_ */


/* ============================================================================== */

/* Subroutine */ int trace_(real *xi, real *yi, real *zi, real *dir, real *
	rlim, real *r0, integer *iopt, real *parmod, S_fp exname, S_fp 
	inname__, integer *nobd, real *xf, real *yf, real *zf, real *xx, real 
	*yy, real *zz, integer *l)
{
    /* System generated locals */
    real r__1, r__2, r__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real r__, x, y, z__, r1, r2, r3, ad, fc, al, ds;
    static integer nn;
    static real rr, xr, yr, zr, err, ryz;
    extern /* Subroutine */ int step_(real *, real *, real *, real *, real *, 
	    integer *, real *, S_fp, S_fp), rhand_(real *, real *, real *, 
	    real *, real *, real *, integer *, real *, S_fp, S_fp);


/*  TRACES A FIELD LINE FROM AN ARBITRARY POINT OF SPACE TO THE EARTH'S */
/*  SURFACE OR TO A MODEL LIMITING BOUNDARY. */

/*  THE HIGHEST ORDER OF SPHERICAL HARMONICS IN THE MAIN FIELD EXPANSION USED */
/*  IN THE MAPPING IS CALCULATED AUTOMATICALLY. IF INNAME=IGRF_GSM, THEN AN IGRF MODEL */
/*  FIELD WILL BE USED, AND IF INNAME=DIP, A PURE DIPOLE FIELD WILL BE USED. */
/*  IN ANY CASE, BEFORE CALLING TRACE, ONE SHOULD INVOKE RECALC, TO CALCULATE CORRECT */
/*  VALUES OF THE IGRF COEFFICIENTS AND ALL QUANTITIES NEEDED FOR TRANSFORMATIONS */
/*  BETWEEN COORDINATE SYSTEMS INVOLVED IN THIS CALCULATIONS. */

/*  ALTERNATIVELY, THE SUBROUTINE RECALC CAN BE INVOKED WITH THE DESIRED VALUES OF */
/*  IYEAR AND IDAY (TO SPECIFY THE DIPOLE MOMENT), WHILE THE VALUES OF THE DIPOLE */
/*  TILT ANGLE PSI (IN RADIANS) AND ITS SINE (SPS) AND COSINE (CPS) CAN BE EXPLICITLY */
/*  SPECIFIED AND FORWARDED TO THE COMMON BLOCK GEOPACK1 (11th, 12th, AND 16th ELEMENTS, RESP.) */

/* ------------- INPUT PARAMETERS: */

/*   XI,YI,ZI - GSM COORDS OF INITIAL POINT (IN EARTH RADII, 1 RE = 6371.2 km), */

/*   DIR - SIGN OF THE TRACING DIRECTION: IF DIR=1.0 THEN WE MOVE ANTIPARALLEL TO THE */
/*     FIELD VECTOR (E.G. FROM NORTHERN TO SOUTHERN CONJUGATE POINT), */
/*     AND IF DIR=-1.0 THEN THE TRACING GOES IN THE OPPOSITE DIRECTION. */

/*   R0 -  RADIUS OF A SPHERE (IN RE) FOR WHICH THE FIELD LINE ENDPOINT COORDINATES */
/*     XF,YF,ZF  SHOULD BE CALCULATED. */

/*   RLIM - UPPER LIMIT OF THE GEOCENTRIC DISTANCE, WHERE THE TRACING IS TERMINATED. */

/*   IOPT - A MODEL INDEX; CAN BE USED FOR SPECIFYING AN OPTION OF THE EXTERNAL FIELD */
/*       MODEL (E.G., INTERVAL OF THE KP-INDEX). ALTERNATIVELY, ONE CAN USE THE ARRAY */
/*       PARMOD FOR THAT PURPOSE (SEE BELOW); IN THAT CASE IOPT IS JUST A DUMMY PARAMETER. */

/*   PARMOD -  A 10-ELEMENT ARRAY CONTAINING MODEL PARAMETERS, NEEDED FOR A UNIQUE */
/*      SPECIFICATION OF THE EXTERNAL FIELD. THE CONCRETE MEANING OF THE COMPONENTS */
/*      OF PARMOD DEPENDS ON A SPECIFIC VERSION OF THE EXTERNAL FIELD MODEL. */

/*   EXNAME - NAME OF A SUBROUTINE PROVIDING COMPONENTS OF THE EXTERNAL MAGNETIC FIELD */
/*    (E.G., T96_01). */
/*   INNAME - NAME OF A SUBROUTINE PROVIDING COMPONENTS OF THE INTERNAL MAGNETIC FIELD */
/*    (EITHER DIP OR IGRF_GSM). */

/* -------------- OUTPUT PARAMETERS: */

/*   XF,YF,ZF - GSM COORDS OF THE LAST CALCULATED POINT OF A FIELD LINE */
/*   XX,YY,ZZ - ARRAYS, CONTAINING COORDS OF FIELD LINE POINTS. HERE THEIR MAXIMAL LENGTH WAS */
/*      ASSUMED EQUAL TO 999. */
/*   L - ACTUAL NUMBER OF THE CALCULATED FIELD LINE POINTS. IF L EXCEEDS 999, TRACING */
/*     TERMINATES, AND A WARNING IS DISPLAYED. */


/*     LAST MODIFICATION:  MARCH 31, 2003. */

/*     AUTHOR:  N. A. TSYGANENKO */


    /* Parameter adjustments */
    --parmod;
    --zz;
    --yy;
    --xx;

    /* Function Body */
    nn = *l;
    err = 1e-4f;
    *l = 0;
    ds = *dir * .5f;
    x = *xi;
    y = *yi;
    z__ = *zi;
    geopack1_11.dd = *dir;
    al = 0.f;

/*  here we call RHAND just to find out the sign of the radial component of the field */
/*   vector, and to determine the initial direction of the tracing (i.e., either away */
/*   or towards Earth): */

    rhand_(&x, &y, &z__, &r1, &r2, &r3, iopt, &parmod[1], (S_fp)exname, (S_fp)
	    inname__);
    ad = .01f;
    if (x * r1 + y * r2 + z__ * r3 < 0.f) {
	ad = -.01f;
    }

/*     |AD|=0.01 and its sign follows the rule: */
/* (1) if DIR=1 (tracing antiparallel to B vector) then the sign of AD is the same as of Br */
/* (2) if DIR=-1 (tracing parallel to B vector) then the sign of AD is opposite to that of Br */
/*     AD is defined in order to initialize the value of RR (radial distance at previous step): */
/* Computing 2nd power */
    r__1 = x;
/* Computing 2nd power */
    r__2 = y;
/* Computing 2nd power */
    r__3 = z__;
    rr = sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3) + ad;
L1:
    ++(*l);
    if (*l > nn - 1) {
	goto L7;
    }
    xx[*l] = x;
    yy[*l] = y;
    zz[*l] = z__;
/* Computing 2nd power */
    r__1 = y;
/* Computing 2nd power */
    r__2 = z__;
    ryz = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = x;
    r2 = r__1 * r__1 + ryz;
    r__ = sqrt(r2);
/*  check if the line hit the outer tracing boundary; if yes, then terminate */
/*   the tracing (label 8): */
    if (*nobd == 0 && (r__ > *rlim || ryz > 1600. || x > 20.)) {
	goto L8;
    }

/*  check whether or not the inner tracing boundary was crossed from outside, */
/*  if yes, then calculate the footpoint position by interpolation (go to label 6): */

    if (r__ < *r0 && rr > r__) {
	goto L6;
    }
/*  check if (i) we are moving outward, or (ii) we are still sufficiently */
/*    far from Earth (beyond R=5Re); if yes, proceed further: */

    if (r__ >= rr || r__ > 5.f) {
	goto L5;
    }
/*  now we moved closer inward (between R=3 and R=5); go to 3 and begin logging */
/*  previous values of X,Y,Z, to be used in the interpolation (after having */
/*  crossed the inner tracing boundary): */
    if (r__ >= 3.f) {
	goto L3;
    }

/*  we entered inside the sphere R=3: to avoid too large steps (and hence inaccurate */
/*  interpolated position of the footpoint), enforce the progressively smaller */
/*  stepsize values as we approach the inner boundary R=R0: */

    fc = .2f;
    if (r__ - *r0 < .05f) {
	fc = .05f;
    }
    al = fc * (r__ - *r0 + .2f);
    ds = *dir * al;
    goto L4;
L3:
    ds = *dir;
L4:
    xr = x;
    yr = y;
    zr = z__;
L5:
    rr = r__;
    step_(&x, &y, &z__, &ds, &err, iopt, &parmod[1], (S_fp)exname, (S_fp)
	    inname__);
    goto L1;

/*  find the footpoint position by interpolating between the current and previous */
/*   field line points: */

L6:
    r1 = (*r0 - r__) / (rr - r__);
    x -= (x - xr) * r1;
    y -= (y - yr) * r1;
    z__ -= (z__ - zr) * r1;
    goto L8;
/*  7   WRITE (*,10)     ! Commented out for IDL Geopack DLM */
L7:
    *l = nn - 1;
/* Label moved for IDL Geopack DLM */
L8:
    ++(*l);
    xx[*l] = x;
    yy[*l] = y;
    zz[*l] = z__;
    *xf = x;
    *yf = y;
    *zf = z__;
    return 0;
/* L10: */
} /* trace_ */


/* ==================================================================================== */

/* Subroutine */ int shuetal_mgnp__(real *xn_pd__, real *vel, real *bzimf, 
	real *xgsm, real *ygsm, real *zgsm, real *xmgnp, real *ymgnp, real *
	zmgnp, real *dist, integer *id)
{
    /* System generated locals */
    real r__1, r__2, r__3;
    doublereal d__1, d__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), tanh(doublereal), f2c_pow_dd(doublereal 
	    *, doublereal *), log(doublereal), sqrt(doublereal), sin(
	    doublereal), cos(doublereal);
    integer f2c_s_wsle(cilist *), f2c_do_lio(integer *, integer *, char *, ftnlen), 
	    f2c_e_wsle(void);

    /* Local variables */
    static real f, r__, t, r0, pd, dr, ct, dt, ds, rm, st;
    static integer id96;
    static real phi, rho;
    static integer nit;
    static real rho2, xmt96, ymt96, zmt96, gradf, alpha, gradf_r__, gradf_t__;
    extern /* Subroutine */ int t96_mgnp__(real *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___252 = { 0, 6, 0, 0, 0 };



/*  FOR ANY POINT OF SPACE WITH COORDINATES (XGSM,YGSM,ZGSM) AND SPECIFIED CONDITIONS */
/*  IN THE INCOMING SOLAR WIND, THIS SUBROUTINE: */

/* (1) DETERMINES IF THE POINT (XGSM,YGSM,ZGSM) LIES INSIDE OR OUTSIDE THE */
/*      MODEL MAGNETOPAUSE OF SHUE ET AL. (JGR-A, V.103, P. 17691, 1998). */

/* (2) CALCULATES THE GSM POSITION OF A POINT {XMGNP,YMGNP,ZMGNP}, LYING AT THE MODEL */
/*      MAGNETOPAUSE AND ASYMPTOTICALLY TENDING TO THE NEAREST BOUNDARY POINT WITH */
/*      RESPECT TO THE OBSERVATION POINT {XGSM,YGSM,ZGSM}, AS IT APPROACHES THE MAGNETO- */
/*      PAUSE. */

/*  INPUT: XN_PD - EITHER SOLAR WIND PROTON NUMBER DENSITY (PER C.C.) (IF VEL>0) */
/*                    OR THE SOLAR WIND RAM PRESSURE IN NANOPASCALS   (IF VEL<0) */
/*         BZIMF - IMF BZ IN NANOTESLAS */

/*         VEL - EITHER SOLAR WIND VELOCITY (KM/SEC) */
/*                  OR ANY NEGATIVE NUMBER, WHICH INDICATES THAT XN_PD STANDS */
/*                     FOR THE SOLAR WIND PRESSURE, RATHER THAN FOR THE DENSITY */

/*         XGSM,YGSM,ZGSM - GSM POSITION OF THE OBSERVATION POINT IN EARTH RADII */

/*  OUTPUT: XMGNP,YMGNP,ZMGNP - GSM POSITION OF THE BOUNDARY POINT */
/*          DIST - DISTANCE (IN RE) BETWEEN THE OBSERVATION POINT (XGSM,YGSM,ZGSM) */
/*                 AND THE MODEL NAGNETOPAUSE */
/*          ID -  POSITION FLAG:  ID=+1 (-1) MEANS THAT THE OBSERVATION POINT */
/*          LIES INSIDE (OUTSIDE) OF THE MODEL MAGNETOPAUSE, RESPECTIVELY. */

/*  OTHER SUBROUTINES USED: T96_MGNP */

/*          AUTHOR:  N.A. TSYGANENKO, */
/*          DATE:    APRIL 4, 2003. */

    if (*vel < 0.f) {
	pd = *xn_pd__;
    } else {
/* Computing 2nd power */
	r__1 = *vel;
	pd = *xn_pd__ * 1.94e-6f * (r__1 * r__1);
/* PD IS THE SOLAR WIND DYNAMIC PRESSURE */
    }

/*  DEFINE THE ANGLE PHI, MEASURED DUSKWARD FROM THE NOON-MIDNIGHT MERIDIAN PLANE; */
/*  IF THE OBSERVATION POINT LIES ON THE X AXIS, THE ANGLE PHI CANNOT BE UNIQUELY */
/*  DEFINED, AND WE SET IT AT ZERO: */

    if (*ygsm != 0.f || *zgsm != 0.f) {
	phi = atan2(*ygsm, *zgsm);
    } else {
	phi = 0.f;
    }

/*  FIRST, FIND OUT IF THE OBSERVATION POINT LIES INSIDE THE SHUE ET AL BDRY */
/*  AND SET THE VALUE OF THE ID FLAG: */

    *id = -1;
    d__1 = (doublereal) pd;
    r0 = (tanh((*bzimf + 8.14f) * .184f) * 1.29f + 10.22f) * f2c_pow_dd(&d__1, &
	    c_b81);
    alpha = (.58f - *bzimf * .007f) * (log(pd) * .024f + 1.f);
/* Computing 2nd power */
    r__1 = *xgsm;
/* Computing 2nd power */
    r__2 = *ygsm;
/* Computing 2nd power */
    r__3 = *zgsm;
    r__ = sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);
    d__1 = (doublereal) (2.f / (*xgsm / r__ + 1.f));
    d__2 = (doublereal) alpha;
    rm = r0 * f2c_pow_dd(&d__1, &d__2);
    if (r__ <= rm) {
	*id = 1;
    }

/*  NOW, FIND THE CORRESPONDING T96 MAGNETOPAUSE POSITION, TO BE USED AS */
/*  A STARTING APPROXIMATION IN THE SEARCH OF A CORRESPONDING SHUE ET AL. */
/*  BOUNDARY POINT: */

    t96_mgnp__(&pd, &c_b82, xgsm, ygsm, zgsm, &xmt96, &ymt96, &zmt96, dist, &
	    id96);

/* Computing 2nd power */
    r__1 = ymt96;
/* Computing 2nd power */
    r__2 = zmt96;
    rho2 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = xmt96;
    r__ = sqrt(rho2 + r__1 * r__1);
    st = sqrt(rho2) / r__;
    ct = xmt96 / r__;

/*  NOW, USE NEWTON'S ITERATIVE METHOD TO FIND THE NEAREST POINT AT THE */
/*   SHUE ET AL.'S BOUNDARY: */

    nit = 0;
L1:
    t = atan2(st, ct);
    d__1 = (doublereal) (2.f / (ct + 1.f));
    d__2 = (doublereal) alpha;
    rm = r0 * f2c_pow_dd(&d__1, &d__2);
    f = r__ - rm;
    gradf_r__ = 1.f;
    gradf_t__ = -alpha / r__ * rm * st / (ct + 1.f);
/* Computing 2nd power */
    r__1 = gradf_r__;
/* Computing 2nd power */
    r__2 = gradf_t__;
    gradf = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = gradf;
    dr = -f / (r__1 * r__1);
    dt = dr / r__ * gradf_t__;
    r__ += dr;
    t += dt;
    st = sin(t);
    ct = cos(t);
/* Computing 2nd power */
    r__1 = dr;
/* Computing 2nd power */
    r__2 = r__ * dt;
    ds = sqrt(r__1 * r__1 + r__2 * r__2);
    ++nit;
    if (nit > 1000) {
	f2c_s_wsle(&io___252);
	f2c_do_lio(&c__9, &c__1, " BOUNDARY POINT COULD NOT BE FOUND; ITERATIONS"
		" DO NOT CONVERGE", (ftnlen)62);
	f2c_e_wsle();
    }
    if (ds > 1e-4f) {
	goto L1;
    }
    *xmgnp = r__ * cos(t);
    rho = r__ * sin(t);
    *ymgnp = rho * sin(phi);
    *zmgnp = rho * cos(phi);
/* Computing 2nd power */
    r__1 = *xgsm - *xmgnp;
/* Computing 2nd power */
    r__2 = *ygsm - *ymgnp;
/* Computing 2nd power */
    r__3 = *zgsm - *zmgnp;
    *dist = sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);
    return 0;
} /* shuetal_mgnp__ */


/* ======================================================================================= */

/* Subroutine */ int t96_mgnp__(real *xn_pd__, real *vel, real *xgsm, real *
	ygsm, real *zgsm, real *xmgnp, real *ymgnp, real *zmgnp, real *dist, 
	integer *id)
{
    /* System generated locals */
    real r__1, r__2, r__3;
    doublereal d__1;

    /* Builtin functions */
    double f2c_pow_dd(doublereal *, doublereal *), atan2(doublereal, doublereal), 
	    sqrt(doublereal), sin(doublereal), cos(doublereal);

    /* Local variables */
    static real a, a0, s0, x0, s00, pd, x00, xm, sq1, sq2, arg, phi, rat, rho,
	     tau, rat16, xksi, xdzt, sigma, rhomgnp;


/*  FOR ANY POINT OF SPACE WITH GIVEN COORDINATES (XGSM,YGSM,ZGSM), THIS SUBROUTINE DEFINES */
/*  THE POSITION OF A POINT (XMGNP,YMGNP,ZMGNP) AT THE T96 MODEL MAGNETOPAUSE, HAVING THE */
/*  SAME VALUE OF THE ELLIPSOIDAL TAU-COORDINATE, AND THE DISTANCE BETWEEN THEM.  THIS IS */
/*  NOT THE SHORTEST DISTANCE D_MIN TO THE BOUNDARY, BUT DIST ASYMPTOTICALLY TENDS TO D_MIN, */
/*  AS THE OBSERVATION POINT GETS CLOSER TO THE MAGNETOPAUSE. */

/*  INPUT: XN_PD - EITHER SOLAR WIND PROTON NUMBER DENSITY (PER C.C.) (IF VEL>0) */
/*                    OR THE SOLAR WIND RAM PRESSURE IN NANOPASCALS   (IF VEL<0) */
/*         VEL - EITHER SOLAR WIND VELOCITY (KM/SEC) */
/*                  OR ANY NEGATIVE NUMBER, WHICH INDICATES THAT XN_PD STANDS */
/*                     FOR THE SOLAR WIND PRESSURE, RATHER THAN FOR THE DENSITY */

/*         XGSM,YGSM,ZGSM - COORDINATES OF THE OBSERVATION POINT IN EARTH RADII */

/*  OUTPUT: XMGNP,YMGNP,ZMGNP - GSM POSITION OF THE BOUNDARY POINT, HAVING THE SAME */
/*          VALUE OF TAU-COORDINATE AS THE OBSERVATION POINT (XGSM,YGSM,ZGSM) */
/*          DIST -  THE DISTANCE BETWEEN THE TWO POINTS, IN RE, */
/*          ID -    POSITION FLAG; ID=+1 (-1) MEANS THAT THE POINT (XGSM,YGSM,ZGSM) */
/*          LIES INSIDE (OUTSIDE) THE MODEL MAGNETOPAUSE, RESPECTIVELY. */

/*  THE PRESSURE-DEPENDENT MAGNETOPAUSE IS THAT USED IN THE T96_01 MODEL */
/*  (TSYGANENKO, JGR, V.100, P.5599, 1995; ESA SP-389, P.181, OCT. 1996) */

/*   AUTHOR:  N.A. TSYGANENKO */
/*   DATE:    AUG.1, 1995, REVISED APRIL 3, 2003. */


/*  DEFINE SOLAR WIND DYNAMIC PRESSURE (NANOPASCALS, ASSUMING 4% OF ALPHA-PARTICLES), */
/*   IF NOT EXPLICITLY SPECIFIED IN THE INPUT: */
    if (*vel < 0.f) {
	pd = *xn_pd__;
    } else {
/* Computing 2nd power */
	r__1 = *vel;
	pd = *xn_pd__ * 1.94e-6f * (r__1 * r__1);

    }

/*  RATIO OF PD TO THE AVERAGE PRESSURE, ASSUMED EQUAL TO 2 nPa: */
    rat = pd / 2.f;
    d__1 = (doublereal) rat;
    rat16 = f2c_pow_dd(&d__1, &c_b88);
/* (THE POWER INDEX 0.14 IN THE SCALING FACTOR IS THE BEST-FIT VALUE OBTAINED FROM DATA */
/*    AND USED IN THE T96_01 VERSION) */

/*  VALUES OF THE MAGNETOPAUSE PARAMETERS FOR  PD = 2 nPa: */

    a0 = 70.f;
    s00 = 1.08f;
    x00 = 5.48f;

/*   VALUES OF THE MAGNETOPAUSE PARAMETERS, SCALED BY THE ACTUAL PRESSURE: */

    a = a0 / rat16;
    s0 = s00;
    x0 = x00 / rat16;
    xm = x0 - a;

/*  (XM IS THE X-COORDINATE OF THE "SEAM" BETWEEN THE ELLIPSOID AND THE CYLINDER) */

/*     (FOR DETAILS ON THE ELLIPSOIDAL COORDINATES, SEE THE PAPER: */
/*      N.A.TSYGANENKO, SOLUTION OF CHAPMAN-FERRARO PROBLEM FOR AN */
/*      ELLIPSOIDAL MAGNETOPAUSE, PLANET.SPACE SCI., V.37, P.1037, 1989). */

    if (*ygsm != 0.f || *zgsm != 0.f) {
	phi = atan2(*ygsm, *zgsm);
    } else {
	phi = 0.f;
    }

/* Computing 2nd power */
    r__1 = *ygsm;
/* Computing 2nd power */
    r__2 = *zgsm;
    rho = sqrt(r__1 * r__1 + r__2 * r__2);

    if (*xgsm < xm) {
	*xmgnp = *xgsm;
/* Computing 2nd power */
	r__1 = s0;
	rhomgnp = a * sqrt(r__1 * r__1 - 1);
	*ymgnp = rhomgnp * sin(phi);
	*zmgnp = rhomgnp * cos(phi);
/* Computing 2nd power */
	r__1 = *xgsm - *xmgnp;
/* Computing 2nd power */
	r__2 = *ygsm - *ymgnp;
/* Computing 2nd power */
	r__3 = *zgsm - *zmgnp;
	*dist = sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);
	if (rhomgnp > rho) {
	    *id = 1;
	}
	if (rhomgnp <= rho) {
	    *id = -1;
	}
	return 0;
    }

    xksi = (*xgsm - x0) / a + 1.f;
    xdzt = rho / a;
/* Computing 2nd power */
    r__1 = xksi + 1.f;
/* Computing 2nd power */
    r__2 = xdzt;
    sq1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = 1.f - xksi;
/* Computing 2nd power */
    r__2 = xdzt;
    sq2 = sqrt(r__1 * r__1 + r__2 * r__2);
    sigma = (sq1 + sq2) * .5f;
    tau = (sq1 - sq2) * .5f;

/*  NOW CALCULATE (X,Y,Z) FOR THE CLOSEST POINT AT THE MAGNETOPAUSE */

    *xmgnp = x0 - a * (1.f - s0 * tau);
/* Computing 2nd power */
    r__1 = s0;
/* Computing 2nd power */
    r__2 = tau;
    arg = (r__1 * r__1 - 1.f) * (1.f - r__2 * r__2);
    if (arg < 0.f) {
	arg = 0.f;
    }
    rhomgnp = a * sqrt(arg);
    *ymgnp = rhomgnp * sin(phi);
    *zmgnp = rhomgnp * cos(phi);

/*  NOW CALCULATE THE DISTANCE BETWEEN THE POINTS {XGSM,YGSM,ZGSM} AND {XMGNP,YMGNP,ZMGNP}: */
/*   (IN GENERAL, THIS IS NOT THE SHORTEST DISTANCE D_MIN, BUT DIST ASYMPTOTICALLY TENDS */
/*    TO D_MIN, AS WE ARE GETTING CLOSER TO THE MAGNETOPAUSE): */

/* Computing 2nd power */
    r__1 = *xgsm - *xmgnp;
/* Computing 2nd power */
    r__2 = *ygsm - *ymgnp;
/* Computing 2nd power */
    r__3 = *zgsm - *zmgnp;
    *dist = sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);

    if (sigma > s0) {
	*id = -1;
    }
/*  ID=-1 MEANS THAT THE POINT LIES OUTSID */
    if (sigma <= s0) {
	*id = 1;
    }
/*                                           THE MAGNETOSPHERE */
/*  ID=+1 MEANS THAT THE POINT LIES INSIDE */
    return 0;
} /* t96_mgnp__ */

