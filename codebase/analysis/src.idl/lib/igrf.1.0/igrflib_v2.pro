;------------------------------------------------------------------------------
; IGRF library
;
; a collection of IDL routines intended to implement the IGRF magnetic field
; model.
;
; 20150122 SGS v0.0  Initial implementation of functions to read IGRF
;                    coefficient and produce magnetic field at a given
;                    location. Several coordinate transformations.
; 20150226 SGS v1.0  Added transformations between geodetic and geocentric
;                    coordinates.
;
; Public Functions:
;
; IGRF_compute
; IGRF_SetDateTime
; IGRF_GetDateTime
; IGRF_SetNow
; IGRF_Tilt
;
; Private Functions:
;
; init_common
; IGRF_loadcoefs
; IGRF_Plm
; IGRF_interpolate_coefs
; IGRF_msg_notime
;
; AACGM_v2_errmsg
; AACGM_v2_Newval
; AACGM_v2_RK45
;
; Coordinate Transformations:
;
; sph2car
; car2sph
; bspcar
; bcarsp
; geo2mag
; mag2geo
; geod2geoc
; plh2xyz
; geoc2geod
;
;------------------------------------------------------------------------------
;

;*-----------------------------------------------------------------------------
;
; NAME:
;       init_common
;
; PURPOSE:
;       Initialize common variables for IGRF routines. Called from many IGRF
;        functions that rely on various constants.
;
; CALLING SEQUENCE:
;       init_common, err=err
;     
;     Keyword:
;       error code
;
;+-----------------------------------------------------------------------------
pro init_common, err=err
  common IGRF_v2_Com, IGRF_datetime, IGRF_coef_set, IGRF_svs, IGRF_coefs, $
                      IGRF_file, IGRF_order, IGRF_maxnyr, IGRF_maxk, $
                      RE, DTOR, geopack, IGRF_FIRST_EPOCH, IGRF_LAST_EPOCH, $
                      IGRF_nmx

  ; initial globals
; IGRF_file   = "igrf13coeffs.txt"    ; using environment variable in v2.3
  IGRF_order  = 13
  IGRF_maxnyr = 100
  IGRF_maxk   = (IGRF_ORDER+1)*(IGRF_ORDER+1)
  geopack     = {ctcl:0.d, ctsl:0.d, stcl:0.d, stsl:0.d, ct0:0.d, st0:0.d, $
                  cl0:0.d, sl0:0.d}
; DTOR        = !const.pi/180.d
  DTOR        = !dpi/180.d
  RE          = 6371.2d     ; magnetic reference spherical radius from IGRF
;  IGRF_FIRST_EPOCH = 1900  ; these get set in IGRF_loadcoeffs
;  IGRF_LAST_EPOCH  = 2015

  IGRF_coef_set = dblarr(IGRF_maxnyr,IGRF_maxk) ; all the coefficients
  IGRF_svs      = dblarr(IGRF_maxk)             ; secular variations
  IGRF_coefs    = dblarr(IGRF_maxk)             ; interpolated coefficients

  IGRF_datetime = {year:-1, month:-1, day:-1, hour:-1, minute:-1, $
                    second:-1, dayno:-1, daysinyear:-1, fyear:-1.d}

  ; load all of the coefficients
  IGRF_loadcoeffs, err=err
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_loadcoeffs
;
; PURPOSE:
;       Load the entire set of spherical harmonic coefficients from the given
;       file.
; 
;  Read the in the coefficients. Note that I am using the same ordering as
;  is used in the AACGM code. That is,
; 
;   l    0  1  1  1  2  2  2  2  2  3  3  3  3  3  3  3  4  4  4  4  4 ...
;   m    0 -1  0  1 -2 -1  0  1  2 -3 -2 -1  0  1  2  3 -4 -3 -2 -1  0 ...
; 
;  C & IDL index: k = l * (l+1) + m
; 
;   k    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 ...
;
; CALLING SEQUENCE:
;       err = IGRF_loadcoeffs(filename);
;     
;     Input Arguments:  
;       filename      - name of file which contains IGRF coefficients; default
;                       is current IGRF model: igrf13coeffs.txt
;
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

pro IGRF_loadcoeffs, file=file, debug=debug, err=err
  common IGRF_v2_Com

  IGRF_file = getenv('IGRF_COEFFS')
  if (strlen(IGRF_file) eq 0) then begin
    ; IGRF coefficient environment variable is not set
    print, ''
    print, '================================================================='
    print, 'Environment variable IGRF_COEFFS is not set.'
    print, 'You MUST set this variable to the fully qualified path of the '
    print, 'IGRF coefficient file that was part of the AACGM-v2 distribution.'
    print, '================================================================='
    print, ''
    exit
  endif

  dgrf   = intarr(IGRF_maxnyr)
  epoch  = intarr(IGRF_maxnyr)

  Slm    = dblarr(IGRF_maxk)
  fctrl  = dblarr(2*IGRF_order+1)
  dfc    = dblarr(2*IGRF_order)

  if keyword_set(debug) then begin
    print, "IGRF_loadcoeffs\n"
    print, "Schmidt quasi-normalization factors\n"
    print, "===================================\n\n"
  endif

  ; factorial
  fctrl[0] = 1.d
  fctrl[1] = 1.d
  for k=2, 2*IGRF_order do fctrl[k] = k*fctrl[k-1]

  ; double factorial
  dfc[1] = 1.d
  for k=3, 2*IGRF_order-1, 2 do dfc[k] = dfc[k-2]*k

  for l=0, IGRF_order do begin
    for m=0, l do begin
      k = l * (l+1) + m;      /* 1D index for l,m */
      n = l * (l+1) - m;      /* 1D index for l,m */

      if (m gt 0) then fac = 2 else fac = 1
      ;/* Davis 2004; Wertz 1978 recursion
      ;Slm[k] = Slm[n] = sqrt(fac*fctrl[l-m]/fctrl[l+m])*dfc[2*l-1]/fctrl[l-m];
      ;*/
      ;/* Winch 2004 */
      tval = sqrt(fac*fctrl[l-m]/fctrl[l+m])
      Slm[k] = tval
      Slm[n] = tval

      if keyword_set(debug) then begin
        print, l, m,k,fctrl[l-m],fctrl[l+m],Slm[k], format='(3(x,d2),3(x,e))'
        print, l,-m,k,fctrl[l-m],fctrl[l+m],Slm[k], format='(3(x,d2),3(x,e))'
      endif
    endfor
  endfor

  ;----------------------------------------------------------------------------
  ;
  ; Read coefficients from file into array
  ;
  ;----------------------------------------------------------------------------

  if (size(file_search(IGRF_file)))[0] eq 0 then begin
    print, ''
    print, 'File '+strtrim(file,2)+' not found.'
    print, ''
    err = -1
    return
  endif

  openr, fp, IGRF_file, /get_lun

  ; read two header lines
  h1 = '' & h2 = ''
  readf, fp, h1
  readf, fp, h2

  ; get next line
  line = ''
  readf, fp, line
  line = strtrim(line,2)
  i = 0
  ; count how many D/IGRF years
  while (1) do begin
