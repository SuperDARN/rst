; aacgmtest.pro
; =============
; Author: R.J.Barnes
;
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
; 
; This file is part of the Radar Software Toolkit (RST).
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program. If not, see <https://www.gnu.org/licenses/>.
; 
; Modifications:
;
;
;
; A simple demonstration program for testing the AACGM library

pro testaacgm


; Open a set of coefficients and load them.
; 
; NOTE: This step can be left out as the AACGM routines will default
; to the year 2000 coefficients

;  openr,unit,'2000.dat',/get_lun,/stdio
;  c=AACGMLoadCoef(unit)
;  free_lun,unit

  glat=85.5
  glon=70.5
  hgt=300.0

  mlat=0.0
  mlon=0.0
  r=0.0

  s=AACGMConvert(glat,glon,hgt,mlat,mlon,r)
  print, 'Geomagnetic Coordinates=',mlat,mlon

  s=AACGMConvert(mlat,mlon,hgt,glat,glon,r,/geo)
  print, 'Inverse Geographic Coordinates=',glat,glon

  yrsec=TimeYMDHMSToYrSec(2002,08,30,10,30,0)

  yrarr=fltarr(10);
  yrarr[*]=yrsec
  yrarr[5:9]+=3600

  mlt=MLTConvertYrsec(2002,yrarr,0.0)

  print, 'MLT=',mlt

end


