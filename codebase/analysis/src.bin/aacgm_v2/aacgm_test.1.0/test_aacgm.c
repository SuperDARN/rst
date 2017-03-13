#include <stdio.h>
#include "aacgmlib_v2.h"
#include "mlt_v2.h"

#define CR printf("\n")
#define DEBUG 1

void next(void);
void line(char ch);

int main(void)
{
double lat,lon,hgt;
double h, mlt_c, mlt_t;
double rtp[3];
double mlat,mlon,r;
int k, err, npts;
int yr, mo, dy, hr, mt, sc, dno;

line('=');
printf("  AACGM-v2 Test Program\n");
line('=');

/* compute AACGM-v2 lat/lon with no time specified */
line('=');
printf("TEST: no date/time (this WILL and SHOULD return an error.)\n");
line('=');
lat = 45.5;
lon = -23.5;
hgt = 1135.;
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A);
if (err == 0) {
	printf("lat = %lf, lon = %lf, height = %lf\n", lat,lon,hgt);
	printf("mlat = %lf, mlon = %lf, r = %lf\n", mlat,mlon,r);
	printf("\n\n");
}

#if (DEBUG > 1)
next();

printf("TEST: Setting time to : %04d%02d%02d %02d%02d:%02d  (will fail)\n", 1850,1,22,0,0,0);
AACGM_v2_SetDateTime(1850, 1, 22, 0,0,0);	/* this should fail */
printf("TEST: Setting time to : %04d%02d%02d %02d%02d:%02d\n", 1900,1,22,0,0,0);
AACGM_v2_SetDateTime(1900, 1, 22, 0,0,0);	/* this is valid */
printf("TEST: Setting time to : %04d%02d%02d %02d%02d:%02d  (will fail)\n", 2020,1,22,0,0,0);
AACGM_v2_SetDateTime(2020, 1, 22, 0,0,0);	/* this shoudl fail */
printf("TEST: Setting time to : %04d%02d%02d %02d%02d:%02d\n", 2019,1,22,0,0,0);
AACGM_v2_SetDateTime(2019, 1, 22, 0,0,0);	/* this is valid */
next();
#endif

yr = 2014;
mo = 3;
dy = 22;
hr = 3;
mt = 11;
sc = 0;
line('=');
printf("TEST: Setting time to : %04d%02d%02d %02d%02d:%02d\n",
					yr, mo, dy, hr, mt, sc);
line('=');
CR;

/* set date and time */
AACGM_v2_SetDateTime(yr, mo, dy, hr, mt, sc);
/* get date and time just set */
AACGM_v2_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sc,&dno);
printf("Expected output: 20140322 0311:00  81\n");
printf("Actual   output: %04d%02d%02d %02d%02d:%02d %3d\n",
                         yr,mo,dy,hr,mt,sc,dno);
CR; CR;

lat = 45.5;
lon = -23.5;
hgt = 1135.;

line('=');
printf("TEST: geographic to AACGM-v2\n");
line('=');
/* compute AACGM lat/lon */
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A);

printf("         "
       " GLAT       GLON        HEIGHT       MLAT       MLON       R\n");
printf("Expected "
       " 45.500000  -23.500000  1135.000000  48.189618  57.763454  1.177533\n");
printf("Actual    %lf  %lf  %lf  %lf  %lf  %lf\n", lat,lon,hgt, mlat,mlon,r);
CR; CR;
#if (DEBUG > 1)
next();
#endif

line('=');
printf("TEST: AACGM-v2 to geographic\n");
line('=');
/* do the inverse: A2G */
hgt = (r-1.)*RE;
err = AACGM_v2_Convert(mlat,mlon,hgt, &lat,&lon, &h, A2G);

printf("         "
       " MLAT       MLON        HEIGHT       GLAT       GLON       HEIGHT\n");
printf("Expected "
       " 48.189618  57.763454  1131.097495  45.440775  -23.472757 "
       " 1134.977896\n");
printf("Actual    %lf  %lf  %lf  %lf  %lf  %lf\n", mlat,mlon,hgt, lat,lon,h);
CR; CR;
#if (DEBUG > 1)
next();
#endif

/* same thing but using field-line tracing */
lat = 45.5;
lon = -23.5;
hgt = 1135.;

printf("Do the same thing but use field-line tracing\n");
line('=');
printf("TEST: geographic to AACGM-v2 (TRACE)\n");
line('=');
/* compute AACGM lat/lon */
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A|TRACE);

printf("         "
       " GLAT       GLON        HEIGHT       MLAT       MLON       R\n");
printf("Expected  "
       "45.500000  -23.500000  1135.000000  48.194757  57.758831  1.177533\n");
printf("Actual    %lf  %lf  %lf  %lf  %lf  %lf", lat,lon,hgt, mlat,mlon,r);
CR; CR;
#if (DEBUG > 1)
next();
#endif

line('=');
printf("TEST: AACGM-v2 to geographic (TRACE)\n");
line('=');
/* do the inverse: A2G */
hgt = (r-1.)*RE;
err = AACGM_v2_Convert(mlat,mlon,hgt, &lat,&lon, &h, A2G|TRACE);

printf("         "
       " MLAT       MLON        HEIGHT       GLAT       GLON       HEIGHT\n");
printf("Expected "
       " 48.194757  57.758831  1131.097495  45.500000  -23.500000 "
       " 1135.000000\n");
printf("Actual    %lf  %lf  %lf  %lf  %lf  %lf\n", mlat,mlon,hgt, lat,lon,h);
CR; CR;
#if (DEBUG > 1)
next();
#endif

