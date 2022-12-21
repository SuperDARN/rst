/* geobeam.h
   =========
Copyright (C) <year>  <name of author>

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
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/

#ifndef _GEOBEAM_H
#define _GEOBEAM_H

struct GeoLocBeam {
  int bm;
  int frang;
  int rsep;
  int rxrise;
  int nrang;
  float *glat[3];
  float *glon[3];
  float *mlat[3];
  float *mlon[3];
  float *gazm;
  float *mazm;
};

struct GeoLocScan {
  int num;
  struct GeoLocBeam *bm;
};
  
int GeoLocCenter(struct RadarSite *site,int mag,float *lat,float *lon,
                 int chisham,int old_aacgm);
int GeoLocBeam(struct RadarSite *site,int year,
               struct GeoLocScan *geol,struct RadarBeam *bm,
               int chisham,int old_aacgm);

#endif
