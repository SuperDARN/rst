; gtable.pro
; =============
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
; GridTableTest
; GridTableAddPoint
; GridTableFindPoint
; GridTableAddBeam
; GridTableFindBeam
; GridTableMap
;
;
; Routines:
; ---------
; GridTableMake
; GridPntMake
; GridBmMake
; GridTableZero
;



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableMake
;
; PURPOSE:
;   Creates a pointer to a structure to store the gridded radar data for a
;   single time interval.
;
; PARAMETERS:
;   GridTable   - The pointer to the structure storing gridded cell and beam data
;
; CALLING SEQUENCE:
;   GridTableMake,GridTable
;
;------------------------------------------------------------------------------
;
pro GridTableMake, GridTable

    MAX_BEAMS = 50                          ; hardcoded for IDL (couldn't reallocate arrays like C)
    MAX_PTS = 1000                          ; hardcoded for IDL (couldn't reallocate arrays like C)

    GridTable = { $
        st_time: 0.D, $                     ; start time of the data, specified as seconds from 0:00UT January 1, 1970
        ed_time: 0.D, $                     ; end time of the data, specified as seconds sfrom 0:00UT January 1, 1970
        chn: 0, $                           ; channel number for a stereo radar
        status: 0, $                        ; is set to zero when beginning a new grid, set to one when populating that grid 
        st_id: 0, $                         ; station identifier number
        prog_id: 0, $                       ; control program identifier number
        nscan: 0, $                         ; number of scans included in the analysis
        npnt: 0, $                          ; number of grid points produced by the analysis
        freq: 0.D, $                        ; frequency that the radar was using for this period
        noise: {mean: 0.D, $                ; mean noise value for this period
            sd: 0.D}, $                     ; standard deviation of the noise for this period
        gsct: 0, $                          ; ground scatter flag
        min: fltarr(4), $                   ; filter lower thresholds (velocity, power, spectral width, velocity error)
        max: fltarr(4), $                   ; filter upper thresholds (velocity, power, spectral width, velocity error)
        bnum: 0, $                          ; number of beams included in the analysis
        bm: ptrarr(MAX_BEAMS, /allocate_heap), $    ; pointer to an array of structures containing the beam data
        pnum: 0, $                          ; number of grid cells in the radar field of view
        pnt: ptrarr(MAX_PTS, /allocate_heap) $      ; pointer to an array of structures containing the grid cells
    }

    ; Initialize start time to -1 to indicate this is the first grid
    GridTable.st_time = -1

    ; Create a pointer to the GridTable structure
    GridTable = ptr_new(GridTable, /no_copy)

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridPntMake
;
; PURPOSE:
;   Creates a pointer to a structure to store the gridded parameters for a
;   single grid cell.
;
; PARAMETERS:
;   GridPnt     - The pointer to the structure storing grid cell data
;
; CALLING SEQUENCE:
;   GridPntMake,GridPnt
;
;------------------------------------------------------------------------------
;
pro GridPntMake, GridPnt
    
    GridPnt = { $
        max: 0L, $                          ; number of range points that map to this cell (?)
        cnt: 0L, $                          ; number of data points that map to this cell (?)
        ref: 0L, $                          ; grid cell reference number
        mlat: 0.D, $                        ; magnetic latitude of data point
        mlon: 0.D, $                        ; magnetic longitude of data point
        azm: 0.D, $                         ; magnetic azimuth of data point
        vel: {median: 0.D, $                ; velocity median value
            median_n: 0.D, $                ; northward-component velocity median value (?)
            median_e: 0.D, $                ; eastward-component velocity median value (?)
            sd: 0.D}, $                     ; velocity standard deviation
        pwr: {median: 0.D, $                ; power median value
            sd: 0.D}, $                     ; power standard deviation
        wdt: {median: 0.D, $                ; spectral width median value
            sd: 0.D} $                      ; spectral width standard deviation
    }

    ; Create a pointer to the GridPnt structure
    GridPnt = ptr_new(GridPnt, /no_copy)

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridBmMake
;
; PURPOSE:
;   Creates a pointer to a structure to store the gridded parameters for a
;   single radar beam.
;
; PARAMETERS:
;   GridBm      - The pointer to the structure storing gridded radar beam data
;   nrang       - The number of range gates along the radar beam
;
; CALLING SEQUENCE:
;   GridBmMake,GridBm,nrang
;
;------------------------------------------------------------------------------
;
pro GridBmMake, GridBm, nrang

    GridBm = { $
        bm: 0, $                            ; beam number
        frang: 0, $                         ; distance to first range [km]
        rsep: 0, $                          ; range separation [km]
        rxrise: 0, $                        ; receiver rise time [microseconds]
        nrang: 0, $                         ; number of range gates
        azm: dblarr(nrang), $               ; calculated azimuth for each range cell
        ival: dblarr(nrang), $              ; calculated velocity component to subtract for inertial reference frame
        inx: lonarr(nrang) $                ; look up table that relates beam and range to the grid cell table
    }

    ; Create a pointer to the GridBm structure
    GridBm = ptr_new(GridBm, /no_copy)

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableZero
;
; PURPOSE:
;   Resets all of the values at each grid cell in the GridPnt structure to zero.
;   *Note: This routine is based on the C RST function GridTableZero in gtable.c*
;
; PARAMETERS:
;   pnum        - The number of grid cells in the GridTable structure
;   GridPnt     - The pointer to the structure storing grid cell data
;
; CALLING SEQUENCE:
;   GridTableZero,pnum,GridPnt
;
;------------------------------------------------------------------------------
;
pro GridTableZero, pnum, GridPnt

    ; Zero out all of the values at each grid cell in the GridPnt structure
    for i=0, pnum-1 do begin
        (*GridPnt[i]).azm = 0.D
        (*GridPnt[i]).vel.median_n = 0.D
        (*GridPnt[i]).vel.median_e = 0.D
        (*GridPnt[i]).vel.sd = 0.D
        (*GridPnt[i]).pwr.median = 0.D
        (*GridPnt[i]).pwr.sd = 0.D
        (*GridPnt[i]).wdt.median = 0.D
        (*GridPnt[i]).wdt.sd = 0.D
        (*GridPnt[i]).cnt = 0L
    endfor

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableTest
;
; PURPOSE:
;   Tests whether gridded data should be written to a file. Returns a non-zero
;   value if the data should be written.
;   *Note: This function is based on the C RST function GridTableTest in gtable.c*
;
; INPUTS:
;   GridTable   - The pointer to the structure storing gridded cell and beam data
;   RadarScan   - The pointer to the structure storing the next set of radar scan
;                 data to be fitted to the grid
;
; CALLING SEQUENCE:
;   ret = GridTableTest(GridTable,RadarScan)
;
;------------------------------------------------------------------------------
;
function GridTableTest, GridTable, RadarScan

    ; Calculate time at center of radar scan
    tm = ((*RadarScan).st_time + (*RadarScan).ed_time)/2.0

    ; If this is the first iteration in make_grid then return
    if (*GridTable).st_time eq -1 then $
        return, 0

    ; If the currently loaded RadarScan occurred after the end of the GridTable
    ; structure then begin with the function
    if tm gt (*GridTable).ed_time then begin

        ; Initialize the number of grid points in the GridTable structure to zero
        (*GridTable).npnt = 0

        ; Calculate average frequency, noise mean, and noise standard deviation
        ; across all scans included in grid record
        (*GridTable).freq = (*GridTable).freq / (*GridTable).nscan
        (*GridTable).noise.mean = (*GridTable).noise.mean / (*GridTable).nscan
        (*GridTable).noise.sd = (*GridTable).noise.sd / (*GridTable).nscan

        ; Loop over number of points in GridTable structure
        for i=0, (*GridTable).pnum-1 do begin

            ; If no velocity measurements in GridPnt then continue
            if (*(*GridTable).pnt[i]).cnt eq 0 then $
                continue

            ; If at least 25% of the possible GridPnt cells don't have velocity
            ; measurements then continue
            if (*(*GridTable).pnt[i]).cnt le (0.25*(*GridTable).nscan*(*(*GridTable).pnt[i]).max) then begin
                (*(*GridTable).pnt[i]).cnt = 0
                continue
            endif

            ; Update the total number of grid points in the GridTable structure
            (*GridTable).npnt += 1

            ; Calculate weighted mean of north/east velocity components 
            (*(*GridTable).pnt[i]).vel.median_n = (*(*GridTable).pnt[i]).vel.median_n / (*(*GridTable).pnt[i]).vel.sd
            (*(*GridTable).pnt[i]).vel.median_e = (*(*GridTable).pnt[i]).vel.median_e / (*(*GridTable).pnt[i]).vel.sd

            ; Calculate magnitude of weighted mean velocity vector
            (*(*GridTable).pnt[i]).vel.median = sqrt((*(*GridTable).pnt[i]).vel.median_n*(*(*GridTable).pnt[i]).vel.median_n + $
                                              (*(*GridTable).pnt[i]).vel.median_e*(*(*GridTable).pnt[i]).vel.median_e)

            ; Calculate azimuth of weighted mean velocity vector                              
            (*(*GridTable).pnt[i]).azm = atan((*(*GridTable).pnt[i]).vel.median_e, (*(*GridTable).pnt[i]).vel.median_n) * 180./!pi

            ; Calculate weighted mean of spectral width and power
            (*(*GridTable).pnt[i]).wdt.median = (*(*GridTable).pnt[i]).wdt.median / (*(*GridTable).pnt[i]).wdt.sd
            (*(*GridTable).pnt[i]).pwr.median = (*(*GridTable).pnt[i]).pwr.median / (*(*GridTable).pnt[i]).pwr.sd

            ; Calculate standard deviation of velocity, power, and spectral width
            (*(*GridTable).pnt[i]).vel.sd = 1/sqrt((*(*GridTable).pnt[i]).vel.sd)
            (*(*GridTable).pnt[i]).wdt.sd = 1/sqrt((*(*GridTable).pnt[i]).wdt.sd)
            (*(*GridTable).pnt[i]).pwr.sd = 1/sqrt((*(*GridTable).pnt[i]).pwr.sd)
        endfor

        ; Reset status of GridTable structure to zero
        (*GridTable).status = 0

        ; GridTable structure ready to write to file
        return, 1
    endif

    ; GridTable structure not ready to write to file
    return, 0

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableAddPoint
;
; PURPOSE:
;   Returns the index of the pointer to a newly added grid cell in the structure
;   storing gridded radar data.
;   *Note: This function is based on the C RST function GridTableAddPoint in gtable.c*
;
; INPUTS:
;   GridTable   - The pointer to the structure storing gridded cell and beam data 
;
; OUTPUTs:
;   inx          - The index of the pointer to the newly added GridPnt structure
;
; CALLING SEQUENCE:
;   inx = GridTableAddPoint(GridTable)
;
;------------------------------------------------------------------------------
;
function GridTableAddPoint, GridTable

    ; Make sure that pointer to the GridTable structure exists
    if ~ptr_valid(GridTable) then $
        return, -1

    ; Get current point from the GridTable structure
    pnt = (*GridTable).pnt[(*GridTable).pnum]

    ; If the point structure doesn't exist, create one
    if (*pnt) eq !NULL then begin
        GridPntMake, tmp
        (*pnt) = (*tmp)
    endif

    ; Update the total number of points in the GridTable structure
    (*GridTable).pnum += 1

    ; Return the index of the current point
    return, (*GridTable).pnum-1

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableFindPoint
;
; PURPOSE:
;   Returns the index of the pointer to the GridPnt structure whose reference
;   number matches the input ref value. If a matching grid cell does not exist
;   in the GridTable structure, then a value of -1 is returned.
;   *Note: This function is based on the C RST function GridTableFindPoint in gtable.c*
;
; INPUTS:
;   GridTable   - The pointer to the structure storing gridded cell and beam data
;   ref         - The reference number of the grid cell to look for
;
; OUTPUTS:
;   n           - The index of the pointer to the matching GridPnt structure
;
; CALLING SEQUENCE:
;   n = GridTableFindPoint(GridTable,ref)
;
;------------------------------------------------------------------------------
;
function GridTableFindPoint, GridTable, ref

    ; Loop over all points in GridTable structure
    for n=0, (*GridTable).pnum-1 do begin
        ; If point ref matches input then break
        if ref eq (*(*GridTable).pnt[n]).ref then $
            break
    endfor

    ; Return error flag if point not found
    if n eq (*GridTable).pnum then $
        return, -1

    ; Return index of matching point
    return, n

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableAddBeam
;
; PURPOSE:
;   Returns the index of the pointer to a newly added grid beam in the structre
;   storing gridded radar data. The magnetic latitude, longitude, and azimuth
;   of the velocity measurements are stored in the GridPnt and GrdBm structures
;   respectively.
;   *Note: This function is based on the C RST function GridTableAddBeam in gtable.c*
;
; INPUTS:
;   GridTable   - The pointer to the structure storing gridded cell and beam data
;   RadarSite   - The structure containing radar site and hardware information
;   alt         - The altitude at which the mapping is to be performed [km]
;   tval        - The center time of the radar scan 
;   RadarBeam   - The pointer to the structure storing fitted beam parameters
;
; OUTPUTS:
;   b           - The index of the pointer to the newly added GridBm structure
;
; CALLING SEQUENCE:
;   b = GridTableAddBeam(GridTable,RadarSite,alt,tval,RadarBeam)
;
;------------------------------------------------------------------------------
;
function GridTableAddBeam, GridTable, RadarSite, alt, tval, RadarBeam, $
    chisham=chisham, old_aacgm=old_aacgm

    ; Make sure that pointer to GridTable structure exists
    if ~ptr_valid(GridTable) then $
        return, -1

    ; Make sure that pointer to RadarBeam structure exists
    if ~ptr_valid(RadarBeam) then $
        return, -1

    ; Velocity correction as a function of radar geodetic latitude [m/s]
    velco = (2*!pi/86400.0)*6356.779*1000*cos(RadarSite.geolat*!pi/180.0)

    ; Get current beam from the GridTable structure
    GridBm = (*GridTable).bm[(*GridTable).bnum]

    ; If the GridBm structure doesn't exist, create one
    if (*GridBm) eq !NULL then begin
        GridBmMake, tmp, (*RadarBeam).nrang
        (*GridBm) = (*tmp)
    endif

    ; Update the total number of beams in the GridTable structure 
    (*GridTable).bnum += 1

    ; Populate GridBm structure with info from RadarBeam structure
    ; (except rxrise - that we get from the hdw.dat file because
    ; nearly all radars write a default value of 100 microseconds
    ; hardcoded in default.h and set by global.c in QNX4 systems)
    (*GridBm).bm = (*RadarBeam).bm
    (*GridBm).frang = (*RadarBeam).frang
    (*GridBm).rsep = (*RadarBeam).rsep
    (*GridBm).rxrise = RadarSite.recrise
    (*GridBm).nrang = (*RadarBeam).nrang 

    ; Convert input tval to year, month, day, hour, minutes, seconds
    ret = TimeEpochToYMDHMS(yr, mo, dy, hr, mt, sc, tval)

    ; Loop through range gates along beam
    for r=0, (*GridBm).nrang-1 do begin
        ; Calculate geographic azimuth and elevation to range/beam position
        ; *Note: This is only used for creating a grid in an inertial reference frame*
        ret = RPosRngBmAzmElv((*GridBm).bm, r, yr, RadarSite, (*GridBm).frang, $
                                 (*GridBm).rsep, (*GridBm).rxrise, alt, geoazm, elv, $
                                 chisham=chisham)

        ; If geographic azimuth/elevation calculation failed then break out of loop
        if ret eq -1 then break

        ; Calculate magnetic latitude, longitude, and azimuth of range/beam position
        ret = RPosInvMag((*GridBm).bm, r, yr, RadarSite, (*GridBm).frang, $
                            (*GridBm).rsep, (*GridBm).rxrise, alt, mlat, mlon, mazm, gazm=gazm, $
                            chisham=chisham, old_aacgm=old_aacgm)

        ; If magnetic latitude/longitude/azimuth calculation failed then break out of loop
        if ret eq -1 then break

        ; Make sure magnetic azimuth varies between 0-360 degrees
        if mazm lt 0. then $
            mazm = mazm + 360.

        ; Make sure magnetic longitude varies between 0-360 degrees
        if mlon lt 0. then $
            mlon = mlon + 360.

        ; Calculate magnetic grid latitude cell (eg, 72.1->72.5, 57.8->57.5, etc)
        if mlat gt 0 then $
            grdlat = floor(mlat) + 0.5 $
        else $
            grdlat = floor(mlat) - 0.5

        ; Calculate magnetic grid longitude spacing at grid latitude
        lspc = (floor(360*cos(abs(grdlat)*!pi/180.0)+0.5))/360.0

        ; Calculate magnetic grid longitude cell
        grdlon = (floor(mlon*lspc)+0.5)/lspc

        ; Calculate reference number to grid latitude/longitude cell
        if mlat gt 0 then $
            ref = 1000*( floor(mlat) ) + ( floor(mlon*lspc) ) $
        else $
            ref = -1000*( floor(-mlat) ) - ( floor(mlon*lspc) )

        ; Find index of GridPnt structure corresponding to reference number
        inx = GridTableFindPoint(GridTable, ref)

        ; If matching GridPnt structure not found then create a new one
        if inx eq -1 then $
            inx = GridTableAddPoint(GridTable)

        ; Get pointer to the GridPnt structure
        GridPnt = (*GridTable).pnt[inx]

        ; Set the reference number of the GridPnt structure
        (*GridPnt).ref = ref

        ; Update the total number of range gates that map to the GridPnt structure
        (*GridPnt).max += 1

        ; Set the magnetic latitude and longitude of GridPnt structure
        (*GridPnt).mlat = grdlat
        (*GridPnt).mlon = grdlon

        ; Set the index, magnetic azimuth, and velocity correction factor of
        ; GridBm structure
        (*GridBm).inx[r] = inx
        (*GridBm).azm[r] = mazm
        (*GridBm).ival[r] = velco*cos((geoazm+90)*!pi/180.0)
    endfor

    ; Return error if didn't finish looping through all gates
    if r ne (*GridBm).nrang then $
        return, -1

    ; Return the index of the beam number added to the GridTable structure
    return, (*GridTable).bnum-1

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableFindBeam
;
; PURPOSE:
;   Returns the index of the pointer to the GridBm structure whose beam number
;   and operating parameters match those of the input RadarBeam structure. If a
;   matching grid beam does not exist in the GridTable structure, then a value
;   of -1 is returned.
;   *Note: This function is based on the C RST function GridTableFindBeam in gtable.c*
;
; INPUTS:
;   GridTable   - The pointer to the structure storing gridded cell and beam data
;   RadarBeam   - The pointer to the structure storing fitted beam parameters
;
; OUTPUTS:
;   n           - The index of the pointer to the matching GridBm structure
;
; CALLING SEQUENCE:
;   n = GridTableFindBeam(GridTable,RadarBeam)
;
;------------------------------------------------------------------------------
;
function GridTableFindBeam, GridTable, RadarBeam

    ; Loop over number of beams in GridTable structure
    for n=0, (*GridTable).bnum-1 do begin
        if (*(*GridTable).bm[n]).bm ne (*RadarBeam).bm then continue
        if (*(*GridTable).bm[n]).frang ne (*RadarBeam).frang then continue
        if (*(*GridTable).bm[n]).rsep ne (*RadarBeam).rsep then continue
        if (*(*GridTable).bm[n]).nrang ne (*RadarBeam).nrang then continue

        ; Break out of loop if GridBm parameters match RadarBeam parameters
        ; including beam number, distance to first range, range separation,
        ; receiver rise time, and number of range gates
        break
    endfor

    ; If beam not found then return error
    if n eq (*GridTable).bnum then $
        return, -1

    ; Return index of GridBm structure that matches RadarBeam parameters
    return, n

end



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableMap
;
; PURPOSE:
;   Maps radar scan data to an equi-area grid in magnetic coordinates.
;   *Note: This function is based on the C RST function GridTableMap in gtable.c*
;
; INPUTS:
;   GridTable   - The pointer to the structure storing gridded cell and beam data
;   RadarScan   - The pointer to the structure storing radar scan data
;   RadarSite   - The structure containing radar site and hardware information
;   tlen        - The integration time for each set of gridded data records
;   iflg        - Set this to a non-zero value for the mapping to be performed in
;                 an inertial reference frame (the component of the line-of-sight
;                 velocities due to the Earth's rotation is subtracted)
;   alt         - The altitude at which the mapping is to be performed [km]
;
; CALLING SEQUENCE:
;   ret = GridTableMap(GridTable,RadarScan,RadarSite,tlen,iflg,alt)
;
;------------------------------------------------------------------------------
;
function GridTableMap, GridTable, RadarScan, RadarSite, tlen, iflg, alt, $
    chisham=chisham, old_aacgm=old_aacgm

    ; Initialize some variables to zero
    freq = 0D
    noise = 0D
    variance = 0D
    cnt = 0L

    ; Hardcoded minimum velocity, power, and spectral width error values from C code
    v_e_min = 100.D
    p_l_e_min = 1.D
    w_l_e_min = 1.D

    ; Calculate center time of radar scan
    tm = ((*RadarScan).st_time + (*RadarScan).ed_time)/2.0

    ; If starting a new grid structure then initialize some values
    if (*GridTable).status eq 0 then begin
        (*GridTable).status = 1
        (*GridTable).noise.mean = 0
        (*GridTable).noise.sd = 0
        (*GridTable).freq = 0
        (*GridTable).nscan = 0
        GridTableZero, (*GridTable).pnum, (*GridTable).pnt
        ;(*GridTable).st_time = tlen*( floor(tm/tlen) )      ; should this be floor or round? With 01 min start time,
        (*GridTable).st_time = (*RadarScan).st_time          ; round gives 02 while floor gives 00
        (*GridTable).ed_time = (*GridTable).st_time + tlen
        (*GridTable).st_id = (*RadarScan).stid
    endif

    ; Loop over number of beams in RadarScan structure
    for n=0, (*RadarScan).num-1 do begin

        ; Make sure beam record is valid, else skip
        if (*(*RadarScan).bm[n]).bm eq -1 then $
            continue

        ; Look for beam index in current GridTable structure
        b = GridTableFindBeam(GridTable, (*RadarScan).bm[n])

        ; If beam not found, add a new beam to GridTable structure
        if b eq -1 then begin
            b = GridTableAddBeam(GridTable, RadarSite, alt, tm, (*RadarScan).bm[n], $
                chisham=chisham, old_aacgm=old_aacgm)

            if b eq -1 then $
                break
        endif

        ; Get pointer to beam in GridTable structure
        bm = (*GridTable).bm[b]

        ; Loop over range gates along radar beam
        for r=0, (*(*RadarScan).bm[n]).nrang-1 do begin

            ; If no scatter in beam / range gate cell then continue
            ; (filters in make_grid can also manually change sct value to 0)
            if (*(*RadarScan).bm[n]).sct[r] eq 0 then $
                continue

            ; Get velocity, power, and spectral width errors
            v_e = (*(*(*RadarScan).bm[n]).rng).v_e[r]
            p_l_e = (*(*(*RadarScan).bm[n]).rng).p_l_e[r]
            w_l_e = (*(*(*RadarScan).bm[n]).rng).w_l_e[r]

            ; If velocity error is less than 100 m/s then set it to 100 m/s ???
            if v_e lt v_e_min then $
                v_e = v_e_min

            ; If lambda power error is less than 1 dB then set it to 1 dB ???
            if p_l_e lt p_l_e_min then $
                p_l_e = p_l_e_min

            ; If spectral width error is less than 1 m/s then set it to 1 m/s ???
            if w_l_e lt w_l_e_min then $
                w_l_e = w_l_e_min

            ; Get grid cell index of radar beam / gate measurement
            inx = (*bm).inx[r]

            ; Add magnetic azimuth of radar beam to GridPnt structure value
            (*(*GridTable).pnt[inx]).azm += (*bm).azm[r]

            if iflg ne 0 then begin
                ; If gridding in inertial frame then add north/east velocities to GridPnt structure including ival correction
                (*(*GridTable).pnt[inx]).vel.median_n += -((*(*(*RadarScan).bm[n]).rng).v[r] + (*bm).ival[r])*1./(v_e*v_e)*cos((*bm).azm[r]*!pi/180.0)
                (*(*GridTable).pnt[inx]).vel.median_e += -((*(*(*RadarScan).bm[n]).rng).v[r] + (*bm).ival[r])*1./(v_e*v_e)*sin((*bm).azm[r]*!pi/180.0)
            endif else begin
                ; Otherwise add north/east velocity components to GridPnt structure normally
                (*(*GridTable).pnt[inx]).vel.median_n += -(*(*(*RadarScan).bm[n]).rng).v[r]*cos((*bm).azm[r]*!pi/180.0)/(v_e*v_e)
                (*(*GridTable).pnt[inx]).vel.median_e += -(*(*(*RadarScan).bm[n]).rng).v[r]*sin((*bm).azm[r]*!pi/180.0)/(v_e*v_e)
            endelse

            ; Add power and spectral width values to GridPnt structure
            (*(*GridTable).pnt[inx]).pwr.median += (*(*(*RadarScan).bm[n]).rng).p_l[r]*1./(p_l_e*p_l_e)
            (*(*GridTable).pnt[inx]).wdt.median += (*(*(*RadarScan).bm[n]).rng).w_l[r]*1./(w_l_e*w_l_e)

            ; Add velocity, power, and spectral width standard deviations to
            ; GridPnt structure
            (*(*GridTable).pnt[inx]).vel.sd += 1./(v_e*v_e)
            (*(*GridTable).pnt[inx]).pwr.sd += 1./(p_l_e*p_l_e)
            (*(*GridTable).pnt[inx]).wdt.sd += 1./(w_l_e*w_l_e)

            ; Update the total number of measurements contained in GridPnt structure
            (*(*GridTable).pnt[inx]).cnt += 1L
        endfor

    endfor

    ; Return an error if all beams in RadarScan structure were not considered
    if n ne (*RadarScan).num then $
        return, -1

    ; Loop over number of beams in RadarScan structure
    for n=0, (*RadarScan).num-1 do begin

        ; Make sure beam record is valid, else skip
        if (*(*RadarScan).bm[n]).bm eq -1 then $
            continue

        (*GridTable).prog_id = (*(*RadarScan).bm[n]).cpid
        freq += (*(*RadarScan).bm[n]).freq
        noise += (*(*RadarScan).bm[n]).noise
        cnt += 1L

    endfor

    ; Calculate average frequency and noise of all beams in RadarScan structure
    freq = freq/cnt
    noise = noise/cnt

    ; Loop over number of beams in RadarScan structure
    for n=0, (*RadarScan).num-1 do begin

        ; Make sure beam record is valid, else skip
        if (*(*RadarScan).bm[n]).bm eq -1 then $
            continue

        ; Calculate variance of noise values
        variance += ((*(*RadarScan).bm[n]).noise - noise)*((*(*RadarScan).bm[n]).noise - noise)
    endfor

    ; Add noise noise mean, noise standard deviation, and frequency values to
    ; GridTable structure for later averaging in GridTableTest
    (*GridTable).noise.mean += noise
    (*GridTable).noise.sd += sqrt(variance/cnt)
    (*GridTable).freq += freq
    (*GridTable).nscan += 1

    ; Return zero if successful
    return, 0

end
