; filter.pro
; =============
; Author: E.G.Thomas (2016)
; 
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
; ---------
; FilterCheckOps
; FilterRadarScan
;
;
; Routines:
; ---------
; FilterBoundType
; FilterBound
;



;------------------------------------------------------------------------------
;+
; NAME:
;   FilterBoundType
;
; PURPOSE:
;   Excludes points from radar scan data depending on their type (eg ground or
;   ionospheric scatter).
;   *Note: This routine is based on the C RST function FilterBoundType in bound.c*
;
; PARAMETERS:
;   RadarScan   - The pointer to the structure storing radar scan data
;   type        - The kind of scatter to exclude from scan. A value of (0)
;                 excludes ionospheric scatter and a value of (1) excludes
;                 ground scatter
;
; CALLING SEQUENCE:
;   FilterBoundType,RadarScan,type
;
;------------------------------------------------------------------------------
;
pro FilterBoundType, RadarScan, type

    ; Loop over all beams in scan
    for bm=0, (*RadarScan).num-1 do begin
        ; Loop over all range gates in beam
        for rng=0, (*(*RadarScan).bm[bm]).nrang-1 do begin

            ; If no scatter in that range gate then continue
            if (*(*RadarScan).bm[bm]).sct[rng] eq 0 then $
                continue

            ; If gsct flag (1=gs,0=iono) matches input type flag
            ; (0=gs,1=iono,2=both) then mark cell as having no scatter
            if (*(*(*RadarScan).bm[bm]).rng).gsct[rng] eq type then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0
        endfor
    endfor
    
end



;------------------------------------------------------------------------------
;+
; NAME:
;   FilterBound
;
; PURPOSE:
;   Examines each point in the radar scan data and tests whether its parameters
;   (velocity, power, spectral width, velocity error) lie within certain values.
;   If they do not the data point is rejected and removed.
;   *Note: This routine is based on the C RST function FilterBoundType in bound.c*
;
; PARAMETERS:
;   RadarScan   - The pointer to the structure storing radar scan data
;   min         - A 4-element array containing minimum allowable values for
;                 each parameter (velocity, power, width, velocity error)
;   max         - A 4-element array containing maximum allowable values for
;                 each parameter (velocity, power, width, velocity error)
;
; CALLING SEQUENCE:
;   FilterBound,RadarScan,min,max
;
;------------------------------------------------------------------------------
;
pro FilterBound, RadarScan, min, max

    ; Loop over all beams in scan
    for bm=0, (*RadarScan).num-1 do begin
        ; Loop over all range gates in beam
        for rng=0, (*(*RadarScan).bm[bm]).nrang-1 do begin

            ; If no scatter in that range gate then continue
            if (*(*RadarScan).bm[bm]).sct[rng] eq 0 then $
                continue

            ; If measured velocity magnitude less than vmin then mark gate as empty
            if abs((*(*(*RadarScan).bm[bm]).rng).v[rng]) lt min[0] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If measured velocity magnitude greater than vmax then mark gate as empty
            if abs((*(*(*RadarScan).bm[bm]).rng).v[rng]) gt max[0] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If measured lambda power less than pmin then mark gate as empty
            if (*(*(*RadarScan).bm[bm]).rng).p_l[rng] lt min[1] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If measured lambda power greater than pmax then mark gate as empty
            if (*(*(*RadarScan).bm[bm]).rng).p_l[rng] gt max[1] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If measured spectral width less than wmin then mark gate as empty
            if (*(*(*RadarScan).bm[bm]).rng).w_l[rng] lt min[2] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If measured spectral width greater than wmax then mark gate as empty
            if (*(*(*RadarScan).bm[bm]).rng).w_l[rng] gt max[2] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If measured velocity error less than vemin then mark gate as empty
            if (*(*(*RadarScan).bm[bm]).rng).v_e[rng] lt min[3] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If measured velocity error greater than vemax then mark gate as empty
            if (*(*(*RadarScan).bm[bm]).rng).v_e[rng] gt max[3] then $
                (*(*RadarScan).bm[bm]).sct[rng] = 0
        endfor
    endfor

end



