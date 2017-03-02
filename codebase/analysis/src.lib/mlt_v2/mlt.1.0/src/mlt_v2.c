
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aacgmlib_v2.h"
#include "rtime.h"
#include "astalg.h"


/* Changes

   Combined MLTAst and MLTAst1

   Removed *mlson variable since it is only used for doing linear
     interpolation between two periods separated by 10 minutes.
*/


double mlon_ref = -1;

struct {
  int yr;
  int mo;
  int dy;
  int hr;
  int mt;
  int sc;
} mlt_date = {-1,-1,-1,-1,-1,-1};

/*
 * Accepts scalars but only recomputes mlon_ref if time has changed from last
 * call so computation is fast(er) for multiple calls at same date/time
 *
 * No options here. Use IDL version for development
 *
 */
double MLTConvert_v2(int yr, int mo, int dy, int hr, int mt ,int sc,
											double mlon)
{
	int err;
	double dd,jd,eqt,dec,ut,at;
	double slon,mlat,r;
	double hgt,aacgm_mlt;

/* check for bad input, which can come from undefined region, and return NAN */
	if (!isfinite(mlon)) {
		return (NAN);
	}

	hgt = 700.;		/* AACGM-v2 coefficients are defined everywhere above this
								 * altitude. */

	if (mlt_date.yr != yr || mlt_date.mo != mo || mlt_date.dy != dy ||
			mlt_date.hr != hr || mlt_date.mt != mt || mlt_date.sc != sc) {
		/* date/time has changed so recompute */
		mlt_date.yr = yr;
		mlt_date.mo = mo;
		mlt_date.dy = dy;
		mlt_date.hr = hr;
		mlt_date.mt = mt;
		mlt_date.sc = sc;

		/* compute corrected time */
  	dd  = AstAlg_dday(dy,hr,mt,sc);
  	jd  = AstAlg_jde(yr,mo,dd);
  	eqt = AstAlg_equation_of_time(jd);
  	dec = AstAlg_solar_declination(jd);
  	ut  = hr*3600. + mt*60. + sc;
  	at  = ut + eqt*60.;

		/* comparision with IDL version is exact
		printf("\n\n");
		printf("dd  = %20.12lf\n", dd);
		printf("jd  = %20.12lf\n", jd);
		printf("eqt = %20.12lf\n", eqt);
		printf("dec = %20.12lf\n", dec);
		printf("\n\n");
		*/

		/* compute reference longitude */
		slon = (43200.-at)*15./3600.;					/* subsolar point  */

		/* compute AACGM-v2 coordinates of reference point */
		err = AACGM_v2_Convert(dec, slon, hgt, &mlat, &mlon_ref, &r, G2A);

		/* check for error: this should NOT happen... */
		if (err != 0) {
			err = -99;
			return (NAN);
		}
	}
/*printf("** %lf\n", mlon_ref);*/

	aacgm_mlt = 12. + (mlon - mlon_ref)/15.;	/* MLT based on subsolar point */

	/* comparision with IDL version is exact */

	while (aacgm_mlt > 24.) aacgm_mlt -= 24.;
	while (aacgm_mlt <  0.) aacgm_mlt += 24.;

  return (aacgm_mlt);
}

/* inverse function: MLT to AACGM-v2 magnetic longitude */
double inv_MLTConvert_v2(int yr, int mo, int dy, int hr, int mt ,int sc,
											double mlt)
{
	int err;
	double dd,jd,eqt,dec,ut,at;
	double slon,mlat,r;
	double hgt,aacgm_mlon;

/* check for bad input, which should not happen for MLT, and return NAN */
	if (!isfinite(mlt)) {
		return (NAN);
	}

	hgt = 700.;		/* AACGM-v2 coefficients are defined everywhere above this
								 * altitude. */

	if (mlt_date.yr != yr || mlt_date.mo != mo || mlt_date.dy != dy ||
			mlt_date.hr != hr || mlt_date.mt != mt || mlt_date.sc != sc) {
		/* date/time has changed so recompute */
		mlt_date.yr = yr;
		mlt_date.mo = mo;
		mlt_date.dy = dy;
		mlt_date.hr = hr;
		mlt_date.mt = mt;
		mlt_date.sc = sc;

		/* compute corrected time */
  	dd  = AstAlg_dday(dy,hr,mt,sc);
  	jd  = AstAlg_jde(yr,mo,dd);
  	eqt = AstAlg_equation_of_time(jd);
  	dec = AstAlg_solar_declination(jd);
  	ut  = hr*3600. + mt*60. + sc;
  	at  = ut + eqt*60.;

		/* compute reference longitude */
		slon = (43200.-at)*15./3600.;					/* subsolar point  */

		/* compute AACGM-v2 coordinates of reference point */
		err = AACGM_v2_Convert(dec, slon, hgt, &mlat, &mlon_ref, &r, G2A);

		/* check for error: this should NOT happen... */
		if (err != 0) {
			err = -99;
			return (NAN);
		}
	}

	aacgm_mlon = (mlt - 12.)*15. + mlon_ref;	/* mlon based on subsolar point */

	while (aacgm_mlon >  180.) aacgm_mlon -= 360.;
	while (aacgm_mlon < -180.) aacgm_mlon += 360.;

  return (aacgm_mlon);
}

double MLTConvertYMDHMS_v2(int yr, int mo, int dy,
                           int hr,int mt,int sc, double mlon)
{
	return (MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlon));
}

double inv_MLTConvertYMDHMS_v2(int yr, int mo, int dy,
                           int hr,int mt,int sc, double mlt)
{
	return (inv_MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlt));
}

double MLTConvertYrsec_v2(int yr,int yr_sec, double mlon)
{
	int mo,dy,hr,mt,sc;

	TimeYrsecToYMDHMS(yr_sec,yr,&mo,&dy,&hr,&mt,&sc);

  return (MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlon));
}

double inv_MLTConvertYrsec_v2(int yr,int yr_sec, double mlt)
{
	int mo,dy,hr,mt,sc;

	TimeYrsecToYMDHMS(yr_sec,yr,&mo,&dy,&hr,&mt,&sc);

  return (inv_MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlt));
}

double MLTConvertEpoch_v2(double epoch, double mlon)
{
	int yr,mo,dy,hr,mt;
	double sc;

	TimeEpochToYMDHMS(epoch,&yr,&mo,&dy,&hr,&mt,&sc);

	return (MLTConvert_v2(yr,mo,dy,hr,mt,(int)sc, mlon));
}

double inv_MLTConvertEpoch_v2(double epoch, double mlt)
{
	int yr,mo,dy,hr,mt;
	double sc;

	TimeEpochToYMDHMS(epoch,&yr,&mo,&dy,&hr,&mt,&sc);

	return (inv_MLTConvert_v2(yr,mo,dy,hr,mt,(int)sc, mlt));
}

