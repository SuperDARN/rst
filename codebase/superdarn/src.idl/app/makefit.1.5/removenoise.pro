; removenoise.pro
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
; FitACFRemoveNoise 


; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFRemoveNoise
;
; PURPOSE:
;       Subtract the noise ACF from the actual ACF.
;       
;
; CALLING SEQUENCE:
;       FitMakeRemoveNoise,mplgs,range,acf,ncf
;
;       This subtracts the noise ACF from an ACF at a given range.
;       The number of lags is given by mplgs, the range to process
;       is given by range, the array of ACF's is given by acf and
;       the noise ACF is given by ncf.
;
;       The array acf is modified.
;
;
;-----------------------------------------------------------------
;


pro FitACFRemoveNoise,mplgs,range,acf,ncf

  plim=abs(ncf[0])
  pa=abs(acf[range,0])

  if (pa gt plim) then acf[range,*]=acf[range,*]-ncf $
  else acf[range,*]=dcomplex(0.0d,0.0d)
end