;------------------------------------------------------------------------------
;+
; NAME:
;   FilterCheckOps
;
; PURPOSE:
;   Checks to make sure that the radar operating parameters do not change
;   between scans.  If the frequency, first range distance, or range separation
;   change between scans the location of scatter will also change making it
;   impossible to sensibly median filter the data. An error is returned if any
;   of these parameters changes between scans.
;   *Note: This function is based on the C RST function FilterCheckOps in checkops.c*
;
; INPUTS:
;   depth       - The number of scans of data to compare
;   RadarScan   - The array of pointers to the structures storing radar scan data
;   fmax        - The maximum allowable variation in frequency between scans [?]
;
; CALLING SEQUENCE:
;   ret = FilterCheckOps(depth,RadarScan,fmax)
;
;------------------------------------------------------------------------------
;
function FilterCheckOps, depth, RadarScan, fmax

    ; Loop over number of scans in boxcar
    for num=0, depth-1 do begin

        ; If center scan then continue
        if num eq depth/2 then $
            continue

        ; Loop over number of beams on center scan
        for bm=0, (*RadarScan[depth/2]).num-1 do begin

            ; Current beam number on center scan
            bt = (*(*RadarScan[depth/2]).bm[bm]).bm

            ; Loop over number of beams on scan num
            for t=0, (*RadarScan[num]).num-1 do begin

                ; Check if current beam number matches center scan beam
                if (*(*RadarScan[num]).bm[t]).bm ne bt then $
                    continue

                ; Return error if the distance to first range doesn't match
                if (*(*RadarScan[num]).bm[t]).frang ne $
                    (*(*RadarScan[depth/2]).bm[bm]).frang then $
                    return, -1

                ; Return error if the range separation doesn't match
                if (*(*RadarScan[num]).bm[t]).rsep ne $
                    (*(*RadarScan[depth/2]).bm[bm]).rsep then $
                    return, -1

                ; Return error if frequency difference is larger than fmax
                if (fmax ne 0) and $
                    (abs((*(*RadarScan[num]).bm[t]).freq - $
                         (*(*RadarScan[depth/2]).bm[bm]).freq) gt fmax) then $
                    return, -1
            endfor
        endfor
    endfor

    ; Return zero if successful
    return, 0
end



