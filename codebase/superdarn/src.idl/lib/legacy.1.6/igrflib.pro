; igrflib.pro
; ===========
; Author: K.Baker & R.J.Barnes
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
; along with this program.  If not, see <https://www.gnu.org/licenses/>.
; 
; 
; Modifications:
; 
; Public Functions
; ----------------
; magcmp
; igrfcall
;
; ---------------------------------------------------------------




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;           magcmp
;
; PURPOSE:  
;
;           use IGRF magnetic field models to return magnetic field
;	    components.
;
; CALLING SEQUENCE:
;
;	B = magcmp(lat,lon,elev,date)
;	     where date is given in decimal years
;	     lat, lon are the geographic latitude and longitude in degrees
;	     elev is the elevation.
;
;-----------------------------------------------------------------------
;

function magcmp,lat,lon,elev,date,ifrst
  bvec=dblarr(3,n_elements(lat))
  s=IGRFModelMagCmp(date,elev,lat,lon,x,y,z)
  bvec[0,*]=x
  bvec[1,*]=y
  bvec[2,*]=z
  return,bvec
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME: 
;
;           igrfcall
;
; PURPOSE:  
;
;           use IGRF magnetic field models to return magnetic field
;	    components.
;
; CALLING SEQUENCE:
;
;	B = igrfcall(date,glat,glong,alt)
;	     where date is given in decimal years
;	     glat, glong are the geographic latitude and longitude in degrees
;	     alt is the altitude in km.
;
;-----------------------------------------------------------------------
;

function igrfcall,date,glat,glong,alt
  if (n_params() NE 4) then begin
    print,"Invalid number of parameters in call to IGRFCALL"
    print,"USE: B=igrfcall(date,glat,glong,altitude)"
    return,[0,0,0]
  endif
	
  temp_date = date

  if (temp_date LT 1900) then temp_date = temp_date + 1900   
  igrf_year = double(temp_date)
  flat = double(glat)
  flong = double(glong)
  height = double(alt)
 
  s=IGRFModelCall(igrf_year,flat,flong,height,x,y,z)
  B=[x,y,z]
  return,B
end
