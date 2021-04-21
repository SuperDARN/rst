; make_grid.pro
; ================
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
; exclude_outofscan
; exclude_range
; strdate
; strtime
;
;
; Routines:
; ---------
; make_grid
;



; *** This function should remove data with scan flags < 0, ***
; *** but has not been fully implemented yet. ***
function exclude_outofscan, RadarScan

    num = 0L

    ; Make sure that pointer to the RadarScan structure exists
    if ~ptr_valid(RadarScan) then $
        return, -1

    ; Make sure that beams have been added to the RadarScan structure
    if (*RadarScan).num eq 0 then $
        return, -1

    ; Loop over number of beams in RadarScan structure
    for n=0, (*RadarScan).num-1 do begin
        if (*(*RadarScan).bm[n]).scan lt 0 then $
            continue

        num += 1L
    endfor

    ; Return zero if succesful
    return, 0
end



;------------------------------------------------------------------------------
;+
; NAME:
;   exclude_range
;
; PURPOSE:
;   Excludes scatter in range gates below minrng or beyond maxrng, or from
;   slant ranges below minsrng or beyond maxsrng. If range gate and slant
;   range threhsolds are both provided, only the slant range thresholds
;   are considered.
;
; PARAMETERS:
;   RadarScan   - The pointer to the structure storing radar scan data
;   minrng      - Exclude data from range gates lower than minrng
;   maxrng      - Exclude data from range gates higher than maxrng
;   minsrng     - Exclude data from slant ranges lower than minsrng
;   maxsrng     - Exclude data from slant ranges higher than maxsrng
;
; CALLING SEQUENCE:
;   exclude_range,RadarScan,minrng,maxrng,minsrng,maxsrng
;
;------------------------------------------------------------------------------
;
pro exclude_range, RadarScan, minrng, maxrng, minsrng, maxsrng

    ; Loop over number of beams in RadarScan structure
    for bm=0, (*RadarScan).num-1 do begin

        ; If RadarBeam structure not set then continue
        if (*(*RadarScan).bm[bm]).bm eq -1 then $
            continue

        ; If minsrng or maxsrng option set then exclude data using slant range
        ; instead of range gate
        if (minsrng ne -1) or (maxsrng ne -1) then begin

            ; Get radar operating parameters from RadarBeam structure
            frang = (*(*RadarScan).bm[bm]).frang
            rsep = (*(*RadarScan).bm[bm]).rsep
            rxrise = (*(*RadarScan).bm[bm]).rxrise
            nrang = (*(*RadarScan).bm[bm]).nrang

            ; Calculate slant range to each range gate and compare to thresholds
            for rng=0, nrang-1 do begin
                r = RadarSlantRange(frang, rsep, rxrise, 0., rng+1)
                if r lt minsrng then $
                    (*(*RadarScan).bm[bm]).sct[rng] = 0

                if r gt maxsrng then $
                    (*(*RadarScan).bm[bm]).sct[rng] = 0
            endfor

        endif else begin

            ; If minrng keyword set then mark all scatter in range gates
            ; less than minrng as being empty
            if minrng ne -1 then $
                for rng=0, minrng-1 do $
                    (*(*RadarScan).bm[bm]).sct[rng] = 0

            ; If maxrng keyword set then mark all scatter in range gates
            ; greater than maxrng as being empty
            if maxrng ne -1 then $
                for rng=maxrng, (*(*RadarScan).bm[bm]).nrang-1 do $
                    (*(*RadarScan).bm[bm]).sct[rng] = 0
        endelse
    endfor

end



