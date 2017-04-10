; morebadlags.pro
; ===============
; Author: R.J.Barnes - Based on C code by K.Baker
; 
; See license.txt
; 
; 
; 
;
;
; PublicFunctions
; ---------------
;
; FitACFMoreBadlags


; ---------------------------------------------------------------

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFMoreBadlags
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        nump=FitACFMoreBadlags(w, badlag, noise_lev,mplgs,nave)
;
;
;-----------------------------------------------------------------
;




function FitACFMoreBadlags, w, badlag, noise_lev,mplgs,nave

  badflag_1 = 0
  badflag_2 = 0

  fluct0 = w[0]/sqrt(2.0d*nave)
  fluct = w[0] + 2.0d*noise_lev+fluct0
  fluct_old = fluct
  sum_np = 0
  k_old = 0

 
  for k=0,mplgs-1 do begin
    if (badlag[k] eq 0) then begin
    
      if (badflag_2 ne 0) then badlag[k]=7 $
      else begin
       if (w[k] le 1.0) then begin
         badlag[k]=3
         badflag_2=badflag_1
         badflag_1=1
       endif else begin
          badflag_1=0
          if (w[k] gt fluct) then begin
            badlag[k]=5
            if (k lt (mplgs-1)) then begin
               if ((w[k] lt fluct_old) and (w[k+1] gt fluct) and $
                   (w[k+1] lt w[k])) then begin
                 badlag[k_old]=9
                 sum_np=sum_np-1 
                 badlag[k]=0   
               endif
            endif
          endif

          fluct_old=fluct
          fluct=2.0d*noise_lev + w[k] + fluct0
      endelse

      endelse
      if (badlag[k] eq 0) then begin
        sum_np=sum_np+1
        k_old=k
    endif
endif

endfor

  return, sum_np
end


