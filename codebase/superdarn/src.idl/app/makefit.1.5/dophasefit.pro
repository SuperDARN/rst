; dophasefit.pro
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
; FitACFDoPhaseFit


; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFDoPhaseFit
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        status=FitACFDoPhaseFit(omega_guess,xflag,mplgs,acf,tau,
;                                w,sum_wk2_arr, 
;                                phi_res, badlag,t0,sum_w,sum_wk,sum_wk2, 
;                                omega,phi0,sdev,phi0_err,omega_err)
;
;
;
;-----------------------------------------------------------------
;




function FitACFDoPhaseFit,omega_guess,xflag,mplgs,acf,tau,w,sum_wk2_arr, $
                         phi_res, badlag,t0,sum_w,sum_wk,sum_wk2, $
                         omega,phi0,sdev,phi0_err,omega_err

  common robc,flg

  LAG_SIZE=48

  omega_loc=0.0d
  omega_old=9999.0d
  omega_old_2= 9999.0d 
  omega=0.0d
  phi0=0.0d
  sdev=0.0d
  phi0_err=0.0d
  omega_err=0.0d
    
  phi_k=dblarr(LAG_SIZE) 

  icnt=0

  phi_loc = 0.0d

  omega_loc = omega_guess
  t2 = t0*t0

  while (abs(omega_old - omega_loc) gt abs(omega_loc * !DPI/64.0d)) do begin
    
; if omega_loc == omega_old_2 it means we are oscillating between
; two different values of omega 
    if ((icnt gt 0) and (omega_loc eq omega_old_2)) then begin
      omega=(omega_old+omega_loc)/2.0D
      phi0=phi_loc
      return, 16
    endif

; if icnt >= 5 it means we aren't converging on a stable value for
; omega

    icnt=icnt+1
  
    if (icnt ge 5) then begin
      omega=omega_loc
      phi0=phi_loc
      return,32
    endif
  
    omega_old_2 = omega_old
    omega_old = omega_loc
    omega_init = omega_loc

    if (xflag eq 0) then phi_loc = 0.0d

    sum_phi = atan(imaginary(acf[0]),double(acf[0]))
    sum_phi = sum_phi*w[0]*w[0];
    sum_kphi = 0.0d;
    n_twopi = 0;
    nphi = 0.0d;

; now go through the point, one at a time, predicting the new
; value for phi_tot from the current best value for omega 



  for k=1,mplgs-1 do begin
      if (badlag[k] ne 0) then continue
      phi_pred = phi_loc + omega_loc*tau[k]*t0
      sign=1
      if (omega_loc gt 0) then sign=0

;
; This code is to counter rounding errors and differences between
; the way the C math libraries and IDL work
;

      n_twopi=round((!DPI + phi_pred - phi_res[k])/(2*!DPI) - sign)
       
      phi_tot = phi_res[k] + n_twopi*(2.0d*!DPI)
      
      if (abs(phi_pred - phi_tot) gt !DPI) then begin
        if (phi_pred gt phi_tot) then phi_tot = phi_tot + 2*!DPI $
        else phi_tot = phi_tot - 2*!DPI;
      endif
   
      phi_k[k] = phi_tot
      sum_phi = sum_phi + phi_tot*w[k]*w[k]
      sum_kphi = sum_kphi + tau[k]*phi_tot*w[k]*w[k]
      nphi=nphi+1


; if this is the first time through the omega fit loop AND
; we are doing ACFs, NOT xcfs, and we've got enough points to
; draw a line, THEN compute a new value of omega_loc as we add each
; new point

      if ((xflag eq 0) and (sum_wk2_arr[k] ne 0) and $
          (omega_old_2 eq 9999.0d)) then begin

          omega_loc = sum_kphi/(t0*sum_wk2_arr[k])
	  omega_loc = (nphi*omega_loc + omega_init)/(nphi + 1)
        
      endif

    endfor

    if (xflag ne 0) then begin
      d = determ([[sum_w,sum_wk*t0],[sum_wk*t0,sum_wk2*t2]])
      if (d eq 0) then return, 8
      phi_loc = determ([[sum_phi,sum_wk*t0],[sum_kphi*t0,sum_wk2*t2]])/d
      omega_loc = determ([[sum_w,sum_phi],[sum_wk*t0,sum_kphi*t0]])/d
    endif else begin
      phi_loc = 0.0d
      if (sum_wk2 le 0.0) then return, 8
      omega_loc = sum_kphi/(t0*sum_wk2)
    endelse

  endwhile

  if (phi_loc gt !DPI) then phi_loc = phi_loc - 2*!DPI

  phi0 = phi_loc
  omega = omega_loc

; Now we calculate the estimated error of the fit 

  q=where(badlag[0:mplgs-1] eq 0,c) 

  wbar = 0.0d
  e2 = 0.0d
  nphi = c

  e2=total(w[q]*w[q]*(phi_k[q] - phi_loc - omega_loc*tau[q]*t0)* $
             (phi_k[q] - phi_loc - omega_loc*tau[q]*t0))

  wbar=total(w[q])

  wbar = wbar/nphi;

  if (xflag ne 0) then sdev = sqrt(e2/(sum_w)/(nphi-2)) $
  else sdev = sqrt(e2/sum_w/(nphi-1))

  if (xflag ne 0) then begin
    phi0_err =  sdev * wbar * sqrt(sum_wk2*t2/d)
    omega_err = sdev * wbar * sqrt(sum_w/d)
  endif else begin
    phi0_err = 0
    omega_err = sdev*wbar/sqrt(sum_wk2)/t0
  endelse

  return, 0
end
