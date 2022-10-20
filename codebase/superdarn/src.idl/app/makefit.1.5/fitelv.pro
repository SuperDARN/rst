; fitelv.pro
; ==========
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
;      2022-01-23 Emma Bland (UNIS): Added "error" and "fitted" fields for FitACF v3
; 
; Public Functions
; ----------------
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
       high:0.0D, $
       error:0.0D, $
       fitted:0.0D $
      }
end

