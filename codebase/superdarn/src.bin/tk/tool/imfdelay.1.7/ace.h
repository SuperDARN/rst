/* ace.h
   =====
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




int aceswe_pos(CDFid id,struct posdata *ptr,double stime,double etime);  
int aceswe_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime);
int acemfi_imf(CDFid id,struct imfdata *ptr,double stime,double etime,int cnv);
 