;------------------------------------------------------------------------------
;+
; NAME:
;   FilterRadarScan
;
; PURPOSE:
;   Performs median filtering on a sequence of radar scan data structures. The
;   filter operates on each range-beam cell, as a 3x3x3 grid of the neighboring
;   cells in both time and space is extracted from the input data and a weighted
;   sum of the number of cells containing scatter is calculated. If this sum is
;   below a certain threshold then the output cell is considered empty and the
;   filter continues to the next range-beam cell. If this sum exceeds the
;   threshold then the median value of each parameter (velocity, power, spectral
;   width) from the 3x3x3 input grid is assigned to the center cell of the output
;   scan data. The errors associated with the parameters are calculated from the
;   standard deviation of the input parameters. Returns zero if successful.
;   *Note: This function is based on the C RST function FilterRadarScan in filter.c*
;
; INPUTS:
;   mode        - Controls the filter threshold and scan information passing. If
;                 the lowest bit (1) is set then a higher threshold is used for
;                 median filtering. If the third bit (4) is set then the scan
;                 information is copied from the center scan of the three input
;                 scans. If the third bit is not set then the average value for
;                 all scans is used.
;   depth       - The number of scans of data supplied to the function
;   inx         - The index of the first scan to apply the filter to
;   src         - The array of pointers to the structures storing radar scan data
;   dst         - The pointer to the structure storing the filtered radar scan data
;   prm         - Determines which parameters are included in the output. If
;                 the lowest bit (1) is set the velocity parameter is included, if
;                 the second bit (2) is set the lambda power parameter is included, if
;                 the third bit (4) is set the spectral width is included, if
;                 the fourth bit (8) is set the lag0 power parameter is included 
;
; CALLING SEQUENCE:
;   ret = FilterRadarScan(mode,depth,inx,src,dst,prm)
;
;------------------------------------------------------------------------------
;
function FilterRadarScan, mode, depth, inx, src, dst, prm

    MAX_BEAM = 256
    FILTER_WIDTH = 3
    FILTER_HEIGHT = 3
    FILTER_DEPTH = 3

    thresh = [12, 24]

    ; Hard coding this value for now so we don't have to dynamically
    ; resize bmptr array later - EGT 20160731
    ; Changed from 10 to 20 for themisscan - EGT 20160926
    TEST_VAL = 20

    bmptr = ptrarr(MAX_BEAM, FILTER_DEPTH, TEST_VAL, /allocate_heap)
    bmcnt = intarr(MAX_BEAM, FILTER_DEPTH)
    weight = intarr(FILTER_WIDTH, FILTER_HEIGHT, FILTER_DEPTH)

    mxbm = 1L
    maxbeam = 0L
    maxrange = 1000L

    ; If input filter depth greater than FILTER_DEPTH then set to FILTER_DEPTH
    if depth gt FILTER_DEPTH then $
        depth = FILTER_DEPTH

    ; Loop over temporal dimension (depth) of median filter
    for i=0, depth-1 do begin
        ; Loop over number of beams in each RadarScan structure
        for n=0, (*src[i]).num-1 do begin
            ; Get current beam number from RadarBeam structure
            bm = (*(*src[i]).bm[n]).bm

            ; Update largest beam number if necessary
            if bm gt maxbeam then $
                maxbeam = bm

            ; Get number of range gates from RadarBeam structure
            rng = (*(*src[i]).bm[n]).nrang

            ; Update largest number of range gates if necessary
            if rng lt maxrange then $
                maxrange = rng
        endfor
    endfor

    ; Add one to get largest total number of beams in RadarScan structures
    maxbeam += 1L

    ; Loop over temporal dimension of median filter
    for z=0, depth-1 do begin
        ; Loop over range dimension of median filter
        for y=0, FILTER_HEIGHT-1 do begin
            ; Loop over beam dimension of median filter
            for x=0, FILTER_WIDTH-1 do begin
                ; Calculate weight of each cell in 3x3x3 filtering grid
                f = (x ne 0) and (x ne FILTER_WIDTH-1) and $
                    (y ne 0) and (y ne FILTER_HEIGHT-1)
                w = 1+f
                f = (z ne 0) and (z ne FILTER_DEPTH-1)
                weight[x,y,z] = w*(1+f)
            endfor
        endfor
    endfor

    ; Reset any information contained in dst RadarScan structure
    ret = RadarScanReset(dst)

    ; Loop over number of beams found from src RadarScan structures
    for bm=0, maxbeam-1 do begin
        ; Add a new beam to the RadarScan structure
        b = RadarScanAddBeam(dst, maxrange)

        ; Initialize the beam number of each beam to -1
        (*b).bm = -1
    endfor

    ; Loop over temporal dimension of median filter
    for z=0, depth-1 do begin
        ; Figure out if scan corresponds to previous, center, or next scan in filter
        i = (inx - (depth-1) + z)

        if i lt 0 then $
            i += depth

        ; Loop over number of beams in RadarScan structure
        for n=0, (*src[i]).num-1 do begin

            ; Get current beam number from RadarBeam structure
            bm = (*(*src[i]).bm[n]).bm

            ; Get index of current beam in bmptr structure
            c = bmcnt[bm,z]

            ; Store pointer to current beam in bmptr array
            bmptr[bm,z,c] = (*src[i]).bm[n]

            ; Update beam count for beam and temporal dimensions
            bmcnt[bm,z] += 1

            ; Update maximum beam count for beam and temporal dimensions
            if bmcnt[bm,z] gt mxbm then $
                mxbm = bmcnt[bm,z]
        endfor

        ; Break out of temporal loop if something went wrong     
        if n ne (*src[i]).num then $
            break
    endfor

    ; Error check to make sure previous loop worked
    if (z ne depth) then begin
        ; Return error
        return, -1
    endif

    ; Get index of "center" scan in temporal dimension
    i = inx - 1
    if i lt 0 then $
        i += depth

    ; Load basic parameters from "center" temporal scan
    (*dst).stid = (*src[i]).stid
    (*dst).version.major = (*src[i]).version.major
    (*dst).version.minor = (*src[i]).version.minor
    (*dst).st_time = (*src[i]).st_time
    (*dst).ed_time = (*src[i]).ed_time

    ; If 4-bit is set then use operating parameters from first
    ; beam found on "center" scan for the output RadarScan structure
    if (mode AND 4) eq 4 then begin
        ; Loop over maximum number of beams in median filter
        for bm=0, maxbeam-1 do begin
            ; If no beams were found for "center" scan then continue
            if bmcnt[bm,depth/2] eq 0 then $
                continue

            ; Get pointer to first beam found for "center" scan
            b = bmptr[bm,depth/2,0]

            ; Load radar operating parameters in RadarBeam structure
            ; *** should we also load bmazm? ***
            (*(*dst).bm[bm]).bm = bm
            (*(*dst).bm[bm]).cpid = (*b).cpid
            (*(*dst).bm[bm]).time = (*b).time
            (*(*dst).bm[bm]).intt = (*b).intt
            (*(*dst).bm[bm]).nave = (*b).nave
            (*(*dst).bm[bm]).frang = (*b).frang
            (*(*dst).bm[bm]).rsep = (*b).rsep
            (*(*dst).bm[bm]).rxrise = (*b).rxrise
            (*(*dst).bm[bm]).freq = (*b).freq
            (*(*dst).bm[bm]).noise = (*b).noise
            (*(*dst).bm[bm]).atten = (*b).atten
            (*(*dst).bm[bm]).channel = (*b).channel
            (*(*dst).bm[bm]).nrang = maxrange
        endfor
    endif else begin
        ; Loop over maximum number of beams in median filter
        for n=0, maxbeam-1 do begin
            ; Initialize radar operating parameters in RadarBeam structure
            ; *** should we also initialize bmazm? ***
            (*(*dst).bm[n]).cpid = -1
            (*(*dst).bm[n]).time = 0
            (*(*dst).bm[n]).intt.sc = 0
            (*(*dst).bm[n]).intt.us = 0L
            (*(*dst).bm[n]).frang = 0
            (*(*dst).bm[n]).rsep = 0
            (*(*dst).bm[n]).rxrise = 0
            (*(*dst).bm[n]).freq = 0D
            (*(*dst).bm[n]).noise = 0D
            (*(*dst).bm[n]).atten = 0
            (*(*dst).bm[n]).channel = -1
            (*(*dst).bm[n]).nrang = -1
        endfor

        ; Loop over temporal dimension of median filter
        for z=0, depth-1 do begin
            ; Loop over maximum number of beams in median filter
            for bm=0, maxbeam-1 do begin
                ; If no beams were previously found then continue
                if bmcnt[bm,z] eq 0 then $
                    continue

                ; Set current beam number in RadarBeam structure
                (*(*dst).bm[bm]).bm = bm

                ; Loop over number of beams found in median filter time/beam cell
                for c=0, bmcnt[bm,z]-1 do begin
                    ; Get pointer to current beam
                    b = bmptr[bm,z,c]

                    ; If this is the first beam in time/beam cell then use it
                    ; to set CPID value for RadarBeam structure
                    if (*(*dst).bm[bm]).cpid eq -1 then $
                        (*(*dst).bm[bm]).cpid = (*b).cpid

                    ; Sum the operating parameters for later averaging across
                    ; all beams in each time/beam cell
                    ; *** should we also average bmazm? ***
                    (*(*dst).bm[bm]).time += (*b).time
                    (*(*dst).bm[bm]).intt.sc += (*b).intt.sc
                    (*(*dst).bm[bm]).intt.us += (*b).intt.us

                    if (*(*dst).bm[bm]).intt.us gt 1e6 then begin
                        (*(*dst).bm[bm]).intt.sc += 1
                        (*(*dst).bm[bm]).intt.us -= 1e6
                    endif

                    (*(*dst).bm[bm]).nave += (*b).nave
                    (*(*dst).bm[bm]).frang += (*b).frang
                    (*(*dst).bm[bm]).rsep += (*b).rsep
                    (*(*dst).bm[bm]).rxrise += (*b).rxrise
                    (*(*dst).bm[bm]).freq += (*b).freq
                    (*(*dst).bm[bm]).noise += (*b).noise
                    (*(*dst).bm[bm]).atten += (*b).atten

                    if (*(*dst).bm[bm]).channel eq 0 then $
                        (*(*dst).bm[bm]).channel = (*b).channel

                    ; If this is the first beam in time/beam cell then use maxrange
                    ; to set the number of range gates for RadarBeam structure
                    if (*(*dst).bm[bm]).nrang eq -1 then $
                        (*(*dst).bm[bm]).nrang = maxrange
                endfor
            endfor
        endfor

        ; Loop over maximum number of beams in median filter
        for n=0, maxbeam-1 do begin
            cnt = 0L

            ; Loop over temporal dimension and count the total number of times
            ; each beam was sounded
            for z=0, depth-1 do $
                cnt += bmcnt[n,z]

            ; If no beams were found then continue
            if cnt eq 0L then $
                continue

            ; Average the radar operating parameters across each beam sounding
            (*(*dst).bm[n]).time = (*(*dst).bm[n]).time/cnt
            (*(*dst).bm[n]).intt.us = (*(*dst).bm[n]).intt.us/cnt
            us = (1e6*(*(*dst).bm[n]).intt.sc)/cnt
            (*(*dst).bm[n]).intt.sc = (*(*dst).bm[n]).intt.sc/cnt
            us = us - 1e6*(*(*dst).bm[n]).intt.sc
            (*(*dst).bm[n]).intt.us += us
            (*(*dst).bm[n]).nave = (*(*dst).bm[n]).nave/cnt
            (*(*dst).bm[n]).frang = (*(*dst).bm[n]).frang/cnt
            (*(*dst).bm[n]).rsep = (*(*dst).bm[n]).rsep/cnt
            (*(*dst).bm[n]).rxrise = (*(*dst).bm[n]).rxrise/cnt
            (*(*dst).bm[n]).freq = (*(*dst).bm[n]).freq/cnt
            (*(*dst).bm[n]).noise = (*(*dst).bm[n]).noise/cnt
            (*(*dst).bm[n]).atten = (*(*dst).bm[n]).atten/cnt
        endfor
    endelse

    ; Create empty RadarCell structure to store all velocity/power/width values
    ; of data in 3x3x3 median filter dimension 
    RadarCellMake, cell, FILTER_WIDTH*FILTER_HEIGHT*depth*mxbm

    ; Create empty RadarCell structure to store velocity/power/width values
    ; which meet the 2 sigma criteria for each parameter
    RadarCellMake, median, FILTER_WIDTH*FILTER_HEIGHT*depth*mxbm

    ; Loop over maximum number of beams in median filter
    for bm=0, maxbeam-1 do begin
        ; Loop over maximum number of range gates in median filter
        for rng=0, maxrange-1 do begin

            ; Set up the spatial 3x3 (BeamxWidth) filtering boundaries
            cnum = 0L
            bmin = bm - FILTER_WIDTH/2
            bbox = bm - FILTER_WIDTH/2
            bmax = bm + FILTER_WIDTH/2
            rmin = rng - FILTER_HEIGHT/2
            rbox = rng - FILTER_HEIGHT/2
            rmax = rng + FILTER_HEIGHT/2

            ; Set lower beam boundary to zero when at edge of FOV
            if bmin lt 0 then $
                bmin = 0

            ; Set upper beam boundary to highest beam when at other edge of FOV
            if bmax ge maxbeam then $
                bmax = maxbeam - 1

            ; Set lower range boundary to zero when at nearest edge of FOV
            if rmin lt 0 then $
                rmin = 0

            ; Set upper range boundary to furthest range gate when at other edge
            if rmax ge maxrange then $
                rmax = maxrange - 1

            ; Initialize "center" cell weight to zero
            w = 0

            ; Loop over beam dimension
            for x=bmin, bmax do begin
                ; Loop over range dimension
                for y=rmin, rmax do begin
                    ; Loop over temporal dimension
                    for z=0, depth-1 do begin
                        ; Loop over number of beams found in time/beam cell
                        for c=0, bmcnt[x,z]-1 do begin
                            ; Get pointer to current beam
                            b = bmptr[x,z,c]

                            ; Add weight of current cell if scatter is present
                            w += weight[x-bbox,y-rbox,z]*(*b).sct[y]

                            ; Make sure we haven't exceeded the number of range
                            ; gates (this seems unnecessary given above check)
                            if y ge (*b).nrang then $
                                continue

                            ; If there is scatter in the beam/range gate cell
                            ; then load it into the cell RadarCell structure
                            if (*b).sct[y] ne 0 then begin
                                cell.gsct[cnum] = (*(*b).rng).gsct[y]
                                cell.p_0[cnum] = (*(*b).rng).p_0[y]
                                cell.p_0_e[cnum] = (*(*b).rng).p_0_e[y]
                                cell.v[cnum] = (*(*b).rng).v[y]
                                cell.v_e[cnum] = (*(*b).rng).v_e[y]
                                cell.w_l[cnum] = (*(*b).rng).w_l[y]
                                cell.w_l_e[cnum] = (*(*b).rng).w_l_e[y]
                                cell.p_l[cnum] = (*(*b).rng).p_l[y]
                                cell.p_l_e[cnum] = (*(*b).rng).p_l_e[y]

                                ; Update the total number of cells in the
                                ; median filter containing scatter
                                cnum += 1L
                            endif
                        endfor
                    endfor
                endfor
            endfor

            ; If no cells were found containing scatter then continue
            if cnum eq 0L then $
                continue

            ; If the current beam is on the edge of the FOV then increase
            ; its calculated weight by 50% (accounting for fewer possible
            ; cells on those beams? what about near/far range edges?)
            if (bm eq 0) or (bm eq maxbeam-1) then $
                w = fix(w*1.5)

            ; If the sum of the weights of the cells containing scatter don't
            ; exceed the threshold (12 or 24) then continue
            if w le thresh[mode MOD 2] then $
                continue

            ; If above threshold was exceeded then mark current beam/gate cell
            ; in output RadarScan structure as having scatter and zero out
            ; groundscatter flag, lambda power, spectral width, and velocity
            (*(*dst).bm[bm]).sct[rng] = 1
            (*(*(*dst).bm[bm]).rng).gsct[rng] = 0
            (*(*(*dst).bm[bm]).rng).p_l[rng] = 0
            (*(*(*dst).bm[bm]).rng).w_l[rng] = 0
            (*(*(*dst).bm[bm]).rng).v[rng] = 0

            ; bitwise and test of prm option to perform velocity median filtering
            if (prm AND 1) eq 1 then begin
                mean = 0D
                variance = 0D
                sigma = 0D
                cnt = 0L

                ; Calculate the mean of the velocity values
                for c=0, cnum-1 do $
                    mean += cell.v[c]
                mean = mean/cnum

                ; Calculate the variance of the velocity values
                for c=0, cnum-1 do $
                    variance += (cell.v[c]-mean)*(cell.v[c]-mean)
                variance = variance/cnum

                ; Calculate the standard deviation of the velocity values
                if variance gt 0 then $
                    sigma = sqrt(variance)

                ; Loop over number of median filter cells for beam/gate
                for c=0, cnum-1 do begin
                    ; If the velocity deviation from the mean is greater
                    ; than 2 standard deviations then continue
                    if abs(cell.v[c]-mean) gt 2*sigma then $
                        continue

                    ; If the velocity mean difference is less than 2 sigma
                    ; then load the cell values into the median RadarCell
                    ; structure
                    median.gsct[cnt] = cell.gsct[c]
                    median.p_0[cnt] = cell.p_0[c]
                    median.p_0_e[cnt] = cell.p_0_e[c]
                    median.v[cnt] = cell.v[c]
                    median.v_e[cnt] = cell.v_e[c]
                    median.w_l[cnt] = cell.w_l[c]
                    median.w_l_e[cnt] = cell.w_l_e[c]
                    median.p_l[cnt] = cell.p_l[c]
                    median.p_l_e[cnt] = cell.p_l_e[c]

                    ; Update the total number of cells with a velocity mean
                    ; difference of less than 2 sigma
                    cnt += 1L
                endfor

                ; Sort velocity values in median RadarCell structure
                ; from most negative to most positive velocity
                vsort = median.v[sort(median.v[0:cnt-1])]

                ; Set current beam/gate velocity to the center of the above
                ; array sorted by velocity (ie the median)
                (*(*(*dst).bm[bm]).rng).v[rng] = vsort[cnt/2]

                ; Reset the mean and variance to zero
                mean = 0D
                variance = 0D

                ; Calculate the mean of the sorted velocities
                for c=0, cnt-1 do $
                    mean += vsort[c]
                mean = mean/cnt

                ; Calculate the variance of the sorted velocities
                for c=0, cnt-1 do $
                    variance += (vsort[c]-mean)*(vsort[c]-mean)
                variance = variance/cnt

                ; Calculate the standard deviation of the sorted velocities
                if variance gt 0 then $
                    sigma = sqrt(variance) $
                else $
                    sigma = 0D

                ; Set the velocity error to the calculated standard deviation
                (*(*(*dst).bm[bm]).rng).v_e[rng] = sigma
            endif

            ; bitwise and test of prm option to perform power median filtering
            if (prm AND 2) eq 2 then begin
                mean = 0D
                variance = 0D
                sigma = 0D
                cnt = 0L

                ; Calculate the mean of the lambda power values
                for c=0, cnum-1 do $
                    mean += cell.p_l[c]
                mean = mean/cnum

                ; Calculate the variance of the lambda power values
                for c=0, cnum-1 do $
                    variance += (cell.p_l[c]-mean)*(cell.p_l[c]-mean)
                variance = variance/cnum

                ; Calculate the standard deviation of the lambda power values
                if variance gt 0 then $
                    sigma = sqrt(variance)

                ; Loop over number of median filter cells for beam/gate
                for c=0, cnum-1 do begin
                    ; If the lambda power deviation from the mean is greater
                    ; than 2 standard deviations then continue
                    if abs(cell.p_l[c]-mean) gt 2*sigma then $
                        continue

                    ; If the lambda power mean difference is less than 2 sigma
                    ; then load the cell values into the median RadarCell
                    ; structure
                    median.gsct[cnt] = cell.gsct[c]
                    median.p_0[cnt] = cell.p_0[c]
                    median.p_0_e[cnt] = cell.p_0_e[c]
                    median.v[cnt] = cell.v[c]
                    median.v_e[cnt] = cell.v_e[c]
                    median.w_l[cnt] = cell.w_l[c]
                    median.w_l_e[cnt] = cell.w_l_e[c]
                    median.p_l[cnt] = cell.p_l[c]
                    median.p_l_e[cnt] = cell.p_l_e[c]

                    ; Update the total number of cells with a power mean
                    ; difference of less than 2 sigma
                    cnt += 1L
                endfor

                ; Sort velocity values in median RadarCell structure
                ; from most negative to most positive velocity
                plsort = median.p_l[sort(median.v[0:cnt-1])]

                ; Set current beam/gate power to the center of the above
                ; array sorted by velocity (ie the median)
                (*(*(*dst).bm[bm]).rng).p_l[rng] = plsort[cnt/2]

                ; Reset the mean and variance to zero
                mean = 0D
                variance = 0D

                ; Calculate the mean of the sorted lambda powers
                for c=0, cnt-1 do $
                    mean += plsort[c]
                mean = mean/cnt

                ; Calculate the variance of the sorted lambda powers
                for c=0, cnt-1 do $
                    variance += (plsort[c]-mean)*(plsort[c]-mean)
                variance = variance/cnt

                ; Calculate the standard deviation of the sorted powers
                if variance gt 0 then $
                    sigma = sqrt(variance) $
                else $
                    sigma = 0D

                ; Set the power error to the calculated standard deviation
                (*(*(*dst).bm[bm]).rng).p_l_e[rng] = sigma
            endif

            ; bitwise and test of prm option to perform width median filtering
            if (prm AND 4) eq 4 then begin
                mean = 0D
                variance = 0D
                sigma = 0D
                cnt = 0L

                ; Calculate the mean of the spectral width values
                for c=0, cnum-1 do $
                    mean += cell.w_l[c]
                mean = mean/cnum

                ; Calculate the variance of the spectral width values
                for c=0, cnum-1 do $
                    variance += (cell.w_l[c]-mean)*(cell.w_l[c]-mean)
                variance = variance/cnum

                ; Calculate the standard deviation of the spectral width values
                if variance gt 0 then $
                    sigma = sqrt(variance)

                ; Loop over number of median filter cells for beam/gate
                for c=0, cnum-1 do begin
                    ; If the spectral width deviation from the mean is greater
                    ; than 2 standard deviations then continue
                    if abs(cell.w_l[c]-mean) gt 2*sigma then $
                        continue

                    ; If the spectral width mean difference is less than 2 sigma
                    ; then load the cell values into the median RadarCell
                    ; structure
                    median.gsct[cnt] = cell.gsct[c]
                    median.p_0[cnt] = cell.p_0[c]
                    median.p_0_e[cnt] = cell.p_0_e[c]
                    median.v[cnt] = cell.v[c]
                    median.v_e[cnt] = cell.v_e[c]
                    median.w_l[cnt] = cell.w_l[c]
                    median.w_l_e[cnt] = cell.w_l_e[c]
                    median.p_l[cnt] = cell.p_l[c]
                    median.p_l_e[cnt] = cell.p_l_e[c]

                    ; Update the total number of cells with a width mean
                    ; difference of less than 2 sigma
                    cnt += 1L
                endfor

                ; Sort velocity values in median RadarCell structure
                ; from most negative to most positive velocity
                wsort = median.w_l[sort(median.v[0:cnt-1])]

                ; Set current beam/gate width to the center of the above
                ; array sorted by velocity (ie the median)
                (*(*(*dst).bm[bm]).rng).w_l[rng] = wsort[cnt/2]

                ; Reset the mean and variance to zero
                mean = 0D
                variance = 0D

                ; Calculate the mean of the sorted spectral widths
                for c=0, cnt-1 do $
                    mean += wsort[c]
                mean = mean/cnt

                ; Calculate the variance of the sorted spectral widths
                for c=0, cnt-1 do $
                    variance += (wsort[c]-mean)*(wsort[c]-mean)
                variance = variance/cnt

                ; Calculate the standard deviation of the sorted spectral widths
                if variance gt 0 then $
                    sigma = sqrt(variance) $
                else $
                    sigma = 0D

                ; Set the width error to the calculated standard deviation
                (*(*(*dst).bm[bm]).rng).w_l_e[rng] = sigma
            endif

            ; bitwise and test of prm option to perform lag0 power median filtering
            if (prm AND 8) eq 8 then begin
                mean = 0D
                variance = 0D
                sigma = 0D
                cnt = 0L

                ; Calculate the mean of the lag0 power values
                for c=0, cnum-1 do $
                    mean += cell.p_0[c]
                mean = mean/cnum

                ; Calculate the variance of the lag0 power values
                for c=0, cnum-1 do $
                    variance += (cell.p_0[c]-mean)*(cell.p_0[c]-mean)

                ; Calculate the standard deviation of the lag0 power values
                variance = variance/cnum
                if variance gt 0 then $
                    sigma = sqrt(variance)

                ; Loop over number of median filter cells for beam/gate
                for c=0, cnum-1 do begin
                    ; If the lag0 power deviation from the mean is greater
                    ; than 2 standard deviations then continue
                    if abs(cell.p_0[c]-mean) gt 2*sigma then $
                        continue

                    ; If the lag0 power mean difference is less than 2 sigma
                    ; then load the cell values into the median RadarCell
                    ; structure
                    median.gsct[cnt] = cell.gsct[c]
                    median.p_0[cnt] = cell.p_0[c]
                    median.p_0_e[cnt] = cell.p_0_e[c]
                    median.v[cnt] = cell.v[c]
                    median.v_e[cnt] = cell.v_e[c]
                    median.w_l[cnt] = cell.w_l[c]
                    median.w_l_e[cnt] = cell.w_l_e[c]
                    median.p_l[cnt] = cell.p_l[c]
                    median.p_l_e[cnt] = cell.p_l_e[c]

                    ; Update the total number of cells with a lag0 power mean
                    ; difference of less than 2 sigma
                    cnt += 1L
                endfor

                ; Sort velocity values in median RadarCell structure
                ; from most negative to most positive velocity
                p0sort = median.p_0[sort(median.v[0:cnt-1])]

                ; Set current beam/gate lag0 power to the center of the above
                ; array sorted by velocity (ie the median)
                (*(*(*dst).bm[bm]).rng).p_0[rng] = p0sort[cnt/2]

                ; Reset the mean and variance to zero
                mean = 0D
                variance = 0D

                ; Calculate the mean of the sorted lag0 powers
                for c=0, cnt-1 do $
                    mean += p0sort[c]
                mean = mean/cnt

                ; Calculate the variance of the sorted lag0 powers
                for c=0, cnt-1 do $
                    variance += (p0sort[c]-mean)*(p0sort[c]-mean)
                variance = variance/cnt

                ; Calculate the standard deviation of the sorted lag0 powers
                if variance gt 0 then $
                    sigma = sqrt(variance) $
                else $
                    sigma = 0D

                ; Set the lag0 power error to the calculated standard deviation
                (*(*(*dst).bm[bm]).rng).p_0_e[rng] = sigma
            endif

        endfor
    endfor

    ; Return zero if successful
    return, 0

end
