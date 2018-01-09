;------------------------------------------------------------------------------
; MLT functions for use with AACGM-v2
;
; IDL user functions intended to compute the magnetic local time at a given
; time and location given by AACGM-v2 coordinates.
;
; Code based on legacy routines authored by Kile Baker, Simon Wing and Robin
; Barnes.
;
; Significant changes include:
;
; - discontinued use of common block used for interpolation between two values
;   separated by 10 minutes.
;
; - MLT is based on the relative AACGM-v2 longitudes of the desired location to
;   that of a reference location. The reference location is taken to be the
;   subsolar point at 700 km altitude. The reference AACGM-v2 longitude is
;   computed using AACGM-v2 coefficients, which act to interpolate through
;   regions where AACGM coordinates are undefined, thereby providing a
;   mechanism for defining the reference longitude in a consistent manner.
;   Differences with values determined at 0 km altitude (where defined) are
;   typically <1 minute and always <5 minutes (in MLT).
;
; 20170601 SGS v1.2  MLTConvert_v2 now calls AACGM_v2_SetDateTime() if the
;                    AACGM-v2 date/time is not currently set OR if the
;                    date/time passed into one of the public functions differs
;                    from the AACGM-v2 date/time by more than 30 days. In each
;                    case the AACGM-v2 coefficients are loaded and interpolated
;                    which could impact other calls to AACGM_v2_Convert() if
;                    the date/time is not reset.
;
; 20160811 SGS v1.1  Added keyword MLT2MLON to allow for inverse calculation,
;                    i.e., mlon argument is actually MLT and mlon is computed
;                    and returned.
;
; 20160423 SGS v1.0  Implementation of MLT with AACGM-v2. The intent is to
;                    maintain compatibility with older routines to the extent
;                    that it is possible. AACGM-v2 broke the MLT functionality
;                    due to the undefined regions. A new routine is provided.
; 
;
; Public Functions:
; -----------------
;
; mlt = mlt_v2(mlon, year=yr, month=mo, day=dy, hour=hr, minute=mt, second=sc,$
;                    sstrace=sstrace, ssheight=ssheight, err=err, /MLT2mlon)
; mlt = MLTConvertYMDHMS_v2(yr,mo,dy,hr,mt,sc, mlon, $
;                           sstrace=sstrace, ssheight=ssheight, anti=anti, $
;                           err=err, dday_bug=dday_bug, /MLT2mlon)
; mlt = MLTConvertEpoch_v2(epoch, mlon, $
;                          sstrace=sstrace, ssheight=ssheight, anti=anti, $
;                          err=err, dday_bug=dday_bug, /MLT2mlon)
; mlt = MLTConvertYrsec_v2(yr,yrsec, mlon, $
;                          sstrace=sstrace, ssheight=ssheight, anti=anti, $
;                          err=err, dday_bug=dday_bug, /MLT2mlon)
;
; Private Functions:
; ------------------
;
; mlt = MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlon, sstrace=sstrace, $
;                        ssheight=ssheight, anti=anti, err=err, $
;                        dday_bug=dday_bug, /MLT2mlon)
;
;-----------------------------------------------------------------------------
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvert_v2
;
; PURPOSE:
;       Calculate Magnetic Local Time (MLT) using AACGM-v2 coordinates
;       
;
; CALLING SEQUENCE:
;       mlt = MLTConvert_v2(yr,mo,dy,hr,mt,sc, mlon [, sstrace=sstrace] $
;                            [, ssheight=ssheight] [, anti=anti] [, err=err] $
;                            [, dday_bug=dday_bug] [, /MLT2mlon]
;
;       mlon     - AACGM-v2 magnetic longitude of desired location
;       sstrace  - use field-line tracing option for reference longitude. Note,
;                  this is an advanced feature and can result in undefined
;                  results. Not recommended.
;       ssheight - use a different height when computing the reference longitude
;                  of the subsolar point.
;       err      - error code. 0 for success.
;       anti     - use antisolar point for reference longitude
;       MLT2mlon - set keyword for inverse calculation. mlon is taken to be MLT
;                  in this case.
;     
;-----------------------------------------------------------------------------
;

function MLTConvert_v2, yr,mo,dy,hr,mt,sc,mlon, sstrace=sstrace, $
                        ssheight=ssheight, anti=anti, err=err, $
                        dday_bug=dday_bug, MLT2mlon=MLT2mlon

  common AACGM_v2_Com

  if (n_elements(aacgm_v2_datetime) eq 0) then begin
    ; AACGM date/time not set so set it to the date/time passed in
    e = AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,sc)
    if e ne 0 then return, e
  endif else begin
    ; If date/time passed into function differs from AACGM data/time by more
    ; than 30 days, recompute the AACGM-v2 coefficients
    e = AACGM_v2_GetDateTime(ayr,month=amo,day=ady,hour=ahr, $
                                 minute=amt,second=asc)
    ajd = TimeYMDHMSToJulian(ayr,amo,ady,ahr,amt,asc)
    tyr = yr    ; arguments are passed by reference and yr is modified
    jd =  TimeYMDHMSToJulian(tyr,mo,dy,hr,mt,sc)
    if abs(jd-ajd) gt 30 then begin
      e = AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,sc)
    endif
    if e ne 0 then return, e
  endelse

  ierr = 0
  mlon = reform(mlon)
  qq = where(finite(mlon, /nan), nqq)
  if nqq gt 0 then begin

    ierr = -4
    if nqq eq n_elements(mlon) then begin
      err = ierr
      return, mlon  ; just return all NaNs
    endif

    mlon[qq] = 0      ; set these to zero because we are changing them later
  endif

  ; compute corrected time
  dd  = AstAlg_dday(dy,hr,mt,sc, dday_bug=dday_bug)
  jd  = AstAlg_jde(yr,mo,dd)
  eqt = AstAlg_equation_of_time(jd)
  dec = AstAlg_solar_declination(jd)

  ; comparison with C version is exact
  ;fmt = '(f20.12)'
  ;print, ''
  ;print, 'dd  = ', string(dd, format=fmt)
  ;print, 'jd  = ', string(jd, format=fmt)
  ;print, 'eqt = ', string(eqt, format=fmt)
  ;print, 'dec = ', string(dec, format=fmt)
  ;print, ''

  ut = hr*3600.0D + mt*60.0D + sc   ; UT in seconds
  at = ut + eqt*60.0D

  ; compute reference longitude: default is subsolar point
  if keyword_set(anti) then $
    slon = -at*15.0/3600.0 $            ; longitude of antisolar point
  else $
    slon = (43200.0-at)*15.0/3600.0   ; longitude of subsolar point

  if keyword_set(ssheight) then hgt = ssheight else hgt = 700.

