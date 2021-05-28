/* grplotstd.h
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



#ifndef _GRPLOTSTD_H
#define _GRPLOTSTD_H

int GrplotStdKey(struct Plot *ptr,
               float ox,float oy,float wdt,float hgt,
               double min,double max,double step,
               int or,int txt_or,int tflg,
               int ttl_num,char *ttl_text,
               int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *a,
               unsigned char *r,unsigned char *g,unsigned char *b);

int GrplotStdKeyLog(struct Plot *ptr,
               float ox,float oy,float wdt,float hgt,
	       double min,double max,double tick_major,double tick_minor,
	       int or,int txt_or,int tflg, 
               int ttl_num,char *ttl_text,
	       int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
	       int ksze,unsigned char *a,
               unsigned char *r,unsigned char *g,unsigned char *b);


#endif
