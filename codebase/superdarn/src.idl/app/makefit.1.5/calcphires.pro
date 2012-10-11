; calcphires.pro
; ==============
; Author: R.J.Barnes - Based on C code by K.Baker
; 
; LICENSE AND DISCLAIMER
; 
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
; 
; This file is part of the Radar Software Toolkit (RST).
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU Lesser General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; any later version.
; 
; RST is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU Lesser General Public License for more details.
; 
; You should have received a copy of the GNU Lesser General Public License
; along with RST.  If not, see <http://www.gnu.org/licenses/>.
; 
; 
; 
;
;
; PublicFunctions
; ---------------
;
; FitACFCalcPhiRes


; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFCalcPhiRes
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        s=FitACFCalcPhiRes(acf,badlag,phires,mplgs)
;
;
;-----------------------------------------------------------------
;




function FitACFCalcPhiRes, acf,badlag,phires,mplgs
 
  LAG_SIZE=48

  if (n_elements(phires) eq 0) then phires=dblarr(LAG_SIZE)

  q=where(badlag[0:mplgs-1] eq 0,c)

  if (c ne 0) then phires[q]=atan(imaginary(acf[q]),double(acf[q]))
  
  q=where(phires ne 0,nzc)
 
  if (nzc gt 0) then return, 0
  return, -1
end
