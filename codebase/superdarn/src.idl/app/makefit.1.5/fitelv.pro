; fitelv.pro
; ==========
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
; FitACFMakeFitElv


; ---------------------------------------------------------------




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFMakElv
;
; PURPOSE:
;      Creates a structure to store the elevation angle data.
;       
;
; CALLING SEQUENCE:
;        
;
;      FitACFMakeFitElv,elv
;
;-----------------------------------------------------------------
;


pro FitACFMakeFitElv,elv

  elv={FitElv, $
       normal:0.0D, $
       low:0.0D, $
       high:0.0D $
      }
end

