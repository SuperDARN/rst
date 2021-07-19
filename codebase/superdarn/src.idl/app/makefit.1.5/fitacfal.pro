; fitacfal.pro
; ============
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
; FitACFFitACF


; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFFitACF
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        omega=FitACFFitACF(acf,range,badlag,nbad,badsmp,lag_lim,prm,
;                           noise_lev_in, xflag,xomega,ptr)
;
;
;
;-----------------------------------------------------------------
;




function FitACFFitACF,acf,range,badlag,nbad,badsmp,lag_lim,prm,noise_lev_in, $
                      xflag,xomega,ptr

  

  common robc,flg
  flg=0

  HUGE_VAL=!VALUES.D_INFINITY
  LAG_SIZE=48

  ACF_UNMODIFIED=0
  ACF_ION_SCAT=1
  ACF_GROUND_SCAT=2
  ACF_MIXED_SCAT=3

  acf_stat=ACF_UNMODIFIED

  bad_pwr=intarr(LAG_SIZE)

  if (abs(acf[0]) lt noise_lev_in) then begin
    badlag[range,*]=3
    ptr[range].p_l=0.0
    ptr[range].p_s=0.0
    ptr[range].p_l_err=0.0
    ptr[range].p_s_err=0.0
    ptr[range].v=0.0
    ptr[range].v_err=0.0
    ptr[range].w_l=0.0
    ptr[range].w_s=0.0
    ptr[range].w_l_err=0.0
    ptr[range].w_s_err=0.0
    ptr[range].phi0=0.0
    ptr[range].phi0_err=0.0
    ptr[range].sdev_l=0.0
    ptr[range].sdev_s=0.0
    return, 2
  endif

  
  FitACFRngOverLap,prm.mppul,prm.mpinc,prm.smsep, $
                      prm.pulse,roverlap


  FitACFCkRng,range,prm.mplgs,prm.mpinc,prm.smsep,prm.lag, $
                        nbad,badsmp,bdlag

  FitACFLagOverLap,range,prm.nave,prm.nrang, $
                   prm.mppul,prm.mplgs,roverlap, $
                   prm.pwr0,prm.lag, $
                   prm.pulse,bdlag
 
  noise_lev=noise_lev_in

  tau=prm.lag[*,1]-prm.lag[*,0]
  tau2=tau^2
  w=abs(acf)-noise_lev
  
  q=where(w lt 0.1,c)
  w[q]=0.1


; we now have to compute the amount of power to subtract off the 
; power level at each range.  The amouont to be subtracted is P(0)/sqrt(nave)
; which is the approximate expectation value of the power level after the
; ACF has decorrelated. 

; [ To derive this, determine the expectation value of
; P**2 = R(tau)*conj(R(tau))]


  P0n=w[0]/sqrt(prm.nave)
  
; give up if left over pwr is too low

  if ((w[0]-P0n) lt noise_lev) then return, 2 
  
; identify any additional bad lags 

  sum_np = FitACFMoreBadLags(w,bdlag,noise_lev,prm.mplgs,prm.nave)

  badlag[range,*]=bdlag
  ptr[range].nump=sum_np

; We must have at least lag_lim good lags 

 
  if (sum_np lt lag_lim) then return,4

 

; this is required to make logs ok 

  if (noise_lev le 0.0) then noise_lev=0.1

; OK, now we have determined the good lags for the phase fit.  
; Now subtract of P0n from the power profile 

; calculate the power values for each lag.  'w' is the linear power.
; wt is the power times the lag.  wt2 is power times lag**2.  
; pwr is the log of the power. wp is the linear power times the log of
; the power.  The items that involve the linear power are all parts of 
; the least squares fits with the weighting done by the linear power. 

  w=w-P0n
  q=where(w le 0.0,c)
  w[q]=0.1

  wt=w^2*tau
  wt2=wt*tau
  pwr=alog(w)
  wp=w*w*pwr