; ideally don't recompute mlon_ref if time has not changed, but then must
; keep track of last time...

  ; compute AACGM-v2 coordinates of reference point
  if keyword_set(anti) then $
    p = cnvcoord_v2(-dec, slon, hgt, trace=sstrace) $ ; anti solar point
  else $
    p = cnvcoord_v2(dec, slon, hgt, trace=sstrace)      ; subsolar point
  if (size(p))[0] eq 0 then return, !values.f_nan       ; date/time not set?
  mlon_ref = p[1]   ; AACGM-v2 longitude of reference point

  ; check for error: if not sstrace then this should NOT happen...
  if n_elements(p) eq 1 or finite(p[0], /nan) then begin
    if ~keyword_set(sstrace) then err = -99 else err = -1
    return, !values.f_nan
  endif

  if keyword_set(MLT2mlon) then begin
    ; inverse calculation: MLT to mlon
    mlt = mlon    ; this is what is actually passed in
    if keyword_set(anti) then $
      tmlon = mlt*15. + mlon_ref $        ; works for an array of MLT
    else $
      tmlon = (mlt - 12.)*15. + mlon_ref  ; works for an array of MLT

    ; ensure mlon is between -180 and 180 regardless of how crazy inputs are...
    nq = 1
    while nq ne 0 do begin
      q = where(tmlon gt 180., nq)
      if nq gt 0 then tmlon[q] -= 360.
    endwhile
    nq = 1
    while nq ne 0 do begin
      q = where(tmlon lt -180., nq)
      if nq gt 0 then tmlon[q] += 360.
    endwhile

    if ierr eq -4 then begin
      ; there were NaNs passed in
      tmlon[qq] = !values.f_nan
      err = ierr
    endif

    return, tmlon
  endif else begin
    ; standard calculation: mlon to MLT
    if keyword_set(anti) then $
      mlt =  0. + (mlon - mlon_ref)/15.0 $  ; works for an array of mlon
    else $
      mlt = 12. + (mlon - mlon_ref)/15.0    ; works for an array of mlon

    ; ensure mlt is between 0 and 24 regardless of how crazy inputs are...
    nq = 1
    while nq ne 0 do begin
      q = where(mlt gt 24., nq)
      if nq gt 0 then mlt[q] -= 24.
    endwhile
    nq = 1
    while nq ne 0 do begin
      q = where(mlt lt 0., nq)
      if nq gt 0 then mlt[q] += 24.
    endwhile

    if ierr eq -4 then begin
      ; there were NaNs passed in
      mlt[qq] = !values.f_nan
      err = ierr
    endif
    return, mlt
  endelse
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertYMDHMS_v2
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt = MLTConvertYMDHMS_v2(yr,mo,dy,hr,mt,sc,mlon [, /MLT2mlon])
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------------------
;
  