;------------------------------------------------------------------------------
;+
; NAME:
;   strdate
;
; PURPOSE:
;   Converts an input date from YYYYMMDD format to an epoch time in number of
;   seconds since 00:00 UT on January 1, 1970.
;
; PARAMETERS:
;   date        - The input date to convert given in YYYYMMDD format
;
; CALLING SEQUENCE:
;   strdate,date
;
;------------------------------------------------------------------------------
;
function strdate, date

    ; Calculate day, month, and year from YYYYMMDD format date
    dy = date MOD 100
    mo = (date / 100) MOD 100
    yr = (date / 10000)

    ; If only 2-digit year provided then assume it was pre-2000 (still valid?)
    if (yr lt 1970) then $
        yr += 1900

    ; Calculate epoch time of input year, month, and day
    tme = TimeYMDHMSToEpoch(yr, mo, dy, 0, 0, 0)

    ; Return epoch time in number of seconds since 00:00UT on January 1, 1970
    return, tme

end



;------------------------------------------------------------------------------
;+
; NAME:
;   strtime
;
; PURPOSE:
;   Converts an input time from HHMM format to number of seconds in long
;   integer format.
;
; PARAMETERS:
;   time        - The input time to convert given in HHMM format
;
; CALLING SEQUENCE:
;   strtime,time
;
;------------------------------------------------------------------------------
;
function strtime, time

    ; Calculate number of hours and minutes from HHMM format time
    mn = time MOD 100
    hr = (time / 100)

    ; Return time as number of seconds
    return, hr*3600L + mn*60L

end



;------------------------------------------------------------------------------
;+
; NAME:
;   make_grid
;
; PURPOSE:
;   Creates a grdmap format file from a fitacf format file.
;
; PARAMETERS:
;   ifilename   - filename of the input fitacf format file
;   ofilename   - filename of the output grdmap format file
;
; CALLING SEQUENCE:
;   make_grid,ifilename,ofilename,sd=sd,st=st,ed=ed,et=et,ex=ex,
;       tl=tl,avlen=avlen,cn=cn,ebm=ebm,minrng=minrng,maxrng=maxrng,
;       fwgt=fwgt,pmax=pmax,vmax=vmax,wmax=wmax,vemax=vemax,
;       pmin=pmin,vmin=vmin,wmin=wmin,vemin=vemin,fmax=fmax,
;       alt=alt,nav=nav,nlm=nlm,nb=nb,is=is,xtd=xtd,
;       ion=ion,gs=gs,both=both,inertial=inertial
;
;------------------------------------------------------------------------------
;
pro make_grid, ifilename, ofilename, sd=sd, st=st, ed=ed, et=et, ex=ex, $
    tl=tl, avlen=avlen, cn=cn, ebm=ebm, minrng=minrng, maxrng=maxrng, $
    minsrng=minsrng, maxsrng=maxsrng, $
    fwgt=fwgt, pmax=pmax, vmax=vmax, wmax=wmax, vemax=vemax, $
    pmin=pmin, vmin=vmin, wmin=wmin, vemin=vemin, fmax=fmax, $
    alt=alt, nav=nav, nlm=nlm, nb=nb, is=is, xtd=xtd, $
    ion=ion, gs=gs, both=both, inertial=inertial, chisham=chisham, $
    old_aacgm=old_aacgm

if n_params() ne 2 then begin
    print, 'Must provide both an input fitACF and output grdmap file name. Returning.'
    return
endif

; Start date of the data period to process. Expressed in the form 
; YYYYMMDD, where YYYY is the year, MM is the month, and DD is the day.
if ~keyword_set(sd) then $
    sdate = -1 $
else $
    sdate = strdate(sd)

; Start time of the data period to process. Expressed in the form
; HHMM, where HH is the number of hours and MM is the number of minutes.
if ~keyword_set(st) then $
    stime = -1 $
else $
    stime = strtime(st)

; End date of the data period to process. Expressed in teh form
; YYYYMMDD, where YYYY is the year, MM is the month, and DD is the day.
if ~keyword_set(ed) then $
    edate = -1 $
else $
    edate = strdate(ed)

; End time of the data period to process. Expressed in the form
; HHMM, where HH is the number of hours and MM is the number of minutes.
if ~keyword_set(et) then $
    etime = -1 $
