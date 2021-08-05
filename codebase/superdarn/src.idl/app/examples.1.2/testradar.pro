; testradar.pro
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
; A simple demonstration program for converting radar coordinates

pro testradar


; Open the data table, this should be defined in the environment
; variable SD_RADAR

  fname=getenv('SD_RADAR')
  openr,inp,fname,/get_lun,/STDIO

; Load the data tables

  network=RadarLoad(inp)
  free_lun,inp

; Load up the hardware data, the environment variable SD_HDWPATH
; should point to the directory containing the hardware files


  s=RadarLoadHardware(network,path=getenv('SD_HDWPATH'))

; Find the radar information about the radar with ID code 3

  r=RadarGetRadar(network,3)


; Get the hardware information for the date in 1998

  site=RadarYMDHMSGetSite(r,1998,6,1,0,0,0)
  
; do the transform

  lat=0.0D
  lon=0.0D
  s=RadarPos(0,8,10,site,180,45,0,300.0,rho,lat,lon)

  rngarr=intarr(75,16)
  bmarr=intarr(75,16)

  print, lat,lon

  for bm=0,15 do begin
     rngarr(*,bm)=indgen(75)
     bmarr(*,bm)=bm
  endfor

  rho=fltarr(75,16)
  lat=fltarr(75,16)
  lon=fltarr(75,16)
  
  s=RadarPos(0,bmarr,rngarr,site,180,45,0,300.0,rho,lat,lon)
  
  print, rho,lat,lon

  stop
  

end
