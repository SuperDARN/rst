; global.pro
; ==========
; Author: K.Baker & R.J.Barnes
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
; ---------------------------------------------------------------
; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	This section of code initializes the common blocks and some of
;	the fundamental parameters associated with the radar data
;
;
;
;	initialize the common blocks
;
	common rawdata_com, rawfileptr, raw_data, badlag, $
	                    more_badrange, lags, rd_byte
	common fitdata_com, fitfileptr, fit_data
	common globals, scan_type, mflag
;
;
;       define constants
;
        TRUE = 1
        FALSE = 0
        PARMS_SIZE = 48
        PULSE_PAT_LEN = 16
        LAG_TAB_LEN = 48
        COMBF_SIZE = 80
        MAX_RANGE = 75
        loop = TRUE
        EOF = -1
        COLOR_TABLE = 12
;
;       data declarations
;	
	scan_type = 0
	mflag = 0
;	x = fltarr(17,76)
;	y = fltarr(17,76)
	fitfileptr = 0L
	fitdef, fit_data
	rawfileptr = 0L
	rawdef, raw_data
	badlag = intarr(LAG_TAB_LEN)
	more_badrange = -1
	lags = intarr(LAG_TAB_LEN,2)
	rd_byte = bytarr(58300)
;
;	return to the user
;
	end



