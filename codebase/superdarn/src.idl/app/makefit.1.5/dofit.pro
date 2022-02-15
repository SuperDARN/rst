; dofit.pro
; =========
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
; FitACFDoFit 


; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFDoFit
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        s=FitACFDoFit(input,lag_lim,goose,ptr,xptr,elv,noise)
;
;
;
;
;-----------------------------------------------------------------
;



function FitACFDoFit,input,lag_lim,goose,ptr,xptr,elv,noise

  HUGE_VAL=!VALUES.D_INFINITY
  

  PULSE_SIZE=64
  LAG_SIZE=96
  MAX_RANGE=300


  C=2.997d8
  LN_TO_LOG=4.342944819d

  if (input.prm.nave le 1) then return, 0

  badlag=intarr(MAX_RANGE,LAG_SIZE)

  acfd=input.acfd
  xcfd=input.xcfd

  FitACFMakeFitRange,nfit

  first_noise_pwr=0.0D

  freq_to_vel=C/(4*!DPI)/(input.prm.tfreq*1000.0D)

  FitACFBadLags,input.prm.mppul,input.prm.lagfr,input.prm.smsep, $
                input.prm.mpinc,input.prm.txpl,input.prm.pulse,nbad,badsmp

; Determine the lag_0 noise level (0 dB reference) and the noise level at 
; which fit_acf is to quit (average power in the 
; fluctuations of the acfs which are pure noise) 

; If there is a coherent noise transmitter, the noise determination can
; actually be done twice.  The first time to identify and remove the
; coherent signal and the second time to determine the residual noise 

  nfirst=1
  nflag=1
  i=0
 
  while (nflag ne 0) do begin


    pwrd=dblarr(input.prm.nrang)
    pwrd=input.prm.pwr0[0:input.prm.nrang-1]

; determine the average lag0 power of the 10 lowest power acfs

    pwrt=pwrd(sort(pwrd))

; look for the lowest 10 values of lag0 power and average to 
; get the noise level.  Ignore values that are exactly 0.  If
; you can't find 10 useable values within the first 1/3 of the
; sorted power list, then just use whatever you got in that 
; first 1/3.  If you didn't get any useable values, then use
; the NOISE parameter */

    q=where(pwrt[0:input.prm.nrang/3-1] gt 0,c)
    if (c gt 10) then c=10
    if (c eq 0) then minpwr=input.prm.noise $
    else minpwr=total(pwrt[q[0:c-1]])/c  
    noise.sky=minpwr
   
; Now determine the level which will be used as the cut-off power 
; for fit_acf.  This is the average power at all non-zero lags of all
; acfs which have lag0 power < 1.6*mnpwr + 1 stnd. deviation from that
; average power level

 
    noise_pwr = FitACFNoiseStat(minpwr,input.prm,nbad, $
                              badsmp,acfd,Noise_signal)

; if this is the second time through 
; take geometric mean of result this time with last time 

    if (nfirst eq 0) then noise_pwr=sqrt(noise_pwr*first_noise_pwr)


; If there appears to be a noise signal, determine a "noise acf"

    if ((nfirst ne 0) and (Noise_signal gt 0.0)) then begin 
      nfirst = 0

      ave_noise_pwr = FitACFNoiseACF(minpwr,input.prm,pwrd,nbad,badsmp, $
                                     acfd,ncfd)
    
      if (input.prm.xcf ne 0) then $
          xcf_noise_pwr=FitACFNoiseACF(minpwr,input.prm,pwrd,nbad,badsmp, $
                                       xcfd,nxcf)
      noise.lag0=double(ncfd[0])
      

