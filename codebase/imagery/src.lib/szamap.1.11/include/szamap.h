/* szamap.c
   ========
   Author: R.J.Barnes
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



int SZATransform(int ssze,void *src,int dsze,void *dst,void *data);

struct PolygonData **SZAContour(int yr,int mo,int dy,int hr,int mt,int sc,
                                 int flg,int mode,float step,int znum,
                                double *zenith);

struct PolygonData *SZATerminator(int yr,int mo,int dy,int hr,int mt,int sc,
                                    int flg,int mode,float step,
				  float zenith);

float *SZAMap(int yr,int mo,int dy,int hr,int mt,int sc,float latmin,
               int wdt,int hgt,int mode,
               int (*trf)(int ssze,void *src,int dsze, void *dst,void *data),
	      void *data);
