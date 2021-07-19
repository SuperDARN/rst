; removenoise.pro
; ===============
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