;    ipos = strpos(line, 'GRF')
    ipos = strpos(line, 'G')
    if ipos ge 0 then begin
      if strmid(line,ipos-1,1) eq 'D' then dgrf[i] = 1
;      line = strtrim(strmid(line, ipos+3),2)
      ipos = strpos(line, ' ')
      line = strtrim(strmid(line, ipos+1),2)
      i++
    endif else break
  endwhile
  nyear = i

; dgrf   = dgrf[0:nyear-1]
; epoch  = epoch[0:nyear-1]
; coeffs = coeffs[0:nyear-1,*]

  if nyear gt IGRF_maxnyr then begin
    print, ''
    print, 'Too many years in file: '+strtrim(nyear,2)
    print, ''
    err = -2
    return
  endif

  ; get next line, which should have the following format:
  ;
  ; "g/h n m 1900.0 1905.0 ... 2010.0 2010-15"
  readf, fp, line
  ipos = strpos(line, 'm')
  line = strtrim(strmid(line, ipos+1),2)

  ; get the years, which should be 5-year integer epochs...
  for m=0, nyear-1 do begin
    ipos = strpos(line, ' ')
    if ipos lt 0 then begin
      epoch[m] = fix(line)
    endif else begin
      epoch[m] = fix(strmid(line,0,ipos))
      line = strtrim(strmid(line, ipos+1),2)
    endelse
  endfor

  ; set epoch range based on what is in the file
  q = where(epoch ne 0, nq)
  IGRF_FIRST_EPOCH = fix(epoch[q[0]])
  IGRF_LAST_EPOCH  = fix(epoch[q[nq-1]])

;  /*
;   *==========================================================================
;   *
;   * Read the in the coefficients. Note that I am using the same ordering as
;   * is used in the AACGM code. That is,
;   *
;   *  l    0  1  1  1  2  2  2  2  2  3  3  3  3  3  3  3  4  4  4  4  4 ...
;   *  m    0 -1  0  1 -2 -1  0  1  2 -3 -2 -1  0  1  2  3 -4 -3 -2 -1  0 ...
;   *
;   * C & IDL index: k = l * (l+1) + m
;   *
;   *  k    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 ...
;   *
;   *==========================================================================
;   */

;  For sum_{l=0}^{order} sum_{m=-l}^{l} there are:
;      sum_{l=0}^{order} (2l + 1) = order+1 + 2*(sum_{l=0}^{order} l)
;                                 = order+1 + 2*(0 + 1 + 2 + ... order)
;                                 = order+1 + 2*((order+1)/2 * order)
;                                 = order^2 + 2*order + 1 coefficients
;  since we there is no l=0 term it is just order^2 + 2*order coefficients
;      
;  /* NOTE that for IGRF there is no l=0 term in the coefficient file */
;for l=0, maxorder do begin