else $
    etime = strtime(et)

; Extent or length of time of the data period to process. Expressed in
; the form HHMM, where HH is the number of hours and MM is the number
; of minutes.
if ~keyword_set(ex) then $
    extime = 0 $
else $
    extime = strtime(ex)

; Causes the program to ignore the scan flag in the fit files and instead
; use a fixed scan length of tl seconds. The scan boundary is aligned with
; the start of the day.
if ~keyword_set(tl) then $
    tlen = 0 $
else $
    tlen = tl

; Sets the time interval to store in each grid record to i seconds. The default
; is 120 seconds or 2 minutes. Note that this was changed from "-i" in the C
; code to "avlen" because IDL needed a unique keyword.
if ~keyword_set(avlen) then $
    avlen = 120

; Filter based on the Stereo channel, either A or B
if ~keyword_set(cn) then $
    channel = 0 $
else begin
    if (strlowcase(cn[0]) eq 'a') then $
        channel = 1
    if (strlowcase(cn[0]) eq 'b') then $
        channel = 2
endelse

; Exclude data from a single beam or an array of beams
if ~keyword_set(ebm) then begin
    ebm = 0
    ebmn = 0
endif else $
    ebmn = n_elements(ebm)

; Exclude data from range gates lower than minrng
if ~keyword_set(minrng) then $
    minrng = -1

; Exclude data from range gates higher than maxrng
if ~keyword_set(maxrng) then $
    maxrng = -1

; Exclude data from slant ranges lower than minsrng
if ~keyword_set(minsrng) then $
    minsrng = -1

; Exclude data from slant ranges higher than maxsrng
if ~keyword_set(maxsrng) then $
    maxsrng = -1

; Set the median filter weighting to wgt. A value of zero disables
; the filter.
if ~keyword_set(fwgt) then $
    mode = 0 $
else $
    mode = fwgt

; Set the lower limit for the velocity magnitude to vmin. Data points in the
; fit file with velocity magnitude below this threshold will be ignored.
if ~keyword_set(vmin) then $
    vmin = 35

; Set the upper limit for the velocity magnitude to vmax. Data points in the
; fit file with velocity magnitude that exceed this threshold will be ignored.
if ~keyword_set(vmax) then $
    vmax = 2000

; Set the lower limit for the lambda power to pmin. Data points in the fit file
; with lamda power below this threshold will be ignored.
if ~keyword_set(pmin) then $
    pmin = 3

; Set the upper limit for the lambda power to pmax. Data points in the fit file
; with labmda power that exceed this threshold will be ignored.
if ~keyword_set(pmax) then $
    pmax = 50

; Set the lower limit for the spectral width to wmin. Data points in the fit file
; with spectral width below this threshold will be ignored.
if ~keyword_set(wmin) then $
    wmin = 10

; Set the upper limit for the spectral width to wmax. Data points in the fit file
; with spectral width that exceed this threshold will be ignored.
if ~keyword_set(wmax) then $
    wmax = 1000

; Set the lower limit for the velocity error to vemin. Data points in the fit
; file with velocity error below this threshold will be ignored.
if ~keyword_set(vemin) then $
    vemin = 0

; Set the upper limit for the velocity error to vemax. Data points in the fit
; file with velocity error that exceed this threshold will be ignored.
if ~keyword_set(vemax) then $
    vemax = 200

; Set the maximum allowed variation in frequency to fmax [kHz]
if ~keyword_set(fmax) then $
    fmax = 500.

; Set the altitude at which the mapping is done to alt [km]
if ~keyword_set(alt) then $
    alt = 300.

; Do not perform temporal filtering. Usually three consecutive scans are used
; in the median filter. However this can obscure rapid transitions in the
; data. This option forces the median filter to operate on only a single scan.
if ~keyword_set(nav) then $
    bxcar = 1 $