; compare the average noise power in the noise acf with the average
; calculated from all the noise ranges independently.  If the average
; power in the noise acf is great than about 1/2 of the independent
; average, then there is a significant coherent signal that can
; be removed.
; Otherwise, there is no coherent signal, so skip the noise removal 


      if (ave_noise_pwr gt (noise_pwr/2.0D)) then begin
          if (input.prm.xcf) then FitACFFitNoise,nxcf,badlag,$
                                        nbad,badsmp, $
                                        noise.sky,input.prm,nfit 
                                   
          FitACFFitNoise,ncfd,badlag,nbad,badsmp, $
                         noise.sky,input.prm,nfit 
                        
          
          noise.vel=freq_to_vel*nfit.v;
       
          if (nfit.p_l le 0.0) then nflag=0 $
          else nflag=1.0
      endif else nflag=0
 
      if (nflag ne 0) then begin
         for i=0,input.prm.nrang-1 do begin 
           FitACFRemoveNoise,input.prm.mplgs,i,acfd, ncfd
           input.prm.pwr0[i] = double(acfd[i,0]) 
           if (input.prm.xcf ne 0)  then $
             FitACFREmoveNoise,input.prm.mplgs,i,xcfd,nxcf
         endfor
         first_mnpwr = minpwr
         first_noise_pwr = noise_pwr
      endif 
    endif else begin
       Noise_signal = 0.0
       nflag = 0
       nfirst = 0
    endelse
  endwhile

  if (noise.sky gt 0.0D) then skylog=10.0*alog10(noise.sky) $
  else skylog=0.0D

  pwrd=pwrd-noise.sky
  q=where(pwrd le 0.0,c)
  if (c gt 0) then ptr[q].p_0=-50.0D;
  q=where(pwrd gt 0.0,c)
  if (c gt 0) then ptr[q].p_0=10.0*alog10(pwrd[q])-skylog
 
  ptr[*].p_l = -50.0D
  ptr[*].p_s = -50.0D
  ptr[*].p_l_err= 0.0D
  ptr[*].p_s_err= 0.0D
  ptr[*].w_l = 0.0D
  ptr[*].w_s = 0.0D
  ptr[*].w_l_err = 0.0D
  ptr[*].w_s_err = 0.0D
  ptr[*].v = 0.0D
  ptr[*].v_err = 0.0D
  ptr[*].phi0 = 0.0D
  ptr[*].phi0_err=0.0D
  ptr[*].sdev_l = 0.0D
  ptr[*].sdev_s = 0.0D
  ptr[*].sdev_phi = 0.0D
  ptr[*].gsct = 0.0D
  ptr[*].qflg = 0
  ptr[*].nump=0

  xptr[*].p_l = -50.0D
  xptr[*].p_s = -50.0D
  xptr[*].p_l_err= 0.0D
  xptr[*].p_s_err= 0.0D
  xptr[*].w_l = 0.0D
  xptr[*].w_s = 0.0D
  xptr[*].w_l_err = 0.0D
  xptr[*].w_s_err = 0.0D
  xptr[*].v = 0.0D
  xptr[*].v_err = 0.0D
  xptr[*].phi0 = 0.0D
  xptr[*].phi0_err=0.0D
  xptr[*].sdev_l = 0.0D
  xptr[*].sdev_s = 0.0D
  xptr[*].sdev_phi = 0.0D
  xptr[*].gsct = 0.0D
  xptr[*].qflg = 0
  xptr[*].nump=0

  elv[*].normal= 0.0D
  elv[*].low = 0.0D
  elv[*].high = 0.0D

  

  for k=0,input.prm.nrang-1 do begin
    ptr[k].qflg=FitACFFitACF(acfd[k,*],k,badlag,nbad,badsmp, $
                             lag_lim,input.prm,noise_pwr,0,0.0,ptr)

   
    xomega=ptr[k].v;
    if (ptr[k].qflg eq 1) then begin

; several changes have been made here to 
; fix an apparent problem in handling HUGE_VAL.
	     
; If there are too few points in an ACF to allow
; the error on a parameter to be calculated then
; the subroutine FitACFFitAcf sets the value to HUGE_VAL.

; However, in this routine the error values are converted
; to natural units (e.g. velocity instead of frequency).
; It appears that multiplying HUGE_VAL by something causes
; a floating point exception that then sets the result of
; the calculation to 0.  Thus the error values that were being
; stored in the file would be zero instead of HUGE_VAL.

; The code now checks to see if the value is set to
; HUGE_VAL before doing the conversion.  If it is then
; instead of a converted version the error value is
; reset to HUGE_VAL.
	    
; convert power from natural log to dB 

      ptr[k].p_l = ptr[k].p_l*LN_TO_LOG - skylog
      ptr[k].p_s = ptr[k].p_s*LN_TO_LOG - skylog

      if (ptr[k].p_l_err ne HUGE_VAL) then $
          ptr[k].p_l_err = ptr[k].p_l_err*LN_TO_LOG

      if (ptr[k].p_s_err ne HUGE_VAL) then $
          ptr[k].p_s_err = ptr[k].p_s_err*LN_TO_LOG

; convert Doppler frequency to velocity 

      ptr[k].v = input.prm.vdir*freq_to_vel*ptr[k].v
	  
      if (ptr[k].v_err ne HUGE_VAL) then $ 
          ptr[k].v_err = freq_to_vel*ptr[k].v_err

; convert decay parameters to spectral widths 

      ptr[k].w_l = freq_to_vel*2*ptr[k].w_l
      if (ptr[k].w_l_err ne HUGE_VAL) then $
          ptr[k].w_l_err = freq_to_vel*2*ptr[k].w_l_err

