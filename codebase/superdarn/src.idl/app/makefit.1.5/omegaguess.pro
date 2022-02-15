; omegaguess.pro
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
; FitACFOmegaGuess 


; ---------------------------------------------------------------





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFOmegaGuess
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        omega=FitACFOmegaGuess(acf,tau,badlag,phi_res,omega_err,mpinc,mplgs)
;
;
;
;
;-----------------------------------------------------------------
;




function FitACFOmegaGuess,acf,tau,badlag,phi_res,omega_err,mpinc,mplgs

  nave=0
  tau_lim=1.0D
  omega=0.0d
  omega2=0.0d
  sum_W=0.0d
  omega_err=9999.0D
  two_sigma=2*!DPI
  sigma=2*!DPI
  average=0.0D
   while ((tau_lim lt 3) and (nave lt 3)) do begin
     
     for j=1,tau_lim do begin
       for i=0,mplgs-1-j do begin
         if ((badlag[i+j] eq 0) and (badlag[i] eq 0)) then begin
         
           delta_tau=tau[i+j]-tau[i]
           if (delta_tau eq tau_lim) then begin
             delta_phi=phi_res[i+j]-phi_res[i]
             W=(abs(acf[i])+abs(acf[i+j]))/2.0D
             W=W*W
           
             if (delta_phi gt !DPI) then delta_phi=delta_phi - 2*!DPI
             if (delta_phi lt -!DPI) then delta_phi=delta_phi + 2*!DPI
             
             if ((average ne 0) and $
                 (abs(delta_phi-average) gt two_sigma)) then dummy=0.0 $
             else begin
               temp = delta_phi/tau_lim
	       omega = omega + temp*W
	       omega2 = omega2 + W*(temp*temp)
	       sum_W = sum_W + W
	       nave=nave+1              
             endelse
           endif
         endif
       endfor
   endfor

   if ((nave ge 3) and (sigma eq 2*!DPI)) then begin
     average = omega/sum_W
     sigma = ((omega2/sum_W) - average*average)/(nave-1)

     if (sigma gt 0.0D) then sigma=sqrt(sigma) $
     else sigma=0.0D 
     two_sigma = 2.0D*sigma
     omega = 0.0D
     omega2 = 0.0D 
     sum_W = 0.0D
     nave = 0
     tau_lim = 1
   endif else begin
     if (nave ge 3) then begin
       omega = omega/sum_W
       omega = omega/(mpinc*1.0e-6)
       sigma = ((omega2/sum_W) - average*average)/(nave-1)
       if (sigma gt 0.0D) then sigma=sqrt(sigma) $
       else sigma=0.0D 
       omega_err = sigma/(mpinc*1.0e-6)
       return,omega
     endif else tau_lim=tau_lim+1
   endelse
  endwhile

  return, 0.0
end