else $
    bxcar = 0

; Do not apply limits to changes in radar parameters between scans. When the
; radar parameters such as range separation or frequency change, the location
; of vectors in adjacent scans will change. These scans are normally ignored,
; as the median filter should only be applied to scans with similar operating
; parameters. This option disables this behavior and includes all scans in
; the analysis.
if ~keyword_set(nlm) then $
    limit = 1 $
else $
    limit = 0

; Do not apply the bounding threshold to lambda power, velocity, spectral
; width, or velocity error.
if ~keyword_set(nb) then $
    bflg = 1 $
else $
    bflg = 0

; Apply a scan flag limit
if keyword_set(ns) then $
    nsflg = 1 $
else $
    nsflg = 0

; Process only those vectors that are classed as ionospheric scatter.
; This is the default operation.
if keyword_set(ion) then $
    ionflg = 1 $
else $
    ionflg = 0

; Process only those vectors that are classed as ground scatter.
if keyword_set(gs) then $
    gsflg = 1 $
else $
    gsflg = 0

; Process both ionospheric and ground scatter vectors.
if keyword_set(both) then $
    bthflg = 1 $
else $
    bthflg = 0

; Generate grid using an inertial reference frame. (The rotation of the
; Earth is factored into the calculation of the velocities).
if ~keyword_set(inertial) then $
    iflg = 0 $
else $
    iflg = 1

; Initialize a couple of status variables for FitReadRadarScan
syncflg = 1
state = 0

; Initialize RadarScan structures
src = ptrarr(3, /allocate_heap)
for i=0, 2 do begin
    RadarScanMake, tmp
    src[i] = tmp
endfor
RadarScanMake, dst

; Initialize GridTable structure
GridTableMake, grid

; Make sure that SD_RADAR environment variable is set
envstr = getenv('SD_RADAR')
if strlen(envstr) eq 0 then begin
    print, 'Environment variable "SD_RADAR" must be defined.'
    return
endif

; Open the radar information file
openr, fp, envstr, /get_lun, /stdio
if size(fp, /type) eq 2 then begin
    if fp eq 0 then begin
        print, 'Could not locate radar information file.'
        return
    endif
endif

network = RadarLoad(fp)
free_lun, fp

; Make sure the SD_HDWPATH environment variable is set
envstr = getenv('SD_HDWPATH')
if strlen(envstr) eq 0 then begin
    print, 'Environment variable "SD_HDWPATH" must be defined.'
    return
endif

; Load the hardware information for the radar network
s = RadarLoadHardware(network, path=envstr)
if s ne 0 then begin
    print, 'Could not load hardware information.'
    return
endif

if mode gt 0 then $
    mode -= 1

; 
(*grid).gsct = 1
if (gsflg) then $
    (*grid).gsct = 0

if (ionflg) then $
    (*grid).gsct = 1

if (bthflg) then $
    (*grid).gsct = 2

if channel ne -1 then $
    (*grid).chn = channel $
else $
    (*grid).chn = 0

; Load the velocity, power, width, and error bounding threshold values into
; temporary arrays
min = [vmin, pmin, wmin, vemin]
max = [vmax, pmax, wmax, vemax]

; Store the velocity, power, width, and error boudning threshold values in the
; GridTable structure only if they are applied by FilterBound
if (bflg) then begin
    for i=0, 3 do begin
        (*grid).min[i] = min[i]
        (*grid).max[i] = max[i]
    endfor
endif

; If median filtering is going to be applied then initialize nbox so that 3
; consecutive scans will go into each iteration of boxcar median filter,
; otherwise set nbox to 1 for operation on only a single scan
if (bxcar) then $
    nbox = 3 $
else $
    nbox = 1

; If median filtering is going to be applied then this is the pointer to
; the radar scan structure which will contain the output of the filter
out = dst

; Set input filename to dname following convention of C make_grid
dname = ifilename

