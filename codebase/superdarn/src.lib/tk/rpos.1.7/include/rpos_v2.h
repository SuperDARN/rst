/* rpos_v2.h
   =========
   Author: E.G.Thomas
*/

#ifndef _RPOS_V2_H
#define _RPOS_V2_H

void RPosGeo_v2(int center, int bcrd, int rcrd, struct RadarSite *pos,
                int frang, int rsep, int rxrise, double height,
                double *rho, double *lat, double *lng,
                int model, int gs, int rear);

double calc_standard_vhm(double r, double fh);
double calc_chisham_vhm(double r, double *hop);
double calc_cv_vhm(double r, int gs, double *hop);

double calc_virtual_height(double r, double elv, double hop);
double calc_elevation_angle(double r, double xh, double hop);
double calc_coning_angle(struct RadarSite *pos, int bmnum, double elv,
                         int center, int rear);
double calc_ground_range(double r, double xh, double hop);

void gc_destination(double lat1, double lon1, double d, double azi,
                    double *lat2, double *lon2);
#endif
