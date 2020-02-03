;------------------------------------------------------------------------------
; AACGM user interface
;
; IDL user functions intended to fully exploit the functionality of
; the AACGM coordinates [Shepherd, 2014] including use of the AACGM
; coefficients and field line tracing
;
; 20140402 SGS v0.0  a simple modification to the existing AACGM IDL software,
;                    but includes additional features: linear interpolation and
;                    fieldline tracing.
; 20140410 SGS v0.1  modified to do interpolation in time; combined all
;                    functions into a single library.
; 20140618 SGS v0.2  more error checking and comments added; common blocks
;                    changed to use leo_ prefix
; 20140701 SGS v0.3  bug fix: fyear was integer instead of float.
;                    fixed common blocks and logic for when time and height
;                    interpolations must occur.
; 20140826 SGS v0.5  change to requiring data/time to be set using external
;                    functions (same as C version), rather than using keywords
; 20140918 SGS v1.0  change function names to _v2 for wider distribution
; 20150121 SGS v2.0  upgrades for IGRF12: 1900-2020. Do not allow times outside
;                    this range. Eliminate reliance on geopack DLMs by calling
;                    native IDL routines.
; 20150223 SGS       decoupling MLT from AACGM-v2 software.
; 20180425 EGT       replaced check against common block in cnvcoord_v2() with
;                    function call to aacgm_v2_getdatetime() so DLMs will work
;                    properly.
;
; Functions:
;
; cnvcoord_v2
; AACGM_v2_SetDateTime
; AACGM_v2_SetNow
; AACGM_v2_GetDateTime
;
;------------------------------------------------------------------------------
;

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_SetDateTime
;
; PURPOSE:
;       Allow user to set date and time for coordinate transformations.
;
; CALLING SEQUENCE:
;       err = AACGMSetDateTime_v2(year, [month], [day], [hour], $
;                                        [minute], [second])
;
;     Arguments:
;       year          - year [1900-2025)
;       month         - month [1-12]
;       day           - day [1-31]
;       hour          - hour [0-24]
;       minute        - minute [0-60]
;       second        - second [0-60]
;
; HISTORY:
;
; New function to provide compatibility with C functions. In order to use
; 5-year epoch coefficients use year=fix(year/5)*5 which will result in
; using the previous 5-year value.
;     
;+-----------------------------------------------------------------------------
;
function AACGM_v2_SetDateTime, year,month,day,hour,minute,second

  common AACGM_v2_Com
  common IGRF_v2_Com

  ; set defaults if not all parameters are passed in
  np = n_params()
  if np lt 6 then second = 0
  if np lt 5 then minute = 0
  if np lt 4 then hour   = 0
  if np lt 3 then day    = 1
  if np lt 2 then month  = 1
  if np lt 1 then return, -1

  days = -1
  doy  = AACGM_v2_Dayno(year,month,day, days=days)
  fyear = year + ((doy-1) + $
            (hour + (minute + second/60.)/60.)/24.) / days

  if n_elements(IGRF_FIRST_EPOCH) eq 0 then init_common
  if (fyear lt IGRF_FIRST_EPOCH or fyear ge IGRF_LAST_EPOCH+5) then begin
    print, ''
    print, 'Date range for AACGM-v2 is ',IGRF_FIRST_EPOCH,'-',IGRF_LAST_EPOCH+5
    print, ''
    return, -1
  endif

  aacgm_v2_datetime = {year:-1, month:-1, day:-1, hour:-1, minute:-1, $
                        second:-1, dayno:-1, daysinyear:-1}

  aacgm_v2_datetime.year       = year
  aacgm_v2_datetime.month      = month
  aacgm_v2_datetime.day        = day
  aacgm_v2_datetime.hour       = hour
  aacgm_v2_datetime.minute     = minute
  aacgm_v2_datetime.second     = second
  aacgm_v2_datetime.dayno      = doy
  aacgm_v2_datetime.daysinyear = days

  err = AACGM_v2_TimeInterp()

  return, err

