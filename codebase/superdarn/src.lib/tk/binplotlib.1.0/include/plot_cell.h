/* plot_cell.h
   ===========
   Author: R.J.Barnes
*/


/*
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/


#ifndef _PLOTCELL_H
#define _PLOTCELL_H

void plot_field_cell(struct Plot *plot,struct RadarBeam *sbm,
                     struct GeoLocBeam *gbm,float latmin,int magflg,
                     float xoff,float yoff,float wdt,float hgt,
                     int (*trnf)(int,void *,int,void *,void *data),void *data,
                     unsigned int(*cfn)(double,void *),void *cdata,
                     int prm,unsigned int gscol,unsigned char gsflg);

void plot_grid_cell(struct Plot *plot,struct GridData *ptr,float latmin,int magflg,
                    float xoff,float yoff,float wdt,float hgt,
                    int (*trnf)(int,void *,int,void *,void *data),void *data,
                    unsigned int(*cfn)(double,void *),void *cdata,int cprm,
                    int old_aacgm);

#endif