;function MLTConvertYMDHMS_v2,yr,mo,dy,hr,mt,sc,mlon, height=height, order=order
function MLTConvertYMDHMS_v2, yr,mo,dy,hr,mt,sc, mlon, $
                           sstrace=sstrace, ssheight=ssheight, anti=anti, $
                           err=err, dday_bug=dday_bug, MLT2mlon=MLT2mlon

  npts = n_elements(yr)
  if npts ne 1 then begin
    mlt = yr*0.D
    for k=0,npts-1 do begin
      if n_elements(mlon) ne 1 then mtmp = mlon[k] else mtmp = mlon
      tmlt = MLTConvert_v2(yr[k],mo[k],dy[k],hr[k],mt[k],sc[k],mtmp, $
                            sstrace=sstrace, ssheight=ssheight, anti=anti, $
                            err=err, dday_bug=dday_bug, MLT2mlon=MLT2mlon)
      mlt[k] = tmlt
    endfor
  endif else begin
    mlt = MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlon, sstrace=sstrace, $
                        ssheight=ssheight, anti=anti, err=err, $
                        dday_bug=dday_bug, MLT2mlon=MLT2mlon)
  endelse

  return, mlt

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertYrsec_v2
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt = MLTConvertYrsec_v2(yr,yrsec,mlon [, /MLT2mlon])
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------------------
;

;function MLTConvertYrsec_v2,yr,yrsec,mlon, height=height, order=order
function MLTConvertYrsec_v2, yr,yrsec, mlon, $
                           sstrace=sstrace, ssheight=ssheight, anti=anti, $
                           err=err, dday_bug=dday_bug, MLT2mlon=MLT2mlon

  npts = n_elements(yrsec)
  if npts ne 1 then begin
    mlt = yrsec*0.D
    for k=0,npts-1 do begin

      if n_elements(yr) ne 1 then yrtmp = yr[k] else yrtmp = yr
      s = TimeYrSecToYMDHMS(yrtmp,mo,dy,hr,mt,sc,yrsec[k])

      if n_elements(mlon) ne 1 then mtmp = mlon[k] else mtmp = mlon
      tmlt = MLTConvert_v2(yrtmp,mo,dy,hr,mt,sc,mtmp, $
                            sstrace=sstrace, ssheight=ssheight, anti=anti, $
                            err=err, dday_bug=dday_bug, MLT2mlon=MLT2mlon)
      mlt[k] = tmlt
    endfor
  endif else begin
    s = TimeYrSecToYMDHMS(yr,mo,dy,hr,mt,sc,yrsec)
    mlt = MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlon, sstrace=sstrace, $
                        ssheight=ssheight, anti=anti, err=err, $
                        dday_bug=dday_bug, MLT2mlon=MLT2mlon)
  endelse

  return, mlt

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertEpoch_v2
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt = MLTConvertEpoch_v2(epoch,mlon [, /MLT2mlon])
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------------------
;

;function MLTConvertEpoch_v2,epoch,mlon, height=height, order=order