end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_SetNow
;
; PURPOSE:
;       Set the date and time for coordinate transformations to the current
;       time, in UT.
;
; CALLING SEQUENCE:
;       err = AACGM_v2_SetNow()
;
; HISTORY:
;
; New function to provide compatibility with C functions. One of two ways to
; set the date/time for coordinate transformations.
;     
;+-----------------------------------------------------------------------------
;
function AACGM_v2_SetNow

  common AACGM_v2_Com
  common IGRF_v2_Com

  aacgm_v2_datetime = {year:-1, month:-1, day:-1, hour:-1, minute:-1, $
                        second:-1, dayno:-1, daysinyear:-1}

  ; use current time (in UT)
  caldat, systime(/julian, /utc), month,day,year, hour,minute,second

  days = -1
  doy  = AACGM_v2_Dayno(year,month,day, days=days)
  fyear = year + ((doy-1) + $
            (hour + (minute + second/60.)/60.)/24.) / days

  if n_elements(IGRF_FIRST_EPOCH) eq 0 then init_common
  if (fyear lt IGRF_FIRST_EPOCH or fyear ge IGRF_LAST_EPOCH+5) then begin
    print, ''
    print, 'Date range for AACGM-v2 is ',IGRF_FIRST_EPOCH,'-',IGRF_LAST_EPOCH+5
    print, ''
    return, -1
  endif

  aacgm_v2_datetime.year       = year
  aacgm_v2_datetime.month      = month
  aacgm_v2_datetime.day        = day
  aacgm_v2_datetime.hour       = hour
  aacgm_v2_datetime.minute     = minute
  aacgm_v2_datetime.second     = second
  aacgm_v2_datetime.dayno      = doy
  aacgm_v2_datetime.daysinyear = days

  err = AACGM_v2_TimeInterp()

  return, err

end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_GetDateTime
;
; PURPOSE:
;       Allow user to get date and time that are used for coordinate
;       transformations.
;
; CALLING SEQUENCE:
;       err = AACGM_v2_GetDateTime(year, [month=month], [day=day], $
;                             [hour=hour], [minute=minute], [second=second], $
;                             [dyno=dayno])
;
;     Keywords:
;       month         - month [1-12]
;       day           - day [1-31]
;       hour          - hour [0-24]
;       minute        - minute [0-60]
;       second        - second [0-60]
;       dyno          - day number [1-365]
;
; HISTORY:
;
; New function to provide compatibility with C functions. Set the keywords to
; variables in order to have them populated with the date and time values
; currently being used for coordinate transformations.
;     
;+-----------------------------------------------------------------------------
;
function AACGM_v2_GetDateTime, year, month=month, day=day, $
                              hour=hour, minute=minute, second=second, $
                              dyno=dayno, silent=silent

  common AACGM_v2_Com

  if (n_elements(aacgm_v2_datetime) eq 0) then begin
    if not keyword_set(silent) then $
      print, "Date and Time are not currently set"
    return, -1
  endif

  year   = aacgm_v2_datetime.year
  month  = aacgm_v2_datetime.month
  day    = aacgm_v2_datetime.day
  hour   = aacgm_v2_datetime.hour
  minute = aacgm_v2_datetime.minute
  second = aacgm_v2_datetime.second
  dayno  = aacgm_v2_datetime.dayno

  return, 0

end

