; scan.pro
; ===========
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
; RadarScanReset
; RadarScanResetBeam
; RadarScanAddBeam
;
;
; Routines:
; ---------
; RadarScanMake
; RadarBeamMake
; RadarCellMake
;



;------------------------------------------------------------------------------
;+
; NAME:
;   RadarScanMake
;
; PURPOSE:
;   Creates a pointer to a structure to store the fitted parameters for a
;   single radar scan.
;
; PARAMETERS:
;   scan        - The pointer to the structure storing radar scan data
;
; CALLING SEQUENCE:
;   RadarScanMake,scan
;
;------------------------------------------------------------------------------
;
pro RadarScanMake, scan

    MAX_BEAMS = 60                          ; hardcoded for IDL (couldn't reallocate arrays like C)
                                            ; increased from 50 to 60 - EGT 20160913
    scan = {RadarScan, $
            stid: 0, $                      ; station identifier number
            version: {rbstr, $
                major: 0B, $                ; major revision number
                minor: 0B}, $               ; minor revision number
            st_time: 0.0D, $                ; start time of the scan, specified as seconds from 0:00UT January 1, 1970
            ed_time: 0.0D, $                ; end time of the scan, specified as seconds from 0:00UT January 1, 1970
            num: 0, $                       ; number of beams of data in the scan
            bm: ptrarr(MAX_BEAMS, /allocate_heap) $   ; pointer to an array of structures containing the beam data
        }

    ; Create a pointer to the RadarScan structure
    scan = ptr_new(scan, /no_copy)

end



;------------------------------------------------------------------------------
;+
; NAME:
;   RadarBeamMake
;
; PURPOSE:
;   Creates a pointer to a structure to store the fitted parameters for a
;   single radar beam.
;
; PARAMETERS:
;   bm          - The pointer to the structure storing fitted beam parameters
;   nrang       - The number of range gates along the radar beam
;
; CALLING SEQUENCE:
;   RadarBeamMake,bm,nrng
;
;------------------------------------------------------------------------------
;
pro RadarBeamMake, bm, nrang

    ; Create a RadarCell structure with nrang elements
    RadarCellMake, rng, nrang

    bm = {RadarBeam, $
            scan: 0, $                      ; scan flag (1 supposed to mean start of new scan)
            bm: 0, $                        ; beam number
            bmazm: 0.0, $                   ; beam azimuth [degrees]
            time: 0.0D, $                   ; beam time stamp, specified as seconds from 0:00UT January 1, 1970
            cpid: 0, $                      ; control program identifier number
            intt: {itstr, $
                 sc:0, $                    ; integration time [seconds]
                 us:0}, $                   ; integration time [microseconds]
            nave: 0, $                      ; number of pulse sequences transmitted
            frang: 0, $                     ; distance to first range [km]
            rsep: 0, $                      ; range separation [km]
            rxrise: 0, $                    ; receiver rise time [microseconds]
            freq: 0L, $                     ; transmitted frequency [kHz]
            noise: 0L, $                    ; calculated noise from clear frequency search
            atten: 0, $                     ; attenuation level
            channel: 0, $                   ; channel number for a stereo radar
            nrang: 0, $                     ; number of range gates
            sct: intarr(nrang), $           ; array of flags indicating that a range contains scatter. A non-zero value indicates that there is scatter at that range
            rng: ptr_new(rng, /no_copy) $   ; pointer to an array of structures containing the range data
        }

    ; Create a pointer to the RadarBeam structure
    bm = ptr_new(bm, /no_copy)

end



;------------------------------------------------------------------------------
;+
; NAME:
;   RadarCellMake
;
; PURPOSE:
;   Creates a structure to store the fitted parameters and their errors in each
;   each range gate cell along a radar beam.
;
; PARAMETERS:
;   rng         - The structure to store the measured values in each range gate
;   nrang       - The number of range gates along the radar beam
;
; CALLING SEQUENCE:
;   RadarCellMake,rng,nrng
;
;------------------------------------------------------------------------------
;
pro RadarCellMake, rng, nrang

    rng = {gsct: intarr(nrang), $           ; ground scatter flag
            p_0: fltarr(nrang), $           ; lag zero power
            p_0_e: fltarr(nrang), $         ; lag zero power error
            v: fltarr(nrang), $             ; velocity from ACF
            v_e: fltarr(nrang), $           ; velocity error from ACF
            w_l: fltarr(nrang), $           ; spectral width from lambda fit of ACF
            w_l_e: fltarr(nrang), $         ; spectral width error from lambda fit of ACF
            p_l: fltarr(nrang), $           ; power from lambda fit of ACF
            p_l_e: fltarr(nrang) $          ; power error from lambda fit of ACF
        }

end



;------------------------------------------------------------------------------
;+
; NAME:
;   RadarScanReset
;
; PURPOSE:
;   Frees memory reserved to store radar scan data in RadarCell structures,
;   sets the pointers to RadarBeam structures to !NULL, and resets the number
;   of beams contained in the RadarScan structure to zero. Returns zero if
;   successful.
;   *Note: This function is based on the C RST function RadarScanReset in radarscan.c*
;
; INPUTS:
;   RadarScan   - The pointer to the structure storing radar scan data 
;
; CALLING SEQUENCE:
;   ret = RadarScanReset(RadarScan)
;
;------------------------------------------------------------------------------
;
function RadarScanReset, RadarScan

    ; Make sure that pointer to the RadarScan structure exists
    if ~ptr_valid(RadarScan) then $
        return, -1

    ; If beam structures already exist in RadarScan then free them
    if (*(*RadarScan).bm[0]) ne !NULL then begin

        ; Loop over number of beams in RadarScan structure
        for n=0, (*RadarScan).num-1 do begin
            ; Free pointer to the RadarCell structure
            ptr_free, (*(*RadarScan).bm[n]).rng

            ; Set pointer to the beam structure to a null
            (*(*RadarScan).bm[n]) = !NULL
        endfor

    endif

    ; Reset number of beams in the RadarScan structure to zero
    (*RadarScan).num = 0

    ; Return zero if successful
    return, 0

end



;------------------------------------------------------------------------------
;+
; NAME:
;   RadarScanResetBeam
;
; PURPOSE:
;   Takes an input scalar or array of beams which are to be removed from the
;   structure storing radar scan data. Frees memory reserved to store radar
;   scan data in RadarCell structures, reorganizes RadarScan structure to only
;   include pointers to remaining RadarBeam structures, and updates number of
;   beams remaining in RadarScan structure. Returns zero if successful.
;   *Note: This function is based on the C RST function RadarScanResetBeam in radarscan.c*
;
; INPUTS:
;   RadarScan   - The pointer to the structure storing radar scan data 
;   bmnum       - The number of beams to remove from the radar scan structure
;   bm          - The scalar or array of beams to remove from the radar scan
;
; CALLING SEQUENCE:
;   ret = RadarScanResetBeam(RadarScan,bmnum,bm)
;
;------------------------------------------------------------------------------
;
function RadarScanResetBeam, RadarScan, bmnum, bm

    ; Initialize count of remaining beams in RadarScan structure to zero
    num = 0L

    ; Check whether a value has been provided for total number of beams
    ; to exclude from radar scan
    if bmnum eq 0 then $
        return, 0

    ; Make sure that a single beam or array of beams has been provided
    if ~keyword_set(bm) then $
        return, -1

    ; Make sure that pointer to the RadarScan structure exists
    if ~ptr_valid(RadarScan) then $
        return, -1

    ; Check whether a beam has been added to the RadarScan structure
    if (*RadarScan).num eq 0 then $
        return, -1

    ; Loop over number of beams loaded in RadarScan structure
    for n=0, (*RadarScan).num-1 do begin

        ; Loop over number of beams to exclude from each radar scan
        for c=0, bmnum-1 do begin
            ; If beams match then break out of loop
            if (*(*RadarScan).bm[n]).bm eq bm[c] then $
                break
        endfor

        ; If we found a matching beam then remove it
        if c ne bmnum then begin
            ; Free pointer to the RadarCell structure for that beam
            if ptr_valid( (*(*RadarScan).bm[n]).rng ) then $
                ptr_free, (*(*RadarScan).bm[n]).rng

            ; Set pointer to the beam structure to a null
            (*(*RadarScan).bm[n]) = !NULL

            ; Continue to next beam in RadarScan structure
            continue

        endif

        ; Place current beam in next available available index
        (*(*RadarScan).bm[num]) = (*(*RadarScan).bm[n])

        ; Update count of number of beams that have not been excluded
        num += 1L

    endfor

    ; If beams were removed then set extra beam pointers at end of arrays
    ; to nulls
    if num gt 0 then begin
        for n=num, (*RadarScan).num-1 do $
            (*(*RadarScan).bm[n]) = !NULL
    endif

    ; Update number of beams remaining in RadarScan structure
    (*RadarScan).num = num

    ; Return zero if successful
    return, 0

end



;------------------------------------------------------------------------------
;+
; NAME:
;   RadarScanAddBeam
;
; PURPOSE:
;   Returns the pointer to a new beam in the structure storing radar scan data.
;   If a RadarBeam structure does not exist for the current beam, then a new one is 
;   created and the total number of beams in the RadarScan structure is updated.
;   *Note: This function is based on the C RST function RadarScanAddBeam in radarscan.c*
;
; INPUTS:
;   RadarScan   - The pointer to the structure storing radar scan data 
;   nrang       - The number of range gates along the radar beam
;
; OUTPUTS:
;   bm          - The pointer to the structure storing fitted beam parameters
;
; CALLING SEQUENCE:
;   bm = RadarScanAddBeam,RadarScan,nrang
;
;------------------------------------------------------------------------------
;
function RadarScanAddBeam, RadarScan, nrang

    ; Make sure that pointer to the RadarScan structure exists
    if ~ptr_valid(RadarScan) then $
        return, !NULL

    ; Get pointer to current beam from the RadarScan structure
    bm = (*RadarScan).bm[(*RadarScan).num]

    ; If the beam structure doesn't exist, create one
    if (*bm) eq !NULL then begin
        RadarBeamMake, tmp, nrang
        (*bm) = (*tmp)
    endif

    ; Set the number of ranges in the beam structure
    if nrang ne 0 then begin
        (*bm).nrang = nrang
    endif

    ; Set the current beam number in the beam structure
    (*RadarScan).bm[(*RadarScan).num] = bm

    ; Update the total number of beams in the RadarScan structure
    (*RadarScan).num += 1

    ; Return the pointer to the current beam structure
    return, bm

end
