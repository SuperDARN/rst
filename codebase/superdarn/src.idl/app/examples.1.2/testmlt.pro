; testmlt.pro
; ============
; Author: R.J.Barnes
; 
; LICENSE AND DISCLAIMER
; 
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
; 
; This file is part of the Radar Software Toolkit (RST).
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU Lesser General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; any later version.
; 
; RST is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU Lesser General Public License for more details.
; 
; You should have received a copy of the GNU Lesser General Public License
; along with RST.  If not, see <http://www.gnu.org/licenses/>.
; 
; 
; 
;
; A Simply demonstration program for testing the MLT library


pro testmlt



  yrsec=TimeYMDHMSToYrSec(2002,08,30,10,30,0)

  yrarr=fltarr(10);
  yrarr[*]=yrsec
  yrarr[5:9]+=3600

  mlt=MLTConvertYrsec(2002,yrarr,0.0)

  print, 'MLT=',mlt

end