;------------------------------------------------------------------------------
;
; NAME:
;       cnvcoord_v2
;
; PURPOSE:
;       user interface to AACGM coordinate transformations.
;
; CALLING SEQUENCE:
;       pos = cnvcoord_v2(inpos, [inlong], [height], [/GEO], [/TRACE], $
;                                     [/BAD_IDEA)
;
;     Note on input arguments:
;      the routine can be called either with a 3-element floating
;      point array giving the input latitude, longitude and height
;      or it can be called with 3 separate floating point values
;      giving the same inputs.
;      The input array can also be given in the form inpos(3,d1,d2,...)
;
;     Keywords:
;       geo           - set this keyword to convert from AACGM to geographic
;                       coordinates. The default is from geographic to AACGM
;       trace         - perform the slower but more accurate field line tracing
;                       to determine the coordinate conversion at all
;                       altitudes.  This feature is new and still in beta form.
;       allow_trace   - perform field line tracing for altitudes above 2000 km.
;                       The default is to throw an error if the altitude is
;                       above 2000 km. Because the tracing requires geopack
;                       to be installed, it is not acceptable to just load
;                       the required geopack functions.
;       bad_idea      - field line tracing is forced above 2000 km unless this
;                       keyword is set, in which case the coefficients will be
;                       used to extrapolate above the maximum altitude that
;                       is intended. Note that results can be nonsensical when
;                       using this option and you are acknowledging it by
;                       setting this keyword.
;
;     Output:  
;       pos           - a vector of the same form as the input with
;                       latitude and longitude specified in degrees and
;                       the distance from the origin in Re
;
; HISTORY:
;
; 20140827 SGS moved setting date/time via keywords to external functions,
;              similar to C version.
;     
;+-----------------------------------------------------------------------------
;

function cnvcoord_v2, in1,in2,in3, geo=geo, trace=trace, gcentric=gcentric, $
                            allow_trace=allow_trace, bad_idea=bad_idea, $
                            verbose=verbose
; SGS: these keywords work in the IDL version but not with the DLMS. It is
;      is possible to use them without the DLMS
;                            eps=eps, debug=debug, $
;                            use_geopack=use_geopack

  common AACGM_v2_Com
  common IGRF_v2_Com

  ; force user to explicitly set the date and time fields for calculations
;  if (n_elements(aacgm_v2_datetime) eq 0) then begin
  if (aacgm_v2_getdatetime(tmp) eq -1) then begin
    print, ""
    print, "ERROR: Date and Time are not currently set."
    print, ""
    print, "You MUST use one of the functions below in order to set the"
    print, " date and time for the coordinate transformations, at least once."
    print, ""
    print, " AACGM_v2_SetNow - use the current date and time, in UT."
    print, ""
    print, " AACGM_v2_SetDateTime(year, [...]) - set date and time."
    print, ""
    return, -1
  endif

;  if myear_v2 lt IGRF_FIRST_EPOCH or myear_v2 gt IGRF_LAST_EPOCH+5 then begin
;    print, 'year range is ',IGRF_FIRST_EPOCH,'-',IGRF_LAST_EPOCH+5
;    return, -1
;  endif

; ; original Baker code that allows for various inputs
; if (n_params() ge 3) then inp = float([in1,in2,in3]) $
; else inp = float(in1)

  ; SGS v2.3 change allows arrays of lat,lon,alt to be passed into function
  if (n_params() ge 3) then inp = double(transpose([[in1],[in2],[in3]])) $
  else inp = double(in1)

  if (n_elements(inp) mod 3 ne 0) then begin
    print,'input position must be fltarr(3) [lat,long,height]'
    return, -1
  end

  err = 0
  s0  = size(inp)
  tmp = reform(inp,3,n_elements(inp)/3)
  outpos = tmp
  s1 = size(tmp)

  for j = 0L,s1[2]-1 do begin
    invec   = tmp[0:2,j]
    outvec  = invec
    ret_val = AACGM_v2_Convert(invec[0],invec[1],invec[2], olat,olon,rad, $
                                geo=geo, trace=trace, bad_idea=bad_idea, $
                                allow_trace=allow_trace, gcentric=gcentric, $
                                verbose=verbose)
; SGS: comment out to work with DLMs that do not support these keywords
;                                eps=eps, debug=debug)
    outvec[0] = olat
    outvec[1] = olon
    outvec[2] = rad

    if (ret_val ne 0) then begin
      outvec[0] = !values.f_nan
      outvec[1] = !values.f_nan
      outvec[2] = !values.f_nan

;     print, "***", ret_val, invec, "***"
;     print,"cnvcoord_v2 error = ",ret_val,err
;     return, 0
    endif

    outpos[*,j] = outvec
  endfor
  outpos = reform(outpos,s0[1:n_elements(s0)-3])

  return, outpos
end

