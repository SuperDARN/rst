; global.pro
; ==========
; Author: K.Baker & R.J.Barnes
; 
; copyright (c) 2012 the johns hopkins university/applied physics laboratory
;
; rst is free software: you can redistribute it and/or modify
; it under the terms of the gnu general public license as published by
; the free software foundation, either version 3 of the license, or
; (at your option) any later version.
; 
; this program is distributed in the hope that it will be useful,
; but without any warranty; without even the implied warranty of
; 
; merchantability or fitness for a particular purpose. see the
; gnu general public license for more details.
; 
; you should have received a copy of the gnu general public license
; along with this program. if not, see <https://www.gnu.org/licenses/>.
; 
; modifications:
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