function MLTConvertEpoch_v2, epoch, mlon, $
                           sstrace=sstrace, ssheight=ssheight, anti=anti, $
                           err=err, dday_bug=dday_bug, MLT2mlon=MLT2mlon

  npts = n_elements(epoch)
  if npts ne 1 then begin
    mlt = epoch*0.D
    for k=0,npts-1 do begin

      s = TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,epoch[i])

      if n_elements(mlon) ne 1 then mtmp = mlon[k] else mtmp = mlon
      tmlt = MLTConvert_v2(yr[k],mo[k],dy[k],hr[k],mt[k],sc[k],mtmp, $
                            sstrace=sstrace, ssheight=ssheight, anti=anti, $
                            err=err, dday_bug=dday_bug, MLT2mlon=MLT2mlon)
      mlt[k] = tmlt
    endfor
  endif else begin
    s = TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,epoch)
    mlt = MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlon, sstrace=sstrace, $
                        ssheight=ssheight, anti=anti, err=err, $
                        dday_bug=dday_bug, MLT2mlon=MLT2mlon)
  endelse

  return, mlt
    
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       mlt_v2
;
; PURPOSE:
;       Legacy routine. Wrapper to compute Magnetic Local Time (MLT) using
;       AACGM-v2 coordinates and time specified using keywords or the
;       current AACGM-v2 time.
;       
; CALLING SEQUENCE:
;       mlt = mlt_v2(mlon [, year=yr] [, month=mo] [, day=dy] [, hour=hr] $
;                          [, minute=mt] [, second=sc] [, err=err] $
;                          [, sstrace=sstrace] [, ssheight=ssheight] $
;                          [, /MLT2mlon]
;
;    mlon       - AACGM-v2 magnetic longitude of desired location
;    date/time  - optional date and time, current AACGM-v2 time used by default
;                  so must be already set.
;    sstrace    - use field-line tracing option for reference longitude. Note,
;                  this is an advanced feature and can result in undefined
;                  results. NOT RECOMMENDED.
;    ssheight   - use a different height when computing the reference longitude
;                  of the subsolar point.
;    err        - error code. 0 for success.
;    anti       - use antisolar point for reference longitude. NOT RECOMMENDED.
;    MLT2mlon   - set keyword for inverse calculation. In this case mlon is
;                  MLT.
;     
;-----------------------------------------------------------------------------
;

function mlt_v2, mlon, $
                year=yr, month=mo, day=dy, hour=hr, minute=mt, second=sc, $
                sstrace=sstrace, ssheight=ssheight, anti=anti, err=err, $
                MLT2mlon=MLT2mlon

  ; check if AACGM time is set, need it to restore AACGM time after
  common AACGM_v2_Com

  ierr = 0
  mlon = reform(mlon)
  qq = where(finite(mlon, /nan), nqq)
  if nqq gt 0 then begin

    ierr = -4
    if nqq eq n_elements(mlon) then begin
      err = ierr
      return, mlon  ; just return all NaNs
    endif

    mlon[qq] = 0      ; set these to zero because we are changing them later
  endif

  tyr = 0 & tmo = 0 & tdy = 0
  thr = 0 & tmt = 0 & tsc = 0
  err = AACGM_v2_GetDateTime(tyr, month=tmo, day=tdy, $
                                  hour=thr, minute=tmt, second=tsc, /silent)

  if ~keyword_set(yr) then begin
    if (err ne 0) then begin
      print, "*****************************************************************"
      print, ""
      print, "ERROR: Date and Time are not currently set."
      print, ""
      print, "For MLT you MUST either specify the time with keywords OR use"
      print, " one of the functions below in order to set the"
      print, " date and time for the necessary coordinate transformations."
      print, ""
      print, " AACGM_v2_SetNow - use the current date and time, in UT."
      print, ""
      print, " AACGM_v2_SetDateTime(year, [...]) - set date and time."
      print, ""
      print, "*****************************************************************"
      err = -1
      return, !values.f_nan
    endif else begin
      yr = tyr & mo = tmo & dy = tdy
      hr = thr & mt = tmt & sc = tsc
    endelse
  endif else begin  ; user is passing in date/time so need to reset AACGM
    if ~keyword_set(mo) or ~keyword_set(dy) then begin
      print, "*****************************************************************"
      print, ""
      print, "ERROR: Year is specified but not Month and Day."
      print, ""
      print, "*****************************************************************"
      err = -2
      return, !values.f_nan
    endif

    if ~keyword_set(hr) then hr = 0
    if ~keyword_set(mt) then mt = 0
    if ~keyword_set(sc) then sc = 0
    e = AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,sc)
  endelse

  ret = MLTConvert_v2(yr,mo,dy,hr,mt,sc,mlon, $
                      sstrace=sstrace, ssheight=ssheight, anti=anti, err=err, $
                      MLT2mlon=MLT2mlon)

  ; restore AACGM date/time if necessary
  if ~keyword_set(yr) then e = AACGM_v2_SetDateTime(tyr,tmo,tdy,thr,tmt,tsc)

  ; times are all taken from AACGM now because linear interpolation, so
  ; no arrays of times here.

  if ierr eq -4 then begin
    ; there were NaNs passed in
    ret[qq] = !values.f_nan
    err = ierr
  endif

  return, ret
end