; we now have to check to see how many additional bad lags have been
; introduced by subtracting off P0n.

  q=where((w lt noise_lev) and (bdlag eq 0),c)
  if (c gt 0) then bad_pwr[q]=1
  
  q=where((bdlag[0:prm.mplgs-1] eq 0) and $
          (bad_pwr[0:prm.mplgs-1] eq 0),npp)


; initialize the sums

  sum_np = 1
  sum_w = w[0]*w[0]
  sum_wk = 0.0d
  sum_wk2 = 0.0d
  sum_wk2_arr=dblarr(LAG_SIZE)
  sum_wk4 = 0.0d
  sum_p = w[0]*w[0]*pwr[0]
  sum_pk = 0.0d
  sum_pk2 = 0.0d
  phi_loc = atan(imaginary(acf[0]), double(acf[0]))
  sum_kphi = 0.0d
  t0 =  prm.mpinc * 1.0d-6
  t2 = t0 * t0
  t4 = t2 * t2


; calculate all the residual phases 
; if calc_phi_res returns a bad status abort the fit 

  if (FitACFCalcPhiRes(acf, bdlag, phi_res, prm.mplgs) ne 0) then return, 2

  if (xflag eq 0) then begin

    if (acf_stat eq ACF_GROUND_SCAT) then omega_loc = 0.0d $
    else omega_loc = FitACFOmegaGuess(acf, tau, bdlag, phi_res, $
				      omega_err_loc,prm.mpinc,prm.mplgs)
    
    sum_phi = 0.0D
  endif else begin
   
    sum_phi = phi_loc * w[0]*w[0]
    omega_loc = xomega
  endelse


; The preliminaries are now over.  
; Now start the fitting process 

; first, calculate the sums needed for the phase fit 

  for k=1,prm.mplgs-1 do begin 
    if (bdlag[k] ne 0) then  sum_wk2_arr[k] = sum_wk2_arr[k-1] $
    else begin
      sum_w = sum_w + w[k]*w[k]
      sum_np = sum_np + 1
      sum_wk = sum_wk + w[k]*w[k]*tau[k]
      sum_wk2 = sum_wk2 + wt2[k]
      sum_wk2_arr[k] = sum_wk2
    endelse
  endfor

  if (range eq 48) then flg=1 else flg=0
 
  status=FitACFDoPhaseFit(omega_loc,xflag, prm.mplgs, acf, tau, $
			 w, sum_wk2_arr, phi_res, bdlag, t0, $
			 sum_w, sum_wk, sum_wk2, $
			 omega_base, phi_loc, phase_sdev, $
			 phi_err, omega_err) 

   ptr[range].phi0=phi_loc
   ptr[range].v=omega_base
   ptr[range].sdev_phi=phase_sdev
   ptr[range].phi0_err=phi_err
   ptr[range].v_err=omega_err
 

; check the status of the phase fit to see if it was actually OK.  
; if not, set error bars to HUGE_VAL 


  if (status ne 0) then begin 
    ptr[range].sdev_phi = HUGE_VAL
    ptr[range].v_err = HUGE_VAL
    if (xflag ne 0) then ptr[range].phi0_err = HUGE_VAL
  endif

  
; OK, we now have our baseline value for omega.  Now re-do the
; phase fit, but using omega_loc + omega__err_loc. 

  if ((xflag eq 0) and (status eq 0)) then  begin
    status = FitACFDoPhaseFit(omega_loc + omega_err_loc, $
                              xflag, prm.mplgs, acf, tau, $
			      w, sum_wk2_arr, phi_res, bdlag, t0, $
			      sum_w, sum_wk, sum_wk2, $
			      omega_high, phi_loc, phase_sdev, $
			      phi_err, omega_err) 

    status = FitACFDoPhaseFit(omega_loc - omega_err_loc, $  
                              xflag, prm.mplgs, acf, tau, $
                              w, sum_wk2_arr, phi_res, bdlag, t0, $
                              sum_w, sum_wk, sum_wk2, $
			      omega_low, phi_loc, phase_sdev, $
			      phi_err, omega_err)

