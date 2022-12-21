/* plot_logo.h
   =========== 
   Author: R.J.Barnes
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

#ifndef _PLOTLOGO_H
#define _PLOTLOGO_H

void plot_logo(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
               unsigned int color,unsigned char mask, char *fontname,
               float fontsize, void *txtdata);

void plot_aacgm(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
              unsigned int color,unsigned char mask, char *fontname,
              float fontsize, void *txtdata, int old);

void plot_web(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
              unsigned int color,unsigned char mask, char *fontname,
              float fontsize, void *txtdata);

void plot_credit(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
                 unsigned int color,unsigned char mask, char *fontname,
                 float fontsize, void *txtdata);

#endif
