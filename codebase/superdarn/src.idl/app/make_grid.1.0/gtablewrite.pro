; gtablewrite.pro
; ==================
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
;
;
; Routines:
; ---------
; GridTableWrite
;



;------------------------------------------------------------------------------
;+
; NAME:
;   GridTableWrite
;
; PURPOSE:
;   Writes a single record of gridded data to a grdmap file. If writing multiple
;   grid records to the same file, the new_file keyword should be set on the first
;   iteration and the returned grdfp value should be passed as a keyword on all
;   subsequent calls to this routine.
;   *Note: This routine is based on the C RST function GridTableWrite in gtablewrite.c*
;
; PARAMETERS:
;   fname       - The name of the output grdmap file in which data should be
;                 recorded
;   GridTable   - The pointer to the structure storing gridded data
;
; KEYWORDS:
;   xtd         - Set this keyword to record extended information including
;                 power and spectral width in addition to the velocity data
;   new_file    - Set this keyword to indicate a new grdmap file should be opened
;   grdfp       - Logical unit number of open grdmap file in which data should
;                 be recorded
;
; CALLING SEQUENCE:
;   GridTableWrite,fname,GridTable,xtd=xtd,new_file=new_file,grdfp=grdfp
;
;------------------------------------------------------------------------------
;
pro GridTableWrite, fname, GridTable, xtd=xtd, new_file=new_file, grdfp=grdfp

    ; Determine whether extended parameter information should be recorded
    if ~keyword_set(xtd) then $
        xtd = 0 $
    else $
        xtd = 1

    ; Create a structure to store grdmap parameter data
    GridMakePrm, prm

    ; Create a structure to store grdmap station vector
    GridMakeStVec, stvec

    ; Create a structure to store grdmap data vector
    GridMakeGVec, gvec

    ; Calculate the year, month, day, hour, minute, and second of grid start
    ; and end time
    ret = TimeEpochToYMDHMS(syr, smo, sdy, shr, smt, ssc, (*GridTable).st_time)
    ret = TimeEpochToYMDHMS(eyr, emo, edy, ehr, emt, esc, (*GridTable).ed_time)

    ; Populate grdmap parameter data structure
    prm.stme.yr = syr
    prm.stme.mo = smo
    prm.stme.dy = sdy
    prm.stme.hr = shr
    prm.stme.mt = smt
    prm.stme.sc = ssc
    prm.etme.yr = eyr
    prm.etme.mo = emo
    prm.etme.dy = edy
    prm.etme.hr = ehr
    prm.etme.mt = emt
    prm.etme.sc = esc
    prm.stnum = 1
    prm.vcnum = (*GridTable).npnt
    prm.xtd = xtd

    ; Populate grdmap station vector data structure
    stvec.st_id = (*GridTable).st_id
    stvec.chn = (*GridTable).chn
    stvec.npnt = (*GridTable).npnt
    stvec.freq = (*GridTable).freq
    stvec.major_revision = 2
    stvec.minor_revision = 0
    stvec.prog_id = (*GridTable).prog_id
    stvec.gsct = (*GridTable).gsct
    stvec.noise.mean = (*GridTable).noise.mean
    stvec.noise.sd = (*GridTable).noise.sd
    stvec.vel.min = (*GridTable).min[0]
    stvec.vel.max = (*GridTable).max[0]
    stvec.pwr.min = (*GridTable).min[1]
    stvec.pwr.max = (*GridTable).max[1]
    stvec.wdt.min = (*GridTable).min[2]
    stvec.wdt.max = (*GridTable).max[2]
    stvec.verr.min = (*GridTable).min[3]
    stvec.verr.max = (*GridTable).max[3]

    ; If new_file keyword is set then open a new grdmap file, otherwise
    ; data will be written to open file with logical unit number grdfp
    if keyword_set(new_file) then begin
        unit = GridOpen(fname, /write)
        grdfp = unit
    endif else $
        unit = grdfp

    ; Resize the grdmap data vector structure according to the number of
    ; data points in the current grid record
    if (*GridTable).npnt ne 0 then $
        gvec = replicate(gvec, (*GridTable).npnt)

    ; Initialize count of grid cells containing data to zero
    cnt = 0

    ; Loop over number of points in GridTable structure
    for n=0, (*GridTable).pnum-1 do begin

        ; If no velocity measurements in GridPnt then continue
        if (*(*GridTable).pnt[n]).cnt eq 0 then $
            continue

        ; Populate grdmap data vector structure with values from
        ; GridPnt structures
        gvec[cnt].mlat = (*(*GridTable).pnt[n]).mlat
        gvec[cnt].mlon = (*(*GridTable).pnt[n]).mlon
        gvec[cnt].azm = (*(*GridTable).pnt[n]).azm
        gvec[cnt].vel.median = (*(*GridTable).pnt[n]).vel.median
        gvec[cnt].vel.sd = (*(*GridTable).pnt[n]).vel.sd
        gvec[cnt].pwr.median = (*(*GridTable).pnt[n]).pwr.median
        gvec[cnt].pwr.sd = (*(*GridTable).pnt[n]).pwr.sd
        gvec[cnt].wdt.median = (*(*GridTable).pnt[n]).wdt.median
        gvec[cnt].wdt.sd = (*(*GridTable).pnt[n]).wdt.sd
        gvec[cnt].st_id = (*GridTable).st_id
        gvec[cnt].chn = (*GridTable).chn
        gvec[cnt].index = (*(*GridTable).pnt[n]).ref

        ; Update the count of grid points in the GridTable structure
        ; containing velocity measurements
        cnt += 1

    endfor

    ; Write the current grid record to a grdmap file
    s = GridWrite(unit, prm, stvec, gvec)

end