; Open the fit file for reading
fitfp = FitOpen(dname, /read)

; Verify that the file was properly opened
if size(fitfp, /type) eq 2 then begin
    if fitfp eq 0 then begin
        print, 'Could not open file: '+dname
        return
    endif
endif

; Read first available radar scan in fit file (will use scan flag if tlen not provided)
s = FitReadRadarScan(fitfp, state, src[0], prm, fit, tlen, syncflg, channel)

; If scan could not be read from fit file then return
if s eq -1 then begin
    print, 'Error reading file.'
    return
endif

; If either start time or date not provided as input then determine it
if ( (stime ne -1) or (sdate ne -1) ) then begin

    ; If start time not provided then use time of first record in fit file
    if stime eq -1 then $
        stime = (*src[0]).st_time MOD (24*3600.)

    ; If start date not provided then use date of first record in fit file
    if sdate eq -1 then $
        stime += (*src[0]).st_time - ( (*src[0]).st_time MOD (24*3600.) ) $
    else $
        stime += sdate

    ; If median filter is going to be applied then we need to load data prior
    ; to the usual start time, so stime needs to be adjusted
    if bxcar eq 1 then begin
        if tlen ne 0 then $
            stime -= tlen $
        else $
            stime -= 15 + (*src[0]).ed_time - (*src[0]).st_time
    endif

    ; Calculate the year, month, day, hour, minute, and second of grid start time
    ret = TimeEpochToYMDHMS(yr, mo, dy, hr, mt, sc, stime)

    ; Search for index of corresponding record in fit file given grid start time
    s = FitSeek(fitfp, yr, mo, dy, hr, mt, sc, inx)

    ; If a matching record could not be found then exit
    if s eq -1 then begin
        print, 'File does not contain the requested interval.'
        return
    endif

    ; If using scan flag instead of tlen then continue to read fit records until
    ; we reach the beginning of a new scan
    if tlen eq 0 then begin
        while (s ne -1) do begin
            s = FitRead(fitfp, prm, fit)
                if abs(prm.scan) eq 1 then $
                    break
        endwhile
    endif else $
        state = 0

    ; Read the first full scan of data from the open fit file corresponding to our
    ; calculated start date and time
    s = FitReadRadarScan(fitfp, state, src[0], prm, fit, tlen, syncflg, channel)

endif else $
    ; If start date and time not provided then use time of first record in fit file
    stime = (*src[0]).st_time

; If end time provided then determine end date
if etime ne -1 then begin
    ; If end date not provided then use date of first record in fit file
    if edate eq -1 then $
        etime += (*src[0]).st_time - ( (*src[0]).st_time MOD (24*3600.) ) $
    else $
        etime += edate
endif

; If time extent provided then use that to calculate end time
if extime ne 0 then $
    etime = stime + extime

; If end time is set and median filtering is going to be used then we need to
; load data after the usual end time, so etime needs to be adjusted
if (etime ne -1) and (bxcar eq 1) then begin
    if tlen ne 0 then $
        etime += tlen $
    else $
        etime += 15 + (*src[0]).ed_time - (*src[0]).st_time
endif

; Calculate the year, month, day, hour, minute, and second of grid start time
; (needed to load AACGM_v2 coefficients)
ret = TimeEpochToYMDHMS(yr, mo, dy, hr, mt, sc, stime)

; Load AACGM coefficients
if keyword_set(old_aacgm) then $
    ret = AACGMInit(yr) $
else $
    ret = AACGM_v2_SetDateTime(yr, mo, dy, 0, 0, 0)

; This value tracks the number of radar scans which have been loaded for gridding
num = 1

; This value tracks the radar scan position in the boxcar median filter; it will
; cycle between 0,1,2 if median filtering is applied or remain 0 if not
index = 0

; Need to initialize this keyword for GridTableWrite so that it knows data will
; be written to a new grid file rather than an already open one
new_file = 1