/* compare tracing to coefficients */
lat = 45.5;
lon = -23.5;
hgt = 150.;

/* set date and time */
AACGM_v2_SetDateTime(2018,1,1,0,0,0);

#if (DEBUG >1)
printf("TEST: geographic to AACGM-v2; coefficients\n");
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A);
printf("lat = %lf, lon = %lf, height = %lf\n", lat,lon,hgt);
printf("mlat = %lf, mlon = %lf, r = %lf\n", mlat,mlon,r);
next();
#endif

#if (DEBUG > 1)
printf("TEST: geographic to AACGM-v2; tracing\n");
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A|TRACE);
printf("lat = %lf, lon = %lf, height = %lf\n", lat,lon,hgt);
printf("mlat = %lf, mlon = %lf, r = %lf\n", mlat,mlon,r);
next();
#endif

#if (DEBUG > 1)
printf("TEST: geographic to AACGM-v2; too high\n");
hgt = 2500;
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A);
printf("lat = %lf, lon = %lf, height = %lf\n", lat,lon,hgt);
printf("mlat = %lf, mlon = %lf, r = %lf\n", mlat,mlon,r);
next();
#endif

#if (DEBUG > 1)
printf("TEST: geographic to AACGM-v2; trace high\n");
hgt = 7500;
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A|TRACE);
printf("lat = %lf, lon = %lf, height = %lf\n", lat,lon,hgt);
printf("mlat = %lf, mlon = %lf, r = %lf\n", mlat,mlon,r);
next();
#endif

#if (DEBUG > 1)
printf("TEST: geographic to AACGM-v2; coefficient high\n");
hgt = 7500;
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A|BADIDEA);
printf("lat = %lf, lon = %lf, height = %lf\n", lat,lon,hgt);
printf("mlat = %lf, mlon = %lf, r = %lf\n", mlat,mlon,r);
next();
#endif

line('=');
printf("TEST: MLT   (T: field-line tracing) (C: coefficients)\n");
line('=');

lat = 77.;
lat = 37.;
lon = -88.;
hgt = 300.;

yr = 2003;
mo = 5;
dy = 17;
hr = 7;
mt = 53;
sc = 16;

/* compute AACGM lat/lon */
AACGM_v2_SetDateTime(yr, mo, dy, hr, mt, sc);
err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A|TRACE);
mlt_t = MLTConvertYMDHMS_v2(yr,mo,dy,hr,mt,sc,mlon);
printf("           "
       "GLAT       GLON        HEIGHT       MLAT       MLON       MLT\n");
printf("Expected T"
       " 37.000000  -88.000000  300.000000  48.840368  -17.006090  1.977745\n");
printf("Actual   T %lf  %lf  %lf  %lf  %lf  %lf\n",
       lat,lon,hgt, mlat,mlon,mlt_t);

err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A);
mlt_c = MLTConvertYMDHMS_v2(yr,mo,dy,hr,mt,sc,mlon);
printf("Expected C"
       " 37.000000  -88.000000  300.000000  48.844355  -16.999464  1.978187\n");
printf("Actual   C %lf  %lf  %lf  %lf  %lf  %lf\n",
       lat,lon,hgt, mlat,mlon,mlt_c);
CR; CR;

line('=');
printf("TEST: Array\n");
line('=');
printf("           "
       "GLAT       GLON        HEIGHT       MLAT       MLON       MLT\n");
printf("Expected   45.0000   0.0000    150.0000     40.2841     76.6676      8.2227\n");
printf("Expected   45.0000   1.0000    150.0000     40.2447     77.4899      8.2775\n");
printf("Expected   45.0000   2.0000    150.0000     40.2108     78.3157      8.3325\n");
printf("Expected   45.0000   3.0000    150.0000     40.1822     79.1452      8.3878\n");
printf("Expected   45.0000   4.0000    150.0000     40.1587     79.9785      8.4434\n");
printf("Expected   45.0000   5.0000    150.0000     40.1400     80.8157      8.4992\n");
printf("Expected   45.0000   6.0000    150.0000     40.1261     81.6569      8.5553\n");
printf("Expected   45.0000   7.0000    150.0000     40.1165     82.5020      8.6116\n");
printf("Expected   45.0000   8.0000    150.0000     40.1111     83.3513      8.6682\n");
printf("Expected   45.0000   9.0000    150.0000     40.1097     84.2048      8.7251\n");
CR;
printf("           "
       "GLAT       GLON        HEIGHT       MLAT       MLON       MLT\n");
npts = 10;
for (k=0; k<npts; k++) {
	lat = 45.;
	lon = k;
	hgt = 150.;

	err = AACGM_v2_Convert(lat,lon,hgt, &mlat,&mlon, &r, G2A|TRACE);
	mlt_t = MLTConvertYMDHMS_v2(yr,mo,dy,hr,mt,sc,mlon);
	printf("Actual     %7.4lf %8.4lf  %10.4lf  %10.4lf  %10.4lf  %10.4lf\n",
								lat,lon,hgt, mlat,mlon,mlt_t);
}
CR; CR;

printf("Check Expected values against Actual values, or\n\n"
       "test_aacgm > some_file.txt\n"
       "diff some_file.txt expected.txt\n\n");

return (0);
}

void next(void)
{
	char ch;

	printf("Press Enter to continue ");
	do {
		scanf("%c", &ch);
	} while (ch != '\n');

	printf("\f");
}

void line(char ch)
{
	int k,n;

	n = 80;
	for (k=0;k<n;k++) printf("%c", ch);
	printf("\n");
}

