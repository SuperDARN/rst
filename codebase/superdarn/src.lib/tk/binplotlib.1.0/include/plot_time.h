/* plot_time.h
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


void plot_time(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,
               double time,
               unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata);

void plot_time_range(struct Plot *plot,
                     float xoff,float yoff,float wdt,float hgt,
                     double stime,double etime,
                     unsigned int color,unsigned char mask,
                     char *fontname,float fontsize,
                     void *txtdata);

void plot_field_time(struct Plot *plot,
                     float xoff,float yoff,float wdt,float hgt,int flg,
                     double stime,double etime,
                     unsigned int color,unsigned char mask,
                     char *fontname,float fontsize,
                     void *txtdata);

void plot_vec_time(struct Plot *plot,
                   float xoff,float yoff,float wdt,float hgt,
                   unsigned int color,unsigned char mask,
                   char *fontname,float fontsize,
                   void *txtdata);

