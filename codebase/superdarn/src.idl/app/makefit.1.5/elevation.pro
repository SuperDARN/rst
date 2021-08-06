; elevation.pro
; =============
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
; FitACFElevation 


; ---------------------------------------------------------------




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFElevation
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        angle=FitACFElevation(prm,range,phi0)
;
;
;
;-----------------------------------------------------------------
;



function FitACFElevation,prm,range,phi0

  C=2.997d8

  if (not finite(phi0)) then return, -!VALUES.D_INFINITY

  antenna_separation=sqrt(prm.interfer[1]*prm.interfer[1] + $ 
			  prm.interfer[0]*prm.interfer[0] + $
	                  prm.interfer[2]*prm.interfer[2])

  elev_corr= prm.phidiff*asin(prm.interfer[2]/antenna_separation)

  if (prm.interfer[1] gt 0.0) then phi_sign=1.0d $ ; in front of main antenna 
  else begin

; interferometer behind main antenna */

    phi_sign= -1.0d
    elev_corr= -elev_corr
  endelse
  offset=0.0d
  offset=(prm.maxbeam/2.0d)-0.5d

  phi= prm.bmsep*(prm.bmnum - offset)* !DPI/ 180.0d;
  c_phi= cos( phi);
  k= 2 * !DPI * prm.tfreq * 1000.0d/C;

; the phase difference phi0 is between -pi and +pi and gets positive,  
; if the signal from the interferometer antenna arrives earlier at the 
; receiver than the signal from the main antenna. 
; If the cable to the interferometer is shorter than the one to 
; the main antenna, than the signal from the interferometer     
; antenna arrives earlier. tdiff < 0  --> dchi_cable > 0        

  dchi_cable= - 2* !DPI * prm.tfreq * 1000.0d * prm.tdiff * 1.0d-6

; If the interferometer antenna is in front of the main antenna 
; then lower elevation angles correspond to earlier arrival     
; and greater phase difference.     
; If the interferometer antenna is behind of the main antenna   
; then lower elevation angles correspond to later arrival       
; and smaller phase difference     

  chi_max= phi_sign* k* antenna_separation* c_phi + dchi_cable

; change phi0 by multiples of twopi, until it is in the range   
; (chi_max - twopi) to chi_max (interferometer in front)       
; or chi_max to (chi_max + twopi) (interferometer in the back)  

  phi_temp= phi0 + 2*!DPI* floor( (chi_max - phi0)/ (2*!DPI))

  if (phi_sign lt 0.0) then phi_temp= phi_temp + (2*!DPI)

; subtract the cable effect 

  psi= phi_temp - dchi_cable
  theta= psi/ (k* antenna_separation)
  theta= (c_phi* c_phi - theta* theta)
 
; set elevation angle to 0 for out of range values

  if ( (theta lt 0.0) or (abs( theta) gt 1.0) ) then theta= -elev_corr $
  else theta= asin( sqrt( theta))

  return,180.0d* (theta + elev_corr)/ !DPI
end
