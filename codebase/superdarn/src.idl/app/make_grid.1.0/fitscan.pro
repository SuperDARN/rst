; fitscan.pro
; ==============
; Author: E.G.Thomas (2016)
; Copyright (C) <year>  <name of author>
; 
; This file is part of the Radar Software Toolkit (RST).
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
; along with this program.  If not, see <https://www.gnu.org/licenses/>.
; 
; Modifications:
;
; Functions:
; ----------
; FitReadRadarScan
;
;
; Routines:
; ---------
;



;------------------------------------------------------------------------------
;+
; NAME:
;   FitReadRadarScan
;
; PURPOSE:
;   Reads one full scan of data from the open fit file with logical unit number
;   unit and stores it in the structure pointed to by RadarScan. Returns zero
;   on success, 1 if the end of file was reached, or -1 if an error occurred.
;   *Note: This function is based on the C RST function FitReadRadarScan in fitscan.c*
;
; INPUTS:
;   unit        - The logical unit number of the open fit file
;   state       - Integer used internally to track progress through the fit file.
;                 Initially this should be set to zero, indicating that no data has
;                 been read. Other possible values are (1) indicating that a
;                 complete scan was read and data from the next scan is pending, or
;                 (2) indicating that a file error occurred
;   prm         - The radar parameter block
;   fit         - The fit data structure
;   tlen        - Ignore scan flag and assume scan has length of tlen seconds
;   lock        - Assume that scan boundaries are fixed relative to start of day
;   channel     - Exclude data record from scan if the channel number recorded
;                 in the radar parameter block (prm) does not match
;
; OUTPUTS:
;   RadarScan   - The pointer to the structure in which to store radar scan data
;
; CALLING SEQUENCE:
;   ret = FitReadRadarScan,unit,state,RadarScan,prm,fit,tlen,lock,channel
;
;------------------------------------------------------------------------------
;
function FitReadRadarScan, unit, state, RadarScan, prm, fit, tlen, lock, channel

    fstatus = 0
    flg = 0

    ; Make sure that pointer to the RadarScan structure exists
    if ~ptr_valid(RadarScan) then $
        return, -1

    ; If the end of the fit file has not already been reached then read the next
    ; record into the prm and fit structures
    if state ne 1 then begin
        ; If no channel is specified then read the next available record
        if channel eq 0 then $
            fstatus = FitRead(unit, prm, fit) $
        else begin
            ; If a channel is specified then continue reading fit records until
            ; either a record for the correct channel is found or the end of
            ; the file is reached
            repeat begin
                fstatus = FitRead(unit, prm, fit)

                if fstatus eq -1 then break

            endrep until ( ((channel eq 2) and (prm.channel ne 2)) or $
                           ((channel eq 1) and (prm.channel eq 2)))
        endelse
    endif

    ; If no errors occured when reading the fit record then begin populating
    ; the RadarScan structure with parameters from the prm structure
    if state ne 2 then begin
        (*RadarScan).stid = prm.stid
        (*RadarScan).version.major = prm.revision.major
        (*RadarScan).version.minor = prm.revision.minor
        (*RadarScan).st_time = TimeYMDHMSToEpoch(prm.time.yr, prm.time.mo, $
                                        prm.time.dy, prm.time.hr, $
                                        prm.time.mt, prm.time.sc+prm.time.us/1.0e6)

        ; If scan flag is being ignored and assuming scan boundaries are fixed
        ; relative to start of day, then recalculate scan start time        
        if ((tlen ne 0) and (lock ne 0)) then $
            (*RadarScan).st_time = tlen*floor((*RadarScan).st_time/tlen)

        ; Reset number of beams in RadarScan structure to zero, set pointers
        ; to RadarBeam structures to !NULL, and free memroy reserved to store
        ; radar scan data in RadarCell structures
        ret = RadarScanReset(RadarScan)

    endif

    ; Indicate that fit data was successfully read and the next record is pending
    state = 1

    ; Add fit and prm records to RadarScan structure until either a new scan 
    ; flag is found or the data duration exceeds tlen, depending on function 
    ; inputs
    repeat begin

        ; Add a new beam to the RadarScan structure
        bm = RadarScanAddBeam(RadarScan, prm.nrang)

        ; Calculate time of radar beam sounding 
        (*bm).time = TimeYMDHMSToEpoch(prm.time.yr, prm.time.mo, $
                                    prm.time.dy, prm.time.hr, $
                                    prm.time.mt, prm.time.sc+prm.time.us/1.0e6)

        ; Load radar operating parameters into RadarBeam structure
        (*bm).scan = prm.scan
        (*bm).bm = prm.bmnum
        (*bm).bmazm = prm.bmazm
        (*bm).cpid = prm.cp
        (*bm).intt.sc = prm.intt.sc
        (*bm).intt.us = prm.intt.us
        (*bm).nave = prm.nave
        (*bm).frang = prm.frang
        (*bm).rsep = prm.rsep
        (*bm).rxrise = prm.rxrise
        (*bm).freq = prm.tfreq
        (*bm).noise = prm.noise.search
        (*bm).atten = prm.atten
        (*bm).channel = prm.channel
        (*bm).nrang = prm.nrang

        ; Set flags indicating scatter in each range gate to zero
        for r=0, (*bm).nrang-1 do $
            (*bm).sct[r] = 0

        ; Loop over number of range gates along radar beam, populating
        ; RadarBeam structure with fit data
        for r=0, (*bm).nrang-1 do begin
            (*bm).sct[r] = fit.qflg[r]
            (*(*bm).rng).gsct[r] = fit.gflg[r]
            (*(*bm).rng).p_0[r] = fit.pwr0[r]
            (*(*bm).rng).p_0_e[r] = 0
            (*(*bm).rng).v[r] = fit.v[r]
            (*(*bm).rng).p_l[r] = fit.p_l[r]
            (*(*bm).rng).w_l[r] = fit.w_l[r]
            (*(*bm).rng).v_e[r] = fit.v_e[r]
        endfor

        ; Calculate end time of radar scan
        (*RadarScan).ed_time = TimeYMDHMSToEpoch(prm.time.yr, prm.time.mo, $
                                        prm.time.dy, prm.time.hr, $
                                        prm.time.mt, prm.time.sc+prm.time.us/1.0e6)

        ; Error check if too many beams were included in RadarScan structure
        if (*RadarScan).num gt 1000 then begin
            flg = -1
            break
        endif

        ; Read the next record into the prm and fit structures
        if channel eq 0 then $
            fstatus = FitRead(unit, prm, fit) $
        else begin
            ; If a channel is specified then continue reading fit records until
            ; either a record for the correct channel is found or the end of
            ; the file is reached
            repeat begin
                fstatus = FitRead(unit, prm, fit)

                if fstatus eq -1 then break

            endrep until ( ((channel eq 2) and (prm.channel ne 2)) or $
                           ((channel eq 1) and (prm.channel eq 2)))
        endelse

        ; If end of file was reached by FitRead then set flg equal to two
        if fstatus eq -1 then $
            flg = 2 $
        else begin
            if tlen eq 0 then begin
                ; Set flg equal to one if scan data according to scan flag
                ; was successfully stored in RadarScan structure
                if abs(prm.scan) eq 1 then $
                    flg = 1
            endif else begin
                ; Set flg equal to one if scan data of length tlen was
                ; successfully stored in RadarScan structure
                if (*RadarScan).ed_time - (*RadarScan).st_time ge tlen then $
                    flg = 1
            endelse
        endelse

    endrep until (flg ne 0)

    ; Decrement value of flg, such that 2->1 indicates the end of the file was
    ; reached or 1->0 indicates attempt to store scan data was a success
    if flg gt 0 then $
        flg -= 1

    ; If the end of fit file was reached then store that information in state
    ; variable
    if flg eq 1 then $
        state = 2

    ; Return zero on success, 1 if the end of file was reached, or -1 if an error
    ; occurred
    return, flg

end
