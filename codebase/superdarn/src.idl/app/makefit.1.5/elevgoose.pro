; elev_goose.pro
; ==============
; Author: R.J.Barnes - Based on C code by K.Baker
; 
;
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
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
; Public Functions
; ----------------
;
; FitACFElevGoose


; ---------------------------------------------------------------




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFElevGoose
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        angle=FitACFElevGoose(prm,range,phi0)
;
;-----------------------------------------------------------------
;




function FitACFElevGoose,prm,range,phi0

  C=2.997d8

  if (not finite(phi0)) then return, -!VALUES.D_INFINITY

; The coordinate system is: +x is in front of the main array,
; +y is to the left of the center, and +z is upward.   
;
; The angle xi is in the x-y plane.  The angle is measured counter-clockwise
; from the x axis.  If the y offset and x offset are both positive, the
; angle is positive.
;
; Sep is the distance between the centers of the two arrays
;
; If the z offset is not zero, then the elevation angle has to be corrected.
; An apparent elevation angle of 0 would actually being pointing slightly
; upward (downward) if the z offset is positive (negative).

  xi = atan2(prm.interfer[0],(prm.interfer[1]))

  sep = sqrt(prm.interfer[0]*prm.interfer[0] + $
	     prm.interfer[1]*prm.interfer[1] + $
	     prm.interfer[2]*prm.interfer[2])

  elev_corr = atan2(prm.interfer[2], $
		    sqrt(prm.interfer[0]*prm.interfer[0] + $
		    prm.interfer[1]*prm.interfer[1]))
  cos_xi = cos(xi)
  sin_xi = sin(xi)
  
  cos_xi2 = cos(xi)*cos(xi)
  sin_xi2 = sin(xi)*sin(xi)
  

; compute phasing matrix cone angle 
  offset=0.0d
  offset=(prm.maxbeam/2.0d)-0.5d

  psi = prm.bmsep*(prm.bmnum-offset)*!DPI/180.0d	


; compute wavenumber

  k = 2.0d * !DPI * prm.tfreq * 1000.0d/C	

  dchi_cable = - 2* !DPI * prm.tfreq * 1000.0d * prm.tdiff * 1.0d-6

; compute the minimum cone angle (alpha) 


  temp = sin(psi) + sin_xi
  sin_psi_xi = sin(psi)*sin_xi

  sn2_alpha_min = (temp*temp)/(1.0 + sin_psi_xi)
  cs_alpha_min = sqrt(1.0 - sn2_alpha_min)

; now iterate sn2_alpha_min, cs_alpha_min to improve value of alpha_min 

  sin_psi2 = sin(psi)*sin(psi)
  sin_psi2_xi2 = sin_psi2 + sin_xi2

  sn2_alpha_old = sn2_alpha_min

  sn2_alpha_min=sin_psi2_xi2 + 2.0*cs_alpha_min*sin_psi_xi
  while( abs(sn2_alpha_min - sn2_alpha_old) gt (0.005*sn2_alpha_old)) do begin
	cs_alpha_min = sqrt(1.0 - sn2_alpha_min)
	sn2_alpha_old = sn2_alpha_min
         sn2_alpha_min=sin_psi2_xi2 + 2.0*cs_alpha_min*sin_psi_xi
  endwhile


  cs_alpha_min = sqrt(1.0 - sn2_alpha_min)

; we've now got the sin & cos of alpha_min 
; compute the total phase difference 

  dchi_sep_max = k * sep / cos_xi * cs_alpha_min
  dchi_max = dchi_cable + dchi_sep_max
  n = 0.5 - dchi_max/(2*!DPI)

  dchi = phi0 - n*(2*!DPI) ; this should be the true phase difference 

  if (dchi gt dchi_max) then dchi = dchi - (2*!DPI)
  if (dchi lt (dchi_max - (2*!DPI))) then dchi = dchi + (2*!DPI)

; compute the cone angle (alpha)

  dchi_old = 0.0d
  while (abs(dchi_old - dchi) gt !DPI) do begin

    cs_alpha = (dchi - dchi_cable)/(k*sep)*cos_xi
    sn2_eps = 1.0 - (cs_alpha*cs_alpha)/(cos_xi2) - (sin_psi2/cos_xi2) $
              - 2.0*cs_alpha*sin_psi_xi/cos_xi2
    dchi_old = dchi

    if ((abs(sn2_eps) gt 1.0) or (sn2_eps lt 0.0)) then begin
      dchi = dchi - (2*!DPI);
      print,'changing dchi by -2pi.',dchi_old,dchi
    endif
  endwhile

  sn_eps = sqrt(sn2_eps)
  elev = asin(sn_eps)

; The calculated elevation angle is actually with respect to the plane
; that includes the two antenna arrays.  This has to be corrected for the
; difference in elevation between the front array and the back array.

  elev = elev + elev_corr
  return, 180.0d*elev/!DPI

end