; if the difference between the high and low values of omega
; is greater than the error estimate of the original fit,
; we will use the original fit as our best guess for the
; velocity, but we'll set the error to be the difference between
; the high and low values.  Actually, at this point we should have
; non-symmetric error bar, but the file format has no provision 
; for that. 

    if (abs(omega_high - omega_low) ge (2*ptr[range].v_err)) then begin
      ptr[range].v = omega_base
      ptr[range].v_err = abs(omega_high - omega_low)
    endif

  endif

; POWER FITS:  We now turn to the power fits.  The sums have to be
; partially redone, since we have subtracted P0n.

; We are now faced with the question of what to do if we don't have enough 
; lags left to do a fit.  we can't abaondon the data because the phase fit is
; actually ok.  we have to have at least 3 points to do the fit and estimate 
; an error on the fit.

; If we don't have at least 3 good points, then simply set the lamda and
; sigma powers both to the power_lag0 level.  If there are only 2 good points
; then calculate the value of sigma and lamda, but set the error estimate
; to HUGE_VAL.

; If we end up with only lag-0 being good, then flag the width estimate
; by setting it to a large negative value.

  if (npp lt 3) then begin
 
    c_log=pwr[0]
 
; if c_log < 0 it means that after subtracting the noise and P0n,
; the result is less than 1.0.  This must really be pretty meaningless
; It shouldn't even be possible since we have supposedly already checked
; this at the beginning. 

  
    if (c_log lt 0) then return, 2
    
    ptr[range].p_l = c_log
    ptr[range].p_s = c_log

    q=where(bdlag[0:prm.mplgs-1] eq 0,c)

; if there are no good lags, or only lag-0 is good, set the width
; to a high negative value, by setting the last_good lag to 1
      
    if (c le 0) then begin
      ptr[range].w_l = -9999.0d
      ptr[range].w_s = -9999.0d
      ptr[range].p_l_err = HUGE_VAL
      ptr[range].p_s_err = HUGE_VAL
      ptr[range].w_l_err = HUGE_VAL
      ptr[range].w_s_err = HUGE_VAL
      ptr[range].sdev_l = HUGE_VAL
      ptr[range].sdev_s = HUGE_VAL 
    endif else begin

; now calculate the width as the lag-0 power divided by the
; time to the last good lag. 
    
      last_good=q[c-1]
      ptr[range].w_l = c_log/(tau[last_good]*t0)
      ptr[range].w_s = c_log/(tau2[last_good]*t2)

; set the errors to the maximum value

      ptr[range].p_l_err = HUGE_VAL
      ptr[range].p_s_err = HUGE_VAL
      ptr[range].w_l_err = HUGE_VAL
      ptr[range].w_s_err = HUGE_VAL
      ptr[range].sdev_l = HUGE_VAL
      ptr[range].sdev_s = HUGE_VAL
    endelse
  endif else begin

; Calculate the sums that were not used in the phase fit 

    for k=1,prm.mplgs-1 do begin 
      if ((bdlag[k] eq 0) and (bad_pwr[k] eq 0)) then begin
        sum_p = sum_p + wp[k]
        sum_pk = sum_pk + pwr[k]*wt[k]
        sum_pk2 = sum_pk2 + pwr[k]*wt2[k]
        sum_wk4 = sum_wk4 + wt2[k]*tau2[k]
      endif
    endfor

 ; Now adjust the sums that were used in the phase fit, but that
 ; have changed because of additional bad lags 

    for k=1,prm.mplgs-1 do begin 
      if (bad_pwr[k] ne 0) then begin
        sum_w = sum_w - w[k]*w[k]
        sum_np = sum_np - 1
        sum_wk = sum_wk - w[k]*w[k]*tau[k]
        sum_wk2 = sum_wk2 - wt2[k]
      endif
    endfor
 