; formatted input sucks in IDL...
;ch = '' & ll = 0 & mm = 0
;fmt = '(a1,i1,x,i1,'+strtrim(nyear+1,2)+'(x,f))'
;fmt = '(a3,i1,x,i1,x,f,f)'
;for i=0,nyear-1 do fmt += ',x,f'
;fmt += ',x,f)'

  coef = dblarr(nyear)
  tsvs = 0.

  for l=1, IGRF_order do begin
    for i=0,2*l do begin
      ; convert ordering of indices to how they are listed in the file...
      if i eq 0 then $
        m = 0 $
      else begin
        m = fix((i-1)/2)+1
        if i mod 2 eq 0 then m = -m
      endelse
      k = l * (l+1) + m   ; 1D index for l,m

      readf, fp, line   ; get the whole line (because formatted input in IDL sux
      for j=1,3 do begin; skip over g/h and l,m
        line = strtrim(strmid(line, strpos(line,' ')), 2)
      endfor
      ; extract coefficients for each epoch
      for j=0,nyear-1 do begin
        IGRF_coef_set[j,k] = double(strmid(line,0,strpos(line, ' '))) * Slm[k]
        line = strtrim(strmid(line, strpos(line,' ')), 2)
      endfor
      ; extract coefficient for secular varation
      IGRF_svs[k] = double(strtrim(line,2)) * Slm[k]
    endfor
  endfor

  free_lun, fp

  if keyword_set(debug) then begin
    for n=0, nyear-1 do $
      print, epoch[n], IGRF_coef_set[n,0]
  endif

end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_Plm
;
; PURPOSE:
;       Internal function to compute array of Gaussian Normalized Associated
;       Legendre functions and the corresponding derivatives.
;
; CALLING SEQUENCE:
;       err = IGRF_Plm(theta, order, plmval, dplmval);
;     
;     Input Arguments: 
;       theta         - co-latitude in radians
;       order         - order of expansion, should NOT exceed ORDER
;
;     Output Arguments:
;       plmval        - pointer to array for storage of values
;       dplmval       - pointer to array for storage of derivative values
;
;     Return Value:
;       error code
;
;     Notes: I am using array indexing similar to that used for m=-l to l,
;            but here m=0 to l, so the arrays are too big and there are no
;            values stored in locations for m<0. Probably should fix that...
;
;       values are stored in a 1D array of dimension (order+1)^2. The
;       indexing scheme used is:
;
;             g  h  g  g  h  h  g  g  g  h  h  h  g  g  g  g  h  h  h  h  h ...
;        l    0  1  1  1  2  2  2  2  2  3  3  3  3  3  3  3  4  4  4  4  4 ...
;        m    0 -1  0  1 -2 -1  0  1  2 -3 -2 -1  0  1  2  3 -4 -3 -2 -1  0 ...
;C & IDL j    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 ...
;FORTRAN j    1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 ...
; 
;+-----------------------------------------------------------------------------

pro IGRF_Plm, theta, order, plmval, dplmval
  common IGRF_v2_Com

  if (order gt IGRF_order) then return

  st = sin(theta)
  ct = cos(theta)

  plmval[0]  = 1.;   /* 0,0 */
  dplmval[0] = 0.;   /* 0,0 */
  ;* compute values of P^{l,l} and dP^{l,l}/dtheta */
  for l=1, order do begin
    k = l * (l+1) + l;    /* l = m */
    n = (l-1) * l + l-1;  /* l-1 = m-l, i.e., previous l=m */
    ;/* Davis 2004; Wertz 1978 recursion
    ;plmval[k]  = plmval[n]*st;
    ;dplmval[k] = dplmval[n]*st + plmval[n]*ct;
    ;*/
    ;/* numerical recipies in C */
;   a = 1-2*l;    /* reverse order to remove Condon-Shortley phase */
    a = 2*l-1;
    plmval[k]  = a*plmval[n]*st;
    dplmval[k] = a*(dplmval[n]*st + plmval[n]*ct);
  endfor

  plmval[2]  =  ct; /* 1,0 */
  dplmval[2] = -st; /* 1,0 */
  ;/* compute values of P^{l,m} and dP^{l,m}/dtheta */
  for l=2, order do begin
    for m=0, l-1 do begin
      k = l * (l+1) + m;        /* l,m */
      n = (l-1) * l + m;        /* l-1,m */
      p = (l-2) * (l-1) + m;    /* l-2,m */
      ;/* Davis 2004; Wertz 1978 recursion
      ;kfac = ((l-1)*(l-1) - m*m)/((double)(2*l-1)*(2*l-3));
      ;plmval[k]  = ct*plmval[n] - kfac*plmval[p];
      ;dplmval[k] = ct*dplmval[n] -st*plmval[n] - kfac*dplmval[p];
      ;*/

      ;/* numerical recipies in C */
      a = 2*l-1;
      ;/* This works but NOT if plmval[p] is NAN!!!!
      ;*
      ;b = (m == l-1) ? 0 : l+m-1;
      ;plmval[k]  = (a*ct*plmval[n] - b*plmval[p])/(l-m);
      ;dplmval[k] = (a*(ct*dplmval[n] - st*plmval[n]) - b*dplmval[p])/(l-m);
      ;*/
      if (m eq l-1) then begin
        plmval[k]  = a*ct*plmval[n]/(l-m);
        dplmval[k] = a*(ct*dplmval[n] - st*plmval[n])/(l-m);
      endif else begin
        b = l+m-1;
        plmval[k]  = (a*ct*plmval[n] - b*plmval[p])/(l-m);
        dplmval[k] = (a*(ct*dplmval[n] - st*plmval[n]) - b*dplmval[p])/(l-m);
      endelse
    endfor
  endfor

end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_compute
;
; PURPOSE:
;       User function to compute IGRF magnetic field at lat/lon and distance.
;
; CALLING SEQUENCE:
;       err = IGRF_compute(r, theta, phi, Br, Btheta, Bphi);
;     
;     Input Arguments: 
;       r             - geocentric distance in km
;       theta         - co-latitude in radians
;       phi           - longitude in radians
;
;     Output Arguments:
;       Br            - pointer to field in radial direction
;       Btheta        - pointer to field in co-latitude direction
;       Bphi          - pointer to field in longtitude direction
;
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

function IGRF_compute, rtp, debug=debug, err=err
  common IGRF_v2_Com

  if keyword_set(debug) then begin
    print,"IGRF_compute"
  endif

  ;* no date/time set so bail */
  if (IGRF_datetime.year lt 0) then begin
    IGRF_msg_notime
    err = -128
    return, [!values.f_nan,!values.f_nan,!values.f_nan]
  endif

  ;* Must avoid singularity at the poles (dividing by sin(theta) later) */
  theta = rtp[1]
  st = sin(theta)
  if (abs(st) lt 1e-15) then $
    if st lt 0. then theta += 1e-15 else theta -= 1e-15

  ;* Compute the values of the Legendre Polynomials, and derivatives */
  plmval  = dblarr(IGRF_maxk)
  dplmval = dblarr(IGRF_maxk)
  IGRF_Plm, theta,IGRF_nmx,plmval,dplmval

  aor = 1.d/rtp[0]    ;* r is in units of RE to be consistent with geopack, */
                      ;* we want RE/r */

  afac = aor*aor

  ;* array of trig functions in phi for faster computation */
  cosm_arr = cos(dindgen(IGRF_order+1)*rtp[2])
  sinm_arr = sin(dindgen(IGRF_order+1)*rtp[2])

  brtp = dblarr(3)

  for l=1, IGRF_nmx do begin  ; no l = 0 term in IGRF
    tbrtp = dblarr(3)
    for m=0, l do begin
      k = l*(l+1) + m;  /* g */
      n = l*(l+1) - m;  /* h */

      tbrtp[0] += (IGRF_coefs[k]*cosm_arr[m] + IGRF_coefs[n]*sinm_arr[m]) * $
              plmval[k];
      tbrtp[1] += (IGRF_coefs[k]*cosm_arr[m] + IGRF_coefs[n]*sinm_arr[m]) * $
              dplmval[k];
      tbrtp[2] += (-IGRF_coefs[k]*sinm_arr[m] + IGRF_coefs[n]*cosm_arr[m]) * $
              m*plmval[k];
    endfor
    afac *= aor;

    brtp[0] += afac*(l+1)*tbrtp[0];
    brtp[1] -= afac*tbrtp[1];
    brtp[2] -= afac*tbrtp[2];
  endfor

  brtp[2] /= sin(theta);

  return, brtp
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_interpolate_coefs
;
; PURPOSE:
;       Function to compute interpolated coefficients.
;
; CALLING SEQUENCE:
;       err = IGRF_interpolate_coefs();
;     
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

pro IGRF_interpolate_coefs, debug=debug, err=err
  common IGRF_v2_Com

  if keyword_set(debug) then begin
    print, "** TIME INTERPOLATION **"
  endif

  ;* NOTE: FORTRAN code allows 10-year extrapolation beyond last epoch.
  ;* Here we are limiting to only 5 */
  if (IGRF_datetime.fyear lt IGRF_FIRST_EPOCH or $
      IGRF_datetime.fyear gt IGRF_LAST_EPOCH + 5) then begin
    ;/* reset date */
    IGRF_datetime.year   = -1
    IGRF_datetime.month  = -1
    IGRF_datetime.day    = -1
    IGRF_datetime.hour   = -1
    IGRF_datetime.minute = -1
    IGRF_datetime.second = -1
    IGRF_datetime.dayno  = -1
    IGRF_datetime.daysinyear = -1

    print, format='(a,i4,a,i4)', $
          "Date range for current IGRF model is: ", IGRF_FIRST_EPOCH, " to ", $
          IGRF_LAST_EPOCH+5
    err = -3

    return
  endif

  myear = IGRF_datetime.year/5*5;                 /* epoch year */
  if (IGRF_datetime.year lt 1995) then $
    IGRF_nmx = 10 else IGRF_nmx = 13  ; expnsn order

  i = (myear - IGRF_FIRST_EPOCH)/5;       /* index of first set of coefs */

  if (IGRF_datetime.fyear lt IGRF_LAST_EPOCH) then begin
    ;* interpolate bounding coefficients */
    for l=1, IGRF_nmx do begin    ;/* no l = 0 term in IGRF */
      for m=-l, l do begin
        k = l * (l+1) + m;      /* SGS: changes indexing */
        IGRF_coefs[k] = IGRF_coef_set[i,k] + (IGRF_datetime.fyear-myear)* $
                        (IGRF_coef_set[i+1,k]-IGRF_coef_set[i,k])/5
      endfor
    endfor
  endif else begin
    ;* use secular varation */
    for l=1, IGRF_nmx do begin    ;/* no l = 0 term in IGRF */
      for m=-l, l do begin
        k = l * (l+1) + m;      /* SGS: changes indexing */
        IGRF_coefs[k] = IGRF_coef_set[i,k] + (IGRF_datetime.fyear-myear)* $
                        IGRF_svs[k]
      endfor
    endfor
  endelse

  ;* compute the components of the unit vector EzMag in geographic coordinates:
  ;* sin(theta0)*cos(lambda0), sin(theta0)*sin(lambda0)
  ;*/

  ;* C & IDL index: k = l * (l+1) + m */
  g10 = -IGRF_coefs[2]; /* 1*2+0 = 2 */
  g11 =  IGRF_coefs[3]; /* 1*2+1 = 3 */
  h11 =  IGRF_coefs[1]; /* 1*2-1 = 1 */

  sq  = g11*g11 + h11*h11;

  sqq = sqrt(sq);
  sqr = sqrt(g10*g10 + sq);

  geopack.sl0  = -h11/sqq;
  geopack.cl0  = -g11/sqq;
  geopack.st0  = sqq/sqr;
  geopack.ct0  = g10/sqr;

  geopack.stcl = geopack.st0*geopack.cl0;
  geopack.stsl = geopack.st0*geopack.sl0;
  geopack.ctsl = geopack.ct0*geopack.sl0;
  geopack.ctcl = geopack.ct0*geopack.cl0;

  if keyword_set(debug) then begin
    print, "sl0  = ", geopack.sl0
    print, "cl0  = ", geopack.cl0
    print, "st0  = ", geopack.st0
    print, "ct0  = ", geopack.ct0
    print, "stcl = ", geopack.stcl
    print, "stsl = ", geopack.stsl
    print, "ctsl = ", geopack.ctsl
    print, "ctcl = ", geopack.ctcl
  endif

  err = 0

  return
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_SetDateTime
;
; PURPOSE:
;       Function to set date and time. MUST be called at least once BEFORE
;       any calls to IGRF functions.
;
; CALLING SEQUENCE:
;       err = IGRF_SetDateTime(year, month, day, hour, minute, second);
;     
;     Input Arguments:  
;       year          - year [1900-2025)
;       month         - month of year [01-12]
;       day           - day of month [01-31]
;       hour          - hour of day [00-24]
;       minute        - minute of hour [00-60]
;       second        - second of minute [00-60]
;
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

function IGRF_SetDateTime, year, month, day, hour, minute, second

  common IGRF_v2_Com

  err=0
  if n_elements(IGRF_file) eq 0 then begin
    init_common, err=err
    if (err) then return, err
  endif

  ; set defaults if not all parameters are passed in
  np = n_params()
  if np lt 6 then second = 0
  if np lt 5 then minute = 0
  if np lt 4 then hour   = 0
  if np lt 3 then day    = 1
  if np lt 2 then month  = 1
  if np lt 1 then begin
    return, -1
  endif

  days = -1
  doy  = AACGM_v2_Dayno(year,month,day, days=days)
  fyear = double(year) + ((doy-1) + $ ; SGS: int year -> rounding errors
            (hour + (minute + second/60.)/60.)/24.) / days

  if (fyear lt IGRF_FIRST_EPOCH or fyear gt IGRF_LAST_EPOCH+5) then begin
    print, ''
    print, 'Date range for IGRF13 is '+strtrim(IGRF_FIRST_EPOCH,2)+'-'+$
                                       strtrim(IGRF_LAST_EPOCH+5,2)
    print, ''
    return, -1
  endif

  IGRF_datetime.year       = year
  IGRF_datetime.month      = month
  IGRF_datetime.day        = day
  IGRF_datetime.hour       = hour
  IGRF_datetime.minute     = minute
  IGRF_datetime.second     = second
  IGRF_datetime.dayno      = doy
  IGRF_datetime.daysinyear = days

  ; interpolate coefficients if time has changed
  if (fyear ne IGRF_datetime.fyear) then begin
    IGRF_datetime.fyear = fyear
    IGRF_interpolate_coefs, err=err
  endif

  return, err

end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_GetDateTime
;
; PURPOSE:
;       Function to get date and time.
;
; CALLING SEQUENCE:
;       err = IGRF_GetDateTime(year, month, day, hour, minute, second, dayno);
;     
;     Output Arguments (integer pointers):  
;       year          - year [1900-2025)
;       month         - month of year [01-12]
;       day           - day of month [01-31]
;       hour          - hour of day [00-24]
;       minute        - minute of hour [00-60]
;       second        - second of minute [00-60]
;       dayno         - day of year [01-366]
;
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

function IGRF_GetDateTime, year, month=month, day=day, $
                           hour=hour, minute=minute, second=second, $
                           dyno=dayno, silent=silent

  common IGRF_v2_Com

  if (n_elements(igrf_v2_datetime) eq 0) then begin
    if not keyword_set(silent) then $
      print, "Date and Time are not currently set"
    return, -1
  endif

  year   = igrf_date.year
  month  = igrf_date.month
  day    = igrf_date.day
  hour   = igrf_date.hour
  minute = igrf_date.minute
  second = igrf_date.second
  dyno   = igrf_date.dayno

  return, 0

end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_SetNow
;
; PURPOSE:
;       Function to set date and time to current computer time in UT.
;
; CALLING SEQUENCE:
;       err = IGRF_SetNow();
;     
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

function IGRF_SetNow
  common IGRF_v2_Com

  if n_elements(IGRF_file) eq 0 then begin
    init_common, err=err
    if (err) then return, err
  endif

  ; use current time (in UT)
  caldat, systime(/julian, /utc), month,day,year, hour,minute,second

  days = -1
  doy  = AACGM_v2_Dayno(year,month,day, days=days)
  fyear = double(year) + ((doy-1) + $ ; SGS: int year -> rounding errors
            (hour + (minute + second/60.)/60.)/24.) / days

  if (fyear lt 1590. or fyear ge 2025.) then begin
    print, ''
    print, 'Date range for GUFM1/IGRF13 is 1590-2025'
    print, ''
    return, -1
  endif

  IGRF_datetime.year       = year
  IGRF_datetime.month      = month
  IGRF_datetime.day        = day
  IGRF_datetime.hour       = hour
  IGRF_datetime.minute     = minute
  IGRF_datetime.second     = second
  IGRF_datetime.dayno      = doy
  IGRF_datetime.daysinyear = days

  ; interpolate coefficients if time has changed
  if (fyear ne IGRF_datetime.fyear) then begin
    IGRF_datetime.fyear = fyear
    IGRF_interpolate_coefs, err=err
  endif

  return, err

end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_Tilt
;
; PURPOSE:
;       Compute the dipole tilt angle at the given time based on the IGRF
;       magnetic field model.
;
; CALLING SEQUENCE:
;       tilt = IGRF_Tilt(year, month, day, hour, minute, second);
;     
;+-----------------------------------------------------------------------------

function IGRF_Tilt, yr, mo, dy, hr, mt, sc

  common IGRF_v2_Com, IGRF_datetime, IGRF_coef_set, IGRF_svs, IGRF_coefs, $
                      IGRF_file, IGRF_order, IGRF_maxnyr, IGRF_maxk, $
                      RE, DTOR, geopack, IGRF_FIRST_EPOCH, IGRF_LAST_EPOCH, $
                      IGRF_nmx

  IGRF_SetDateTime,yr,mo,dy,hr,mt,sc

  rad    = double(57.295779513)
  dtwopi = double(360.)

  dd   = AstAlg_dday(dy,hr,mt,sc)
  jd   = AstAlg_jde(yr,mo,dd)
  dec  = AstAlg_solar_declination(jd)*DTOR
  sras = AstAlg_solar_right_ascension(jd)*DTOR

  s1 = cos(sras) * cos(dec)
  s2 = sin(sras) * cos(dec)
  s3 = sin(dec)

  dayno = AACGM_v2_Dayno(yr,mo,dy)

  ; need Greenwich Mean Sidereal Time
  ; SGS: seems like this should be somewhere in astalg.c, but can't find it
  fday = (double(hr)*3600. + double(mt)*60.+double(sc))/86400.
  dj   = (double(yr) - 1900.)*365 + (double(yr) - 1901)/4. + double(dayno) - $
                      .5 + double(fday)
  d__1 = dj*0.9856473354 + 279.690983 + fday*360. + 180.
  ; SGS: double modulus
  q = d__1/dtwopi
  if (q ge 0) then q =  floor(q) $
  else             q = -floor(-q)
  gst  = (d__1 - dtwopi*q)/rad

  sgst = sin(gst)
  cgst = cos(gst)

  d1 = geopack.stcl * cgst - geopack.stsl * sgst
  d2 = geopack.stcl * sgst + geopack.stsl * cgst
  d3 = geopack.ct0;

  sps  = d1*s1 + d2*s2 + d3*s3

  return, asin(sps)/DTOR
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       IGRF_v2_errmsg
;
; PURPOSE:
;       Display error message because no date and time have been set.
;
; CALLING SEQUENCE:
;       IGRF_v2_errmsg(code);
;     
;+-----------------------------------------------------------------------------

pro IGRF_v2_errmsg, ecode

  print, ""
  print, "**************************************************************************"

  case ecode of
    0: begin  ;* no Date/Time set */
  print,"* IGRF ERROR: No Date/Time Set                                           *"
  print,"*                                                                        *"
  print,"* You must specifiy the date and time in order to use IGRF models. Before *"
  print,"* calling IGRF functions you must set the date and time to the integer    *"
  print,"* using the function:                                                     *"
  print,"*                                                                         *"
  print,"*   IGRF_SetDateTime(year,month,day,hour,minute,second);                  *"
  print,"*                                                                         *"
  print,"* or to the current computer time in UT using the function:               *"
  print,"*                                                                         *"
  print,"*   IGRF_SetNow();                                                        *"
  print,"*                                                                         *"
  print,"* subsequent calls to IGRF functions will use the last date and time      *"
  print,"* that was set, so update to the actual date and time that is desired.    *"
  end

    1: begin  ; Date/Time out of bounds */
  print, "* IGRF ERROR: Date out of bounds                                     *"
  print,"*                                                                        *"
  print,"* The current date range for GUFM1/IGRF13 is [1590-2025), which          *"
  print,"* includes the 5-year secular variation.                                 *"
  end
  endcase

  print, "**************************************************************************"
  print, ""
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       sph2car
;
; PURPOSE:
;       Converts spherical coordinates into Cartesian coordinates.
;
; CALLING SEQUENCE:
;       err = sph2car(r,theta,phi, x,y,z);
;     
;     Input Arguments:
;       r             - geocentric distance [RE, where RE=6371.2 km]
;       theta         - co-latitude [radians]
;       phi           - longitude [radians]
;
;     Output Arguments (pointers to type double):  
;       x             - Cartesian components
;       y
;       z
;
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

function sph2car, rtp

  sq = rtp[0]*sin(rtp[1])

  return, [sq*cos(rtp[2]), sq*sin(rtp[2]), rtp[0]*cos(rtp[1])]

end

;*-----------------------------------------------------------------------------
;
; NAME:
;       car2sph
;
; PURPOSE:
;       Converts Cartesian coordinates into spherical coordinates.
;
; CALLING SEQUENCE:
;       err = car2sph(x,y,z, r,theta,phi);
;
;     Input Arguments
;       x             - Cartesian components [RE]
;       y
;       z
;
;     Output Arguments:
;       r             - geocentric distance [RE]
;       theta         - co-latitude [radians]
;       phi           - longitude [radians]
;
;     Return Value:
;       error code
;
;     Note: at the poles (x=0 and y=0) it is assumed that phi=0
;
;+-----------------------------------------------------------------------------

function car2sph, xyz

  rtp = dblarr(3)

  sq = xyz[0]*xyz[0] + xyz[1]*xyz[1]
  rtp[0] = sqrt(sq + xyz[2]*xyz[2])
  
  if (sq eq 0.) then begin
    rtp[2] = 0.
    if (xyz[2] lt 0) then rtp[1] = !dpi else rtp[1] = 0.d
  endif else begin
    sq = sqrt(sq)
    rtp[2] = atan(xyz[1],xyz[0])
    rtp[1] = atan(sq,xyz[2])
    if (rtp[2] lt 0) then rtp[2] += 2*!dpi
  endelse

  return, rtp
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       bspcar
;
; PURPOSE:
;       Converts spherical field components to Cartesian components.
;
; CALLING SEQUENCE:
;       err = bspcar(theta,phi, br,btheta,bphi, bx,by,bz);
;
;     Input Arguments
;       theta         - colatitude of point [radians]
;       phi           - longitude of point [radians]
;       br            - radial component [nT]; radially positive
;       btheta        - colatitude component [nT]; southward positive
;       bphi          - longitude component [nT]; eastward positive
; 
;     Output Arguments:
;       bx            - Cartesian components [RE]
;       by
;       bz
;
;     Return Value:
;       error code
;
;+-----------------------------------------------------------------------------

function bspcar, theta,phi, brtp
  st = sin(theta)
  ct = cos(theta)
  sp = sin(phi)
  cp = cos(phi)
  be = brtp[0]*st + brtp[1]*ct

  bxyz = dblarr(3)

  bxyz[0] = be*cp - brtp[2]*sp
  bxyz[1] = be*sp + brtp[2]*cp
  bxyz[2] = brtp[0]*ct - brtp[1]*st

  return, bxyz
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       bcarsp
;
; PURPOSE:
;       Converts Cartesian field components into spherical components.
;
; CALLING SEQUENCE:
;       err = bcarsp(theta,phi, bx,by,bz, br,btheta,bphi);
;
;     Input Arguments
;       x,y,z         - Cartesian components of point
;       bx,by,bz      - Cartesian field components [nT]
;
;     Output Arguments:
;       br            - spherical field components [nT]
;       btheta
;       bphi
;
;     Return Value:
;       error code
;
;     Note: at the poles (theta=0 or pi) it is assumed that phi=0 and therefore
;           btheta=bx and bphi=by
;
;+-----------------------------------------------------------------------------

function bcarsp, xyz, bxyz

  rho2 = xyz[0]*xyz[0] + xyz[1]*xyz[1]
  r    = sqrt(rho2 + xyz[2]*xyz[2])
  rho  = sqrt(rho2)

  if (rho eq 0.) then begin
    cp = 1.
    sp = 0.
  endif else begin
    cp = xyz[0]/rho
    sp = xyz[1]/rho
  endelse

  ct = xyz[2]/r
  st = rho/r

  brtp = dblarr(3)

  brtp[0] = (xyz[0]*bxyz[0] + xyz[1]*bxyz[1] + xyz[2]*bxyz[2])/r
  brtp[1] = (bxyz[0]*cp + bxyz[1]*sp)*ct - bxyz[1]*st
  brtp[2] = bxyz[1]*cp - bxyz[0]*sp

  return, brtp
end


function geo2mag, xyzg
  common IGRF_v2_Com

  xyzm = dblarr(3)

  xyzm[0] = xyzg[0]*geopack.ctcl + xyzg[1]*geopack.ctsl - xyzg[2]*geopack.st0
  xyzm[1] = xyzg[1]*geopack.cl0  - xyzg[0]*geopack.sl0
  xyzm[2] = xyzg[0]*geopack.stcl + xyzg[1]*geopack.stsl + xyzg[2]*geopack.ct0

  return, xyzm
end

function mag2geo, xyzm
  common IGRF_v2_Com

  xyzg = dblarr(3)

  xyzg[0] = xyzm[0]*geopack.ctcl - xyzm[1]*geopack.sl0 + xyzm[2]*geopack.stcl
  xyzg[1] = xyzm[0]*geopack.ctsl + xyzm[1]*geopack.cl0 + xyzm[2]*geopack.stsl
  xyzg[2] = xyzm[2]*geopack.ct0  - xyzm[0]*geopack.st0

  return, xyzg
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       geod2geoc
;
; PURPOSE:
;       Convert from geodetic coordinates (as specified by WGS84) to
;       geocentric coordinates using algorithm from IGRF Fortran code.
; 
; CALLING SEQUENCE:
;       [r,theta,phi] = geod2geoc(lat,lon,alt)
;     
;     Input Arguments:  
;       lat,lon       - geodetic latitude and longitude [degrees N and E]
;       alt           - distance above sea level [km]
;
;     Return Value:
;       r             - radial distance from center of Earth [RE]
;       theta         - angle from north pole [radians]
;       phi           - azimuthal angle [radians]
;
;+-----------------------------------------------------------------------------

function geod2geoc, lat, lon, alt
  common IGRF_v2_Com

  a = 6378.1370d;             /* semi-major axis */
  f = 1.d/298.257223563d;     /* flattening */
  b = a*(1.d -f);             /* semi-minor axis */
  a2 = a*a;
  b2 = b*b;
  theta = (90.d -lat)*DTOR; /* colatitude in radians   */
  st = sin(theta)
  ct = cos(theta)
  one = a2*st*st
  two = b2*ct*ct
  three = one + two
  rho = sqrt(three);          /* [km] */
  r = sqrt(alt*(alt+2*rho) + (a2*one + b2*two)/three);    /* [km] */
  cod = (alt+rho)/r
  sod = (a2-b2)/rho *ct*st/r;

  rtp = dblarr(3)

  rtp[0] = r/RE;              /* units of RE */
  rtp[1] = acos(ct*cod - st*sod);
  rtp[2] = lon*DTOR;

  return, rtp
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Newval
;
; PURPOSE:
;       Advance position along magnetic field line by one step, i.e.,
;       numerical field-line tracing.
; 
; CALLING SEQUENCE:
;       k = AACGM_v2_Newval(xyz, dir, ds)
;     
;     Input Arguments:  
;       xyz           - Cartesian position
;       dir           - direction along field-line to trace
;       ds            - stepsize to take
;
;     Return value:
;       k[3]          - rate functions evaluated
;
;+-----------------------------------------------------------------------------
;*

function AACGM_v2_Newval, xyz, idir, ds

  rtp  = dblarr(3)
  brtp = dblarr(3)
  bxyz = dblarr(3)

  rtp = car2sph(xyz);                 /* convert to spherical coords */
  brtp = IGRF_compute(rtp);           /* compute the IGRF field here */
  bxyz = bspcar(rtp[1],rtp[2], brtp); /* convert field to Cartesian */

  bmag = sqrt(bxyz[0]*bxyz[0] + bxyz[1]*bxyz[1] + bxyz[2]*bxyz[2]);
  k = ds*idir*bxyz/bmag

  return, k
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_RK45
;
; PURPOSE:
;       Advance position along magnetic field line by one step, i.e.,
;       numerical field-line tracing using either a fixed stepsize RK4 method
;       or a Runge-Kutta-Fehlberg adaptive stepsize ODE solver.
; 
; CALLING SEQUENCE:
;       AACGM_v2_RK45, xyz, dir, ds, eps, fixed=fixed, max_ds=max_ds, RRds=RRds
;     
;     Input Arguments:  
;       xyz           - Cartesian position
;       dir           - direction along field-line to trace
;       ds            - stepsize to take
;
;     Keywords:
;       fixed         - set this keyword to do RK4 method with stepsize ds
;       max_ds        - maximum stepsize that is allowed, in units of RE
;       RRds          - set to use a maximum stepsize that is proportional
;                       to cube of the distance from the origin.
;
;     Return Value:
;       none          - position variables (x,y,z) are modified directly
;
; HISTORY:
;
; Revision 1.0  140610 SGS initial version
; Revision 2.0  150122 SGS modified to remove dependence on geopack and now
;                          pass in arrays for xyz
; 
; NOTES:
;
; geopack is no longer required
; geopack is required to be installed for this function to work
; 
;
;+-----------------------------------------------------------------------------
;*

pro AACGM_v2_RK45, xyz, idir, ds, eps, fixed=fixed, max_ds=max_ds, RRds=RRds, $
                                        verbose=verbose
  common IGRF_v2_Com

  ; if 'fixed' is set then just do straight RK4 and ds is spatial step size
  ;  in kilometers
  ; default is to do adapative step size where eps is error in km
  ; set max_ds to the maximum step size (in RE) to prevent too large step

  rtp  = dblarr(3)
  brtp = dblarr(3)
  bxzy = dblarr(3)

  ;* convert position to spherical coords */
  rtp = car2sph(xyz)

  ;* compute IGRF field in spherical coords */
  brtp = IGRF_compute(rtp)

  ;* convert field from spherical coords to Cartesian */
  bxyz = bspcar(rtp[1],rtp[2], brtp)

  ;* magnitude of field to normalize vector */
  bmag = sqrt(bxyz[0]*bxyz[0] + bxyz[1]*bxyz[1] + bxyz[2]*bxyz[2]);

  if keyword_set(fixed) then begin
    ;**************
    ;* RK4 Method *
    ;**************
    k1 = ds*idir*bxyz/bmag
    k2 = AACGM_v2_Newval(xyz + .5*k1, idir, ds)
    k3 = AACGM_v2_Newval(xyz + .5*k2, idir, ds)
    k4 = AACGM_v2_Newval(xyz + k3, idir, ds)

    xyz += (k1 + 2*k2 + 2*k3 + k4)/6.
  endif else begin
    ;************************
    ;* Adaptive RK45 method * 
    ;************************
    rr = eps+1; /* just to get into the loop */
    while (rr gt eps) do begin
      k1 = ds*idir*bxyz/bmag
      k2 = AACGM_v2_Newval(xyz + k1/4., idir,ds)
      k3 = AACGM_v2_Newval(xyz + 3.*k1/32. + 9.*k2/32., idir,ds)
      k4 = AACGM_v2_Newval(xyz + (1932.*k1 - 7200.d*k2 + 7296.d*k3)/ $
                            2197.d, idir,ds)
      k5 = AACGM_v2_Newval(xyz + 439.*k1/216. - 8.*k2 + 3680.*k3/513. - $
                            845.*k4/4104., idir,ds)
      k6 = AACGM_v2_Newval(xyz - 8.*k1/27. + 2.*k2 - 3544.*k3/2565. + $
                            1859.*k4/4104. - 11.*k5/40., idir,ds)
      w1 = xyz + 25.*k1/216. + 1408.*k3/2565. + 2197.*k4/4104. - k5/5.
      w2 = xyz + 16.*k1/135. + 6656.*k3/12825. + 28561.*k4/56430. - $
                            9.*k5/50. + 2.*k6/55.

;      rr = sqrt(total((w1-w2)*(w1-w2)))/ds
      rr = abs(w1 - w2)/ds
      rr = sqrt(total(rr*rr))
      if keyword_set(verbose) then print, 'diff: ',rr
;      if rr eq 0 then begin
      if rr gt 1e-16 then begin
        delt = 0.84 * (eps / RR)^0.25   ; this formula relates the difference
                                        ; in the local trucation errors to the
                                        ; global error of the solution. There
                                        ; are lots of assumptions here...

        if keyword_set(verbose) then print, 'delt: ', delt
        newds = ds * delt
        ds = newds
        ;* maximum stepsize is fixed to max_ds in units of Re */
        if keyword_set(max_ds) then ds = min([max_ds,ds])

        ; Setting this keyword uses a maximum stepsize that is proportional to
        ; the distance from the origin, i.e., the further away from the origin
        ; the larger the stepsize you can take to maintain the same accuracy.
        ; Note that this is the maximum stepsize, if the algorithm says it
        ; should be smaller, it will use the smaller.
        ; Maximum stepsize is r^3 * 50km, where r is in units of Re
;        if keyword_set(RRds) then   ds = min([50*rtp[0]*rtp[0]*rtp[0]/RE, ds])
        ds = min([50*rtp[0]*rtp[0]*rtp[0]/RE, ds])
      endif else begin
        ; it is possible for the two solutions to give the same answer, which
        ; would correspond to an infinitely large stepsize
        if keyword_set(max_ds) then ds = max_ds   ; limit the stepsize or
                                                  ; just leave it alone
      endelse
    endwhile

    ; we use the RK4 solution
    xyz = w1
    ; I would assume that using the higher order RK5 method is better, but
    ; there is the suggestion that using the RK4 solution guarantees accuracy
    ; while the RK5 does not. Apparently some texts are now suggesting using
    ; the RK5 solution...
  endelse
end


pro IGRF_msg_notime

  print, ""
  print, $
  "***************************************************************************"
  print, $
  "* IGRF ERROR: No Date/Time Set                                            *"
  print, $
  "*                                                                         *"
  print, $
  "* You must specifiy the date and time in order to use IGRF models. Before *"
  print, $
  "* calling IGRF functions you must set the date and time to the integer    *"
  print, $
  "* using the function:                                                     *"
  print, $
  "*                                                                         *"
  print, $
  "*   IGRF_SetDateTime(year,month,day,hour,minute,second);                  *"
  print, $
  "*                                                                         *"
  print, $
  "* or to the current computer time in UT using the function:               *"
  print, $
  "*                                                                         *"
  print, $
  "*   IGRF_SetNow();                                                        *"
  print, $
  "*                                                                         *"
  print, $
  "* subsequent calls to IGRF functions will use the last date and time      *"
  print, $
  "* that was set, so update to the actual date and time that is desired.    *"
  print, $
  "***************************************************************************"
  print, ""

end

;*-----------------------------------------------------------------------------
;
; NAME:
;       plh2xyz
;
; PURPOSE:
;       Convert from geodetic coordinates (as specified by WGS84) to geocentric
;       coordinates (RE = 6371.2 km) using an alternate method from wikipedia.
; 
; CALLING SEQUENCE:
;       [r,theta,phi] = geod2geoc(lat,lon,alt)
;     
;     Input Arguments:  
;       lat,lon       - geodetic latitude and longitude [degrees N and E]
;       alt           - distance above sea level [km]
;
;     Return Value:  
;       r             - radial distance from center of Earth [RE]
;       theta         - angle from north pole [radians]
;       phi           - azimuthal angle [radians]
;
;+-----------------------------------------------------------------------------

function plh2xyz, lat, lon, alt
  common IGRF_v2_Com

  a = 6378.1370d              ; semi-major axis
  f = 1.d/298.257223563d      ; flattening
  b = a*(1.d -f)              ; semi-minor axis
  ee = (2.d - f) * f

  st = sin(lat*DTOR)
  ct = cos(lat*DTOR)
  sp = sin(lon*DTOR)
  cp = cos(lon*DTOR)

  N = a / sqrt(1.d - ee*st*st)
  Nac = (N + alt) * ct

  x = Nac * cp
  y = Nac * sp
  z = (N*(1.d - ee)+alt) * st

  r = sqrt(Nac*Nac + z*z)
  t = acos(z/r)

  rtp = dblarr(3)

  rtp[0] = r/RE         ; units of RE
  rtp[1] = t
  rtp[2] = lon*DTOR

  return, rtp
end

;*-----------------------------------------------------------------------------
;
; NAME:
;       geoc2geod
;
; PURPOSE:
;       Convert from geocentric coordinates (RE = 6371.2 km) to geodetic
;       coordinates (as specified by WGS84) using algorithm from wikipedia.
; 
; CALLING SEQUENCE:
;       [lat,lon,h] = geoc2geod(lat,lon,r)
;     
;     Input Arguments:  
;       lat,lon       - geocentric latitude and longitude [degrees N and E]
;       r             - radial distance from center of Earth [RE]
;
;     Return Value:  
;       lat,lon       - geodetic latitude and longitude using WGS84 [radians]
;       h             - distance above sea level [km]
;
;+-----------------------------------------------------------------------------

function geoc2geod, lat,lon,r
  common IGRF_v2_Com

  a = 6378.1370d              ; semi-major axis
  f = 1.d/298.257223563d      ; flattening
  b = a*(1.d -f)              ; semi-minor axis
  ee = (2.d - f) * f
  e4 = ee*ee
  aa = a*a

  theta = (90.d - lat)*DTOR
  phi   = lon * DTOR

  st = sin(theta)
  ct = cos(theta)
  sp = sin(phi)
  cp = cos(phi)

  x = r*RE * st * cp
  y = r*RE * st * sp
  z = r*RE * ct

  k0i   = 1.d - ee
  pp    = x*x + y*y
  zeta  = k0i*z*z/aa
  rho   = (pp/aa + zeta - e4)/6.d
  s     = e4*zeta*pp/(4.d*aa)
  rho3  = rho*rho*rho
  t     = (rho3 + s + sqrt(s*(s+2*rho3)))^(1.d/3.d)
  u     = rho + t + rho*rho/t
  v     = sqrt(u*u + e4*zeta)
  w     = ee*(u + v - zeta)/(2.d*v)
  kappa = 1.d + ee*(sqrt(u+v+w*w) + w)/(u + v)

  dlat  = atan(z*kappa,sqrt(pp))/DTOR

  h = sqrt(pp + z*z*kappa*kappa)/ee * (1.d/kappa - k0i)

  return, [dlat,lon,h]
end

