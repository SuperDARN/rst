; fitnoise.pro
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
; FitACFFitNoise


; ---------------------------------------------------------------




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFFitNoise
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;       
;     FitACFFitNoise,ncf,badlag,nbad,badsmp,skynoise,prm,ptr
;
;
;-----------------------------------------------------------------
;



pro FitACFFitNoise,ncf,badlag,nbad,badsmp,skynoise,prm,ptr

  lag_lim = prm.mplgs/2 ; require at least 1/2 of lags be OK

  istat = FitACFFitACF(ncf,0,badlag,nbad,badsmp, $ 
                       lag_lim,prm,0.0,0,0.0,ptr)

  tau = prm.mpinc * 1.0d-6

  if ((istat eq 1) and (ptr.v eq 0.0)) then ptr.v = 1.0
  if ((istat eq 1) and (abs(ptr.v_err/ptr.v) < 0.05)) then begin
    if (ptr.w_l_err le ptr.w_s_err) then begin
      if (ptr.p_s gt skynoise) then ptr.p_s = skynoise
      A = exp(ptr.p_s)
      for j=0,prm.mplgs-1 do begin
        t = (prm.lag[j,1] - prm.lag[j,0])*tau
        ncf[j]=dcomplex(A*exp(-ptr.w_l*t)*cos(ptr.v*t), $
                        A*exp(-ptr.w_l*t)*sin(ptr.v*t))
      endfor
    endif else begin
      if (ptr.p_s gt skynoise) then ptr.p_s = skynoise
      A = exp(ptr.p_s)
      for j=0,prm.mplgs-1 do begin
        t = (prm.lag[1,j] - prm.lag[0,j])*tau
        ncf[j]=dcomplex(A*exp(-(ptr.w_s*t)*(ptr.w_s*t))*cos(ptr.v*t), $
                        A*exp(-(ptr.w_s*t)*(ptr.w_s*t))*sin(ptr.v*t))
      endfor
    endelse
  endif else ncf=0.0d;
end