;   start with the lamda fit 

    d = determ([[sum_w,-t0*sum_wk],[t0*sum_wk,-t2*sum_wk2]])
    c_log = determ([[sum_p,-t0*sum_wk],[t0*sum_pk,-t2*sum_wk2]])/d
    P0 = exp(c_log) + P0n
    c_log = alog(P0)

    ptr[range].p_l = c_log

    ptr[range].w_l = determ([[sum_w,sum_p],[t0*sum_wk,t0*sum_pk]])/d

    if (sum_np gt 3) then begin
       q=where((bdlag[0:prm.mplgs-1] eq 0) and (bad_pwr[0:prm.mplgs-1] eq 0),c)
       temp=pwr[q]-(c_log-tau[q]*t0*ptr[range].w_l)

       e2=total(w[q]*w[q]*temp*temp)
       wbar=total(w[q])
       npp=c
  
       wbar = wbar/npp
       ptr[range].sdev_l = sqrt(e2/sum_w/(npp - 2))

       if ((sum_w*sum_wk2 - sum_wk*sum_wk) le 0) then begin
         ptr[range].p_l_err = HUGE_VAL
         ptr[range].w_l_err = HUGE_VAL
         ptr[range].sdev_l = HUGE_VAL
       endif else begin
         c_log_err = ptr[range].sdev_l * wbar * $
                     sqrt(sum_wk2/(sum_w*sum_wk2 - sum_wk*sum_wk))
	 ptr[range].p_l_err = c_log_err
         ptr[range].w_l_err = ptr[range].sdev_l * wbar * $
		 sqrt(sum_w/(t2*(sum_w*sum_wk2 - $
                         sum_wk*sum_wk))) 
       endelse
    endif else begin
      ptr[range].p_l_err = HUGE_VAL
      ptr[range].w_l_err = HUGE_VAL
      ptr[range].sdev_l = HUGE_VAL
    endelse
    
    

; now do the sigma fit

    d = determ([[sum_w,-t2*sum_wk2],[t2*sum_wk2,-t4*sum_wk4]])
    c_log = determ([[sum_p,-t2*sum_wk2],[t2*sum_pk2,-t4*sum_wk4]])/d
    P0 = exp(c_log) + P0n
    c_log = alog(P0)
    ptr[range].p_s = c_log

    ptr[range].w_s = determ([[sum_w,sum_p],[t2*sum_wk2,t2*sum_pk2]])/d

    if (sum_np gt 3) then begin
       q=where((bdlag[0:prm.mplgs-1] eq 0) and (bad_pwr[0:prm.mplgs-1] eq 0),c)
       temp=pwr[q]-(c_log-tau2[q]*t2*ptr[range].w_s)

       e2=total(w[q]*w[q]*temp*temp)
       wbar=total(w[q])
       npp=c
  
       wbar = wbar/npp
       ptr[range].sdev_s = sqrt(e2/sum_w/(npp - 2))

       if ((sum_w*sum_wk4 - sum_wk2*sum_wk2) le 0) then begin
         ptr[range].p_s_err = HUGE_VAL
         ptr[range].w_s_err = HUGE_VAL
         ptr[range].sdev_s = HUGE_VAL
       endif else begin
         c_log_err = ptr[range].sdev_s * wbar * $
                     sqrt(sum_wk4/(sum_w*sum_wk4 - sum_wk2*sum_wk2))
	 ptr[range].p_s_err = c_log_err
         ptr[range].w_s_err = ptr[range].sdev_s * wbar * $
		 sqrt(sum_w/(t4*(sum_w*sum_wk4 - $
                         sum_wk2*sum_wk2))) 
       endelse
    endif else begin
      ptr[range].p_s_err = HUGE_VAL
      ptr[range].w_s_err = HUGE_VAL
      ptr[range].sdev_s = HUGE_VAL
    endelse
    if (acf_stat eq ACF_GROUND_SCAT) then ptr[range].gsct=1 $
    else ptr[range].gsct=0    

endelse

  if (npp lt 1) then return, 4

  return, 1


end