;while s ne -1 do begin
while s eq 0 do begin               ; changed by EGT 20160913
                                    ; (problem with end of fit file)

    ; Exclude scatter in beams listed in ebm
    ret = RadarScanResetBeam(src[index], ebmn, ebm)

    ; If 'ns' keyword set then discard data with scan flag < 0
    if (nsflg) then $
        ret = exclude_outofscan(src[index])

    ; Exclude scatter in range gates below minrng or beyond maxrng
    exclude_range, src[index], minrng, maxrng, minsrng, maxsrng

    ; Exclude either ground or ionospheric scatter based on gsct flag
    FilterBoundType, src[index], (*grid).gsct

    ; Exclude scatter outside velocity, power, spectral width, and
    ; velocity error boundaries 
    if (bflg) then $
        FilterBound, src[index], min, max

    ; If enough radar scans have been loaded and the 'nlm' (no limit)
    ; keyword has not been set, then check to make sure there has not been
    ; a change in distance to first range, range separation, or transmit
    ; frequency greater than fmax between the center and adjacent scans
    ; in the boxcar median filter 
    if (num ge nbox) and (limit eq 1) and (mode ne -1) then $
        chk = FilterCheckOps(nbox, src, fmax) $
    else $
        chk = 0

    ; If the operations check succeeded and enough radar scans have
    ; been loaded then proceed with the filtering and gridding
    if (chk eq 0) and (num ge nbox) then begin

        ; Apply the boxcar median filter to the radar scans pointed to by src
        if mode ne -1 then $
            ret = FilterRadarScan(mode, nbox, index, src, dst, 15) $
        else $
            out = src[index]

        ; Calculate the year, month, day, hour, minute, and second of output
        ; scan start time
        ret = TimeEpochToYMDHMS(yr, mo, dy, hr, mt, sc, (*out).st_time)

        ; Load the appropriate radar hardware information for the day
        ; and time of the radar scan (this is only done once)
        if ~keyword_set(site) then begin
            radar = RadarGetRadar(network, (*out).stid)

            site = RadarYMDHMSGetSite(radar, yr, mo, dy, hr, mt, sc)
        endif

        ; Test whether gridded data should be written to a file; if so returns
        ; average (?) velocity, power, and width values for each grid cell 
        s = GridTableTest(grid, out)

        ; If no errors were returned and the beginning of the grid record
        ; is after stime then write the grid data to a grdmap file
        if (s eq 1) and ((*grid).st_time ge stime) then begin
            GridTableWrite, ofilename, grid, xtd=xtd, new_file=new_file, grdfp=grdfp

            ; Update the new_file keyword after first iteration so that
            ; GridTableWrite knows to add subsequent records to an already open
            ; grid file
            if new_file eq 1 then $
                new_file = 0
        endif

        ; Map radar scan data in structure pointed to by 'out' to an equi-area
        ; grid in magnetic coordinates, storing the output in the structure
        ; pointed to by 'grid'
        s = GridTableMap(grid, out, site, avlen, iflg, alt, $
            chisham=chisham, old_aacgm=old_aacgm)

        if s ne 0 then begin
            print, 'Error mapping beams.'
            break
        endif
    endif

    ; If median filtering, increment the index of the 3-element RadarScan structure
    if (bxcar) then $
        index += 1

    ; If median filtering, reset src (RadarScan) structure index if it exceeds 2
    if index gt 2 then $
        index = 0

    ; Read next radar scan from fit file
    s = FitReadRadarScan(fitfp, state, src[index], prm, fit, tlen, syncflg, channel)

    ; If scan data is beyond end of gridding time then break out of loop
    if (etime ne -1) and ((*src[index]).st_time gt etime) then $
        break

    ; Update the number of scans read in this big while loop
    num += 1

endwhile

; Close the fit and grid files
s = FitClose(fitfp)
s = GridClose(grdfp)

end