; sigma is returned as sigma**2 so check the sign for validity
;if sigma**2 is negative take sqrt of the abs and transfer the sign 

      if (ptr[k].w_s ge 0) then ptr[k].w_s=sqrt(ptr[k].w_s) $
      else ptr[k].w_s=-sqrt(-ptr[k].w_s) 

      if ((ptr[k].w_s ne 0) and (ptr[k].w_s_err ne HUGE_VAL)) then $
        ptr[k].w_s_err = 0.5d*ptr[k].w_s_err/abs(ptr[k].w_s) $
      else ptr[k].w_s_err=HUGE_VAL


      ptr[k].w_s = 3.33d*freq_to_vel*ptr[k].w_s
      if (ptr[k].w_s_err ne HUGE_VAL) then $
          ptr[k].w_s_err = 3.33d*freq_to_vel*ptr[k].w_s_err

      
; Now check the values of power, velocity and width
; to see if this should be flagged as ground-scatter *
	        
     if (ptr[k].gsct eq 0) then ptr[k].gsct=FitACFGroundScatter(ptr[k]) 

    endif
 
    if ((input.prm.xcf ne 0) and (ptr[k].qflg eq 1)) then begin
      xptr[k].qflg=FitACFFitACF(xcfd[k,*],k,badlag,nbad,badsmp, $
                               lag_lim,input.prm,noise_pwr,1,xomega,xptr)
    endif

    if (xptr[k].qflg eq 1) then begin

      xptr[k].p_l = xptr[k].p_l*LN_TO_LOG - skylog
      xptr[k].p_s = xptr[k].p_s*LN_TO_LOG - skylog

      if (xptr[k].p_l_err ne HUGE_VAL) then $
          xptr[k].p_l_err = xptr[k].p_l_err*LN_TO_LOG

      if (xptr[k].p_s_err ne HUGE_VAL) then $
          xptr[k].p_s_err = xptr[k].p_s_err*LN_TO_LOG

; convert Doppler frequency to velocity 

      xptr[k].v = input.prm.vdir*freq_to_vel*xptr[k].v
	  
      if (xptr[k].v_err ne HUGE_VAL) then $ 
          xptr[k].v_err = freq_to_vel*xptr[k].v_err

; convert decay parameters to spectral widths 

      xptr[k].w_l = freq_to_vel*2*xptr[k].w_l
      if (xptr[k].w_l_err ne HUGE_VAL) then $
          xptr[k].w_l_err = freq_to_vel*2*xptr[k].w_l_err

; sigma is returned as sigma**2 so check the sign for validity
;if sigma**2 is negative take sqrt of the abs and transfer the sign 

      if (xptr[k].w_s ge 0.0d) then xptr[k].w_s=sqrt(xptr[k].w_s) $
      else xptr[k].w_s=-sqrt(-xptr[k].w_s) 

      if ((xptr[k].w_s ne 0.0d) and (xptr[k].w_s_err ne HUGE_VAL)) then $
        xptr[k].w_s_err = 0.5d*xptr[k].w_s_err/abs(xptr[k].w_s) $
      else xptr[k].w_s_err=HUGE_VAL

      xptr[k].w_s = 3.33d*freq_to_vel*xptr[k].w_s
      if (xptr[k].w_s_err ne HUGE_VAL) then $
          xptr[k].w_s_err = 3.33d*freq_to_vel*xptr[k].w_s_err
	
; calculate the elevation angle 

      if (xptr[k].phi0 gt !DPI)  then xptr[k].phi0 = xptr[k].phi0 - 2*!DPI
      if (xptr[k].phi0 lt -!DPI) then xptr[k].phi0 = xptr[k].phi0 + 2*!DPI
      if (input.prm.phidiff ne 0) then $ 
	    xptr[k].phi0 = xptr[k].phi0*input.prm.phidiff


; changes which array is first
		
      range = 0.15d*(input.prm.lagfr + input.prm.smsep*(k-1))
      if (goose eq 0) then begin
        elv[k].normal = FitACFElevation(input.prm,range, xptr[k].phi0)
	elv[k].low =  $
             FitACFElevation(input.prm,range, xptr[k].phi0+xptr[k].phi0_err)
        elv[k].high = $
             FitACFElevation(input.prm,range,xptr[k].phi0-xptr[k].phi0_err)
      endif else begin
        elv[k].normal = FitACFElevGoose(input.prm,range, xptr[k].phi0)
        elv[k].low = $
             FitACFElevGoose(input.prm,range, xptr[k].phi0+xptr[k].phi0_err)
        elv[k].high =  $
             FitACFElevGoose(input.prm,range, xptr[k].phi0-xptr[k].phi0_err)
      endelse
    endif
   
    if (ptr[k].qflg eq 1) then i=i+1
  endfor
 
  return, i
 
end

