;------------------------------------------------------------------------------
; AACGM library
;
; a collection of IDL routines intended to fully exploit the functionality of
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
; 20140709 SGS v0.4  changed longitude output of traced results to -180 to 180
;                    in order to be consistent with other outputs
; 20140826 SGS v0.5  change to requiring data/time to be set using external
;                    functions (same as C version), rather than using keywords.
;                    moved coefficient loading and time interpolation into new
;                    function, AACGM_v2_TimeInterp.
; 20140918 SGS v1.0  change function names to _v2 for wider distribution
; 20150121 SGS v2.0  replace geopack calls with internal calls; minor bug
;                    fixes; New release to coincide with new set of
;                    coefficients derived from IGRF12 model [1900-2020];
;                    Tracing functions (RK45 and Newval) now live in igrflib
;                    and do not rely on geopack
; 20150807 SGS v2.?  minor bug fix related to geocentric v geodetic coords.
;                    line 801: lat_in = p[1] -> lon_out = p[1]
;                    NO CHANGE in OUTPUT, so no release
;
; Functions:
;
; AACGM_v2_Convert
; AACGM_v2_LoadCoef
; AACGM_v2_Sgn
; AACGM_v2_Rylm
; AACGM_v2_Alt2CGM
; AACGM_v2_CGM2Alt
; AACGM_v2_ConvertGeoCoord
; AACGM_v2_Dayno
; AACGM_v2_Trace
; AACGM_v2_Trace_inv
; AACGM_v2_TimeInterp
;
; moved to igrflib.pro
; AACGM_v2_Newval
; AACGM_v2_RK45
;
;------------------------------------------------------------------------------
;

; define this common block right here
common AACGM_v2_Com, coef_v2,coefs_v2, cint_v2, $
                      height_old_v2, order_v2,kmax_v2, $
                      myear_v2, fyear_v2, myear_old_v2, fyear_old_v2, $
                      aacgm_v2_datetime

;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
; AACGM routines, non fieldline tracing
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Common blocks for the AACGM algorithm
;
; coefs_v2 holds both full sets of coefficients that bound the time
; coef_v2  holds the linearly interpolated set of coefficients
; cint_v2  holds the coefficients evalated at the altitude of interest
;
;+-----------------------------------------------------------------------------
;

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_LoadCoef
;
; PURPOSE:
;
;       Load two sets of AACGM coefficients that bracket the desired date.
;       Interpolation in time is performed at a later stage.
;
; CALLING SEQUENCE:
;       s = AACGM_v2_LoadCoef(units, order=ord)
;
;     Input Arguments:  
;       units         - logical unit numbers of coefficient files already open
;
;     Output Arguments:  
;       s             - return -1 if IO fails for whatever reason
;
;     Keywords:
;       order         - default is 10th order but allow for other orders
;
; HISTORY:
;
; added ability to interpolate in time with two sets of coefficients.
;     
;+-----------------------------------------------------------------------------
;

function AACGM_v2_LoadCoef, units, order=ord
  common AACGM_v2_Com

  on_ioerror, iofail

  if keyword_set(ord) then order_v2 = ord else order_v2 = 10  ; default order
  kmax_v2 = (order_v2+1)*(order_v2+1)

  ncoord = 3    ; xyz
  nquart = 5    ; quartic altitude fit coefficients
  nflag  = 2    ; 0: GEO->AACGM; 1: AACGM->GEO

  ; interpolation in time, so need two sets of coefficients....
  coefs_v2 = dblarr(2,kmax_v2,ncoord,nquart,nflag)

  temp_coef = dblarr(kmax_v2,ncoord,nquart,nflag)
  readf, units[0],temp_coef
  coefs_v2[0,*,*,*,*] = temp_coef

  readf, units[1],temp_coef
  coefs_v2[1,*,*,*,*] = temp_coef

  coef_v2 = dblarr(kmax_v2,ncoord,nquart,nflag)
  cint_v2 = dblarr(kmax_v2,ncoord,nflag)
; leo_first_coeff_old = -1.

; sol_dec_old = 0       ; what the hell are these used for?!
; told = 1e12
; mslon1=0
; mslon2=0

  return, 0

iofail:
  return, -1

end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Sgn
;
; PURPOSE:
;       return the signed quantity of a variable where the magnitude is given
;       by the first argument and the sign is given by the second argument.
;
; CALLING SEQUENCE:
;       AACGM_v2_Sgn, a, b
;     
;     Input Arguments:  
;       a             - magnitude
;       b             - sign
;
;     Return Value:
;       signed quantity
;
;+-----------------------------------------------------------------------------
;

function AACGM_v2_Sgn, a, b
  if (a ge 0) then x = a else x = -a
  if (b ge 0) then return, x
  return, -x
end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Rylm
;
; PURPOSE:
;       Computes an array of real spherical harmonic function values
;       Y_lm(phi,theta) for a given colatitiude (phi) and longitude (theta)
;       for all the values up to l = order, which is typically 10. The
;       values are stored in a 1D array of dimension (order+1)^2. The
;       indexing scheme used is:
;
;        l    0  1  1  1  2  2  2  2  2  3  3  3  3  3  3  3  4  4  4  4  4 ...
;        m    0 -1  0  1 -2 -1  0  1  2 -3 -2 -1  0  1  2  3 -4 -3 -2 -1  0 ...
;C & IDL j    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 ...
;FORTRAN j    1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 ...
; 
; CALLING SEQUENCE:
;       AACGM_v2_Rylm, colat,lon,order, ylmval
;
;     Input Arguments:  
;       colat         - The colatitude of the point for which the spherical
;                       harmonic Y_lm is to be calculated
;
;       lon           - The longitude of the point for which the spherical
;                       harmonic Y_lm is to be calculated
;
;       order         - The order of the spherical harmonic function expansion.
;                       The total number of terms computed will be (order+1)^2
;
;     Output Argument:
;       ylmval        - 1D array of spherical harmonic functions at the point
;                       (colat,lon)
;
; HISTORY:
;
; Revision 1.1  94/10/12  15:24:21  15:24:21  baker (Kile Baker S1G)
; Initial revision
;
; subsequent revisions, porting to C and IDL by baker, wing and barnes.
;
; NOTES by SGS:
;
; It is likely that the original version was taken from FORTRAN and used array
; indexing that begins with 1. Indexing is somewhat more natural using the
; zeros-based indexing of C/IDL. Indices have thus been changed from the
; original version.
;
; It appears that the original version used unnormalized spherical harmonic
; functions. I suspect this might be better, but realized it too late. The
; coefficients I derived are for orthonormal spherical harmonic functions
; which then require the same for evaluation. I believe that the original
; authors used orthogonal spherical harmonic functions which eliminate the
; need for computing the normalization factors. I suspect this is just fine,
; but have not tested it.
; 
;+-----------------------------------------------------------------------------
;

pro AACGM_v2_Rylm, colat,lon,order, ylmval

  ; Note: the recurrence relations are for the associated legendre polynomials.
  ;       SGS has added the normalization factor for the spherical harmonic
  ;       functions given by (6.8.2). See note about this above.

  cos_theta = cos(colat)
  sin_theta = sin(colat)

  cos_lon = cos(lon)
  sin_lon = sin(lon)

  d1    = -sin_theta
  z2    = dcomplex(cos_lon,sin_lon)
  z1    = d1*z2
  q_fac = z1

  ; Generate Zonal Harmonics (P_l^(m=0) for l = 1,order) using recursion
  ; relation (6.8.7), p. 252, Numerical Recipes in C, 2nd. ed., Press. W.
  ; et al. Cambridge University Press, 1992) for case where m = 0.
  ;
  ; l Pl = cos(theta) (2l-1) Pl-1 - (l-1) Pl-2          (6.8.7)
  ;
  ; where Pl = P_l^(m=0) are the associated Legendre polynomials

  ylmval[0] = 1.d         ; l = 0, m = 0
  ylmval[2] = cos_theta   ; l = 1, m = 0

  for l=2,order do begin
    ; indices for previous two values: k = l * (l+1) + m with m=0
    ia = (l-2)*(l-1)
    ib = (l-1)*l
    ic = l * (l+1)

    ylmval[ic] = (cos_theta * (2*l-1) * ylmval[ib] - (l-1)*ylmval[ia])/l
  endfor

  ; Generate P_l^l for l = 1 to (order+1)^2 using algorithm based upon (6.8.8)
  ; in Press et al., but incorporate longitude dependence, i.e., sin/cos (phi)
  ;
  ; Pll = (-1)^l (2l-1)!! (sin^2(theta))^(l/2)
  ;
  ; where Plm = P_l^m are the associated Legendre polynomials

  q_val = q_fac
  ylmval[3] = double(q_val)       ; l = 1, m = +1
  ylmval[1] = -imaginary(q_val)   ; l = 1, m = -1
  for l=2,order do begin
    d1    = l*2 - 1.
    z2    = d1*q_fac
    z1    = z2*q_val
    q_val = z1

    ; indices for previous two values: k = l * (l+1) + m
    ia = l*(l+2)    ; m = +l
    ib = l*l        ; m = -l

    ylmval[ia] = double(q_val)
    ylmval[ib] = -imaginary(q_val)
  endfor

  ; Generate P_l,l-1 to P_(order+1)^2,l-1 using algorithm based upon (6.8.9)
  ; in Press et al., but incorporate longitude dependence, i.e., sin/cos (phi)
  ;
  ; Pl,l-1 = cos(theta) (2l-1) Pl-1,l-1

  for l=2,order do begin
    l2 = l*l
    tl = 2*l
    ; indices for Pl,l-1; Pl-1,l-1; Pl,-(l-1); Pl-1,-(l-1)
    ia = l2 - 1
    ib = l2 - tl + 1
    ic = l2 + tl - 1
    id = l2 + 1

    fac = tl - 1
    ylmval[ic] = fac * cos_theta * ylmval[ia]     ; Pl,l-1
    ylmval[id] = fac * cos_theta * ylmval[ib]     ; Pl,-(l-1)
  endfor

  ; Generate remaining P_l+2,m to P_(order+1)^2,m for each m = 1 to order-2
  ; using algorithm based upon (6.8.7) in Press et al., but incorporate
  ; longitude dependence, i.e., sin/cos (phi).
  ;
  ; for each m value 1 to order-2 we have P_mm and P_m+1,m so we can compute
  ; P_m+2,m; P_m+3,m; etc.

  for m=1,order-2 do begin
    for l=m+2,order do begin
      ca = double(2.*l-1)/(l-m)
      cb = double(l+m-1.)/(l-m)

      l2 = l*l
      ic = l2 + l + m
      ib = l2 - l + m
      ia = l2 - l - l - l + 2 + m
      ; positive m
      ylmval[ic] = ca * cos_theta * ylmval[ib] - cb * ylmval[ia]

      ic -= (m+m)
      ib -= (m+m)
      ia -= (m+m)
      ; negative m
      ylmval[ic] = ca * cos_theta * ylmval[ib] - cb * ylmval[ia]
    endfor
  endfor

  ; Normalization added here (SGS)
  ;
  ; Note that this is NOT the standard spherical harmonic normalization factors
  ;
  ; The recursive algorithms above treat positive and negative values of m in
  ; the same manner. In order to use these algorithms the normalization must
  ; also be modified to reflect the symmetry.
  ;
  ; Output values have been checked against those obtained using the internal
  ; IDL legendre() function to obtain the various associated legendre
  ; polynomials.
  ;
  ; As stated above, I think that this normalization may be unnecessary. The
  ; important thing is that the various spherical harmonics are orthogonal,
  ; rather than orthonormal.

  fact = factorial(indgen(2*order+1))
  ffff = dblarr((order+1)*(order+1))
  for l=0,order do begin
    for m=0,l do begin
      k = l * (l+1) + m         ; 1D index for l,m
      ffff[k] = sqrt((2*l+1)/(4*!dpi) * fact[l-m]/fact[l+m])
    endfor
    for m=-l,-1 do begin
      k = l * (l+1) + m         ; 1D index for l,m
      kk = l * (l+1) - m
      ffff[k] = ffff[kk] * (-1)^(-m mod 2)
    endfor
  endfor

  ylmval *= ffff

  return
end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Alt2CGM
;
; PURPOSE:
;       Transformation from so-called 'at-altitude' coordinates to AACGM.
;       The purpose of this function is to scale the latitudes in such a
;       way so that there is no gap. The problem is that for non-zero
;       altitudes (h) are range of latitudes near the equator lie on dipole
;       field lines that near reach the altitude h, and are therefore not
;       accessible. This is the inverse transformation.
;
;       cos (lat_aacgm) = sqrt( Re/(Re + h) ) cos (lat_at-alt)
;       
;
; CALLING SEQUENCE:
;       AACGM_v2_Alt2CGM,r_height_in,r_lat_alt,r_lat_adj
;
;     Input Arguments:  
;       r_height_in   - The altitude (h)
;       r_lat_alt     - The 'at-altitude' latitude
;
;     Output Arguments:  
;       r_lat_adj     - The corrected latitude, i.e., AACGM latitude
;
; HISTORY:
;
; This function is unchanged from its original version (Baker ?)
;     
;+-----------------------------------------------------------------------------
;

pro AACGM_v2_Alt2CGM, r_height_in, r_lat_alt, r_lat_adj
  common IGRF_v2_Com

  ; convert from at-altitude to AACGM coordinates
; eradius = 6371.2
  eps     = 1e-9
  unim    = 0.9999999

  r1 = cos(!pi*r_lat_alt/180.)
  ra = r1*r1
  if (ra lt eps) then ra = eps

  r0 = (r_height_in/RE + 1.) / ra
  if (r0 lt unim) then r0 = unim
  
  r1 = acos(sqrt(1./r0))
  r_lat_adj= AACGM_v2_Sgn(r1, r_lat_alt)*180.0/!pi

  return
end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_CGM2Alt
;
; PURPOSE:
;       Transformation from AACGM to so-called 'at-altitude' coordinates.
;       The purpose of this function is to scale the latitudes in such a
;       way so that there is no gap. The problem is that for non-zero
;       altitudes (h) are range of latitudes near the equator lie on dipole
;       field lines that near reach the altitude h, and are therefore not
;       accessible. This mapping closes the gap.
;
;       cos (lat_at-alt) = sqrt( (Re + h)/Re ) cos (lat_aacgm)
;       
;
; CALLING SEQUENCE:
;       AACGM_v2_CGM2Alt,r_height_in,r_lat_in,r_lat_adj, error
;
;     Input Arguments:  
;       r_height_in   - The altitude (h)
;       r_lat_in      - The AACGM latitude
;
;     Output Arguments:  
;       r_lat_adj     - The 'at-altitude' latitude
;       error         - variable is set if latitude is below the value that
;                       is mapped to the origin
;
; HISTORY:
;
; This function is unchanged from its original version (Baker ?)
;     
;+-----------------------------------------------------------------------------
;

pro AACGM_v2_CGM2Alt, r_height_in,r_lat_in, r_lat_adj, error
  common IGRF_v2_Com

  ; convert from AACGM to at-altitude coordinates
  ;eradius = 6371.2
  unim    = 1
  error   = 0

  r1 = cos(!pi*r_lat_in/180.0)
  ra = (r_height_in/RE+ 1)*(r1*r1)
  if (ra gt unim) then begin
    ra = unim
    error = 1
  endif

  r1 = acos(sqrt(ra))
  r_lat_adj = AACGM_v2_Sgn(r1,r_lat_in)*180.0/!pi

  return
end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_ConvertGeoCoord
;
; PURPOSE:
;
;       Convert to and from AACGM and Geographic with numerous options, such as
;       whether to use the coefficients or do fieldline tracing. The altitude
;       dependence is determined here. The default is to perform the
;       transformation from Geographic to AACGM coordinates using the AACGM
;       coefficients. Several keywords can be used to modify the behavior.
;
; CALLING SEQUENCE:
;       AACGM_v2_ConvertGeoCoord, lat_in,lon_in,height_in, lat_out,lon_out, $
;                                 error, geo=geo, trace=trace, $
;                                 allow_trace=allow_trace, bad_idea=bad_idea, $
;                                 at_alt=at_alt, verbose=verbose, debug=debug
;
;     Input Arguments:  
;       lat_in        - input latitude (degrees)
;       lon_in        - input longitude (degrees)
;       height_in     - input altitude (km)
;
;     Output Arguments:  
;       lat_out       - transformed latitude (degrees)
;       lon_out       - transformed longitude (degrees)
;       error         - non-zero integer code used to indicate failure.
;
;     Keywords:
;       geo           - perform inverse transformation: AACGM -> GEO
;       trace         - perform fieldline tracing instead of using coefficients
;       allow_trace   - allow tracing above 2000 km altitude. The default is
;                       to not allow altitudes above 2000 km.
;       bad_idea      - use coefficients to extrapolate for altitudes above
;                       2000 km. This really is a bad idea...
;
;       at_alt        - use the intermediate transformation to at-altitude
;                       dipole coordinates. Note: only used for testing in
;                       the forward direction. Should disable this feature
;                       for release. Use requires that coefficients be derived
;                       from intermediate coordinates, which is not the case.
;       verbose       - debugging for fieldline tracing of inverse
;                       transformation. Should disable this feature for release
;       debug         - set keyword to use built-in IDL Legendre functions
;                       instead of Rylm procedure. Included for debugging of
;                       Rylm procedure. Remove for release.
;
; HISTORY:
;
; Fieldline tracing and modified altitude dependence added to original version.
; 20140827 SGS removed myear and fyear as arguments
;     
;+-----------------------------------------------------------------------------
;

pro AACGM_v2_ConvertGeoCoord, lat_in,lon_in,height_in, lat_out,lon_out, $
                                error, geo=geo, trace=trace, $
                                bad_idea=bad_idea, allow_trace=allow_trace, $
                                eps=eps, at_alt=at_alt, verbose=verbose, $
                                debug=debug
  common AACGM_v2_Com
  common IGRF_v2_Com

  ; need RE later on
  if (n_elements(RE) eq 0) then init_common

  if (height_in lt 0) then begin
    if ~keyword_set(verbose) || verbose ge 0 then $
      print, $
      'WARNING: coordinate transformations are not intended for altitudes < 0 km : ', $
      height_in
    error = -2
  endif

  if height_in gt 2000 and $
      not keyword_set(trace) and not keyword_set(bad_idea) and $
      not keyword_set(allow_trace) then begin
    ; the user is not using fieldline tracing or indicated that they know
    ; what they are doing by setting the 'bad_idea' keyword.
    print, 'ERROR: coefficients are not valid for altitudes above 2000 km.'
    print, '       Use fieldline tracing option [/trace] to perform fieldline'
    print, '       tracing at all altitudes, [/allow_trace] to perform'
    print, '       fieldline tracing only above 2000 km or indicate that you'
    print, '       want to use the coefficients for extrapolation and are aware'
    print, '       that the results can be nonsensical by setting the bad_idea'
    print, '       keyword.'
    error = -4
    return
  endif

  if (abs(lat_in) gt 90.) then begin
    print, 'ERROR: latitudes must be in the range -90 to +90 degrees'
    error = -8
    return
  endif

; SGS v2.3 removing requirement that longitude be 0 to 360. Does not seems to
;          matter and is inconsistent with C version: -180 to 180.
;
; ; SGS - better checking of inputs needed here
; if lon_in lt 0 then lon_in += 360
; if ((lon_in lt 0) or (lon_in gt 360)) then begin
;   print, 'ERROR: longitudes must be in the range 0 to 360 degrees'
;   print, lon_in
;   error = -16
;   return
; endif

; SGS v2.3 geodetic to geocentric (and back) are done in AACGM_v2_Convert()

  ; field line tracing
  if keyword_set(trace) or (height_in gt 2000 and keyword_set(allow_trace)) $
  then begin
    if keyword_set(geo) then begin
      AACGM_v2_Trace_inv, lat_in,lon_in,height_in, tmp_lat,tmp_lon, error, $
                    fixed=fixed, ds=ds, eps=eps, max_ds=max_ds, $
                    verbose=verbose
        lat_out = tmp_lat
        lon_out = tmp_lon

    endif else begin
      AACGM_v2_Trace, lat_in,lon_in,height_in, tmp_lat,tmp_lon, error, $
                    fixed=fixed, ds=ds, eps=eps, max_ds=max_ds
      lat_out = tmp_lat
      lon_out = tmp_lon
    endelse
    return
  endif

  ; 20140827 SGS moved coefficient loading to Date/Time setting functions
  ; 20140827 SGS moved time interpolation to Date/Time setting functions

  flag = keyword_set(geo)

  ; determine the altitude dependence of the coefficients
  if (height_in ne height_old_v2[flag]) then begin
;   print, '*** HEIGHT INTERPOLATION ***'
    alt_var    = height_in/2000.0   ; make this scaling height a variable?
    alt_var_sq = alt_var*alt_var
    alt_var_cu = alt_var*alt_var_sq
    alt_var_qu = alt_var*alt_var_cu

    for i=0,2 do begin    ; should this be variable, i.e. we only use x and y
      for j=0,kmax_v2-1 do begin 
        cint_v2[j,i,flag] = coef_v2[j,i,0,flag] + $
                          coef_v2[j,i,1,flag] * alt_var + $
                          coef_v2[j,i,2,flag] * alt_var_sq + $
                          coef_v2[j,i,3,flag] * alt_var_cu + $
                          coef_v2[j,i,4,flag] * alt_var_qu 
      endfor
    endfor
    height_old_v2[flag] = height_in
  endif

  x = double(0)
  y = double(0)
  z = double(0)

; lon_input = lon_in*!pi/180.0 
  lon_input = lon_in*DTOR

  ; Intermediate coordinate. Only used for inverse transmformation
; if not keyword_set(at_alt) or (flag eq 0) then begin
  if not keyword_set(at_alt) and (flag eq 0) then begin
    colat_input = (90.-lat_in)*DTOR
  endif else begin
    ; convert from AACGM to at-altitude coordinates
    error = -64
    AACGM_v2_CGM2Alt, height_in,lat_in, lat_adj, errflg
    if (errflg ne 0) then return
    colat_input = (90. - lat_adj)*DTOR
  endelse

  ; SGS - remove this feature after timing tests
  if keyword_set(debug) then begin
    ; use the built-in legendre function to compute values of the spherical
    ; harmonic functions
    fact = factorial(indgen(2*order_v2+1))

    ylmval = dblarr(kmax_v2)
    for l=0,order_v2 do begin

      ; m < 0 terms
      for m=-l,-1 do begin
        plm = legendre(cos(colat_input), l, m, /double)
        ylm = sqrt((2*l+1)/(4*!dpi) * fact[l-m]/fact[l+m])*plm*sin(m*lon_input)
        k = l * (l+1) + m         ; 1D index for l,m
        ylmval[k] = ylm
        x += cint_v2[k,0,flag] * ylm
        y += cint_v2[k,1,flag] * ylm
        z += cint_v2[k,2,flag] * ylm
      endfor

      ; m = 0 term
      plm = legendre(cos(colat_input), l, 0, /double)
      ylm = sqrt((2*l+1)/(4*!dpi)) * plm
      k = l * (l+1)               ; 1D index for l,m
      ylmval[k] = ylm
      x += cint_v2[k,0,flag] * ylm
      y += cint_v2[k,1,flag] * ylm
      z += cint_v2[k,2,flag] * ylm

      ; m > 0 terms
      for m=1,l do begin
        plm = legendre(cos(colat_input), l, m, /double)
        ylm = sqrt((2*l+1)/(4*!dpi) * fact[l-m]/fact[l+m])*plm*cos(m*lon_input)
        k = l * (l+1) + m         ; 1D index for l,m
        ylmval[k] = ylm
        x += cint_v2[k,0,flag] * ylm
        y += cint_v2[k,1,flag] * ylm
        z += cint_v2[k,2,flag] * ylm
      endfor

    endfor
    ;print, 'legendre()'
    ;print, ylmval
  endif else begin
    ; use the Rylm function (adapted to orthonormal functions; SGS) to compute
    ; values of the spherical harmonic functions

    ylmval = dblarr(kmax_v2)
    AACGM_v2_Rylm, colat_input,lon_input, order_v2,ylmval

    for l = 0,order_v2 do begin
      for m = -l,l do begin
        k = l * (l+1) + m

        x += cint_v2[k,0,flag]*ylmval[k]
        y += cint_v2[k,1,flag]*ylmval[k]
        z += cint_v2[k,2,flag]*ylmval[k]    ; SGS - need this for sign...
      endfor
    endfor
  ;print, 'Rylm'
  ;print, ylmval
  endelse

  ; COMMENT: SGS
  ; 
  ; This answers one of my questions about how the coordinates for AACGM are
  ; guaranteed to be on the unit sphere. Here they compute xyz indpendently
  ; using the SH coefficients for each coordinate. They reject anything that
  ; is +/- .1 Re from the surface of the Earth. They then scale each xyz
  ; coordinate by the computed radial distance. This is a TERRIBLE way to do
  ; things... but necessary for the inverse transformation.

  ; SGS - new method that ensures position is on unit sphere and results in a
  ;       much better fit. Uses z coordinate only for sign, i.e., hemisphere.
  if (flag eq 0) then begin
    fac = x*x + y*y
    if fac gt 1. then begin
      ; we are in the forbidden region and the solution is undefined
      lat_out = !values.f_nan
      lon_out = !values.f_nan
      error = -64
      return
    endif

    ztmp = sqrt(1. - fac)
    if z lt 0 then z = -ztmp else z = ztmp

    colat_temp = acos(z)

  endif else begin
  ; SGS - for inverse the old normalization produces lower overall errors...
    r = sqrt(x*x + y*y + z*z)
    if ((r lt 0.9) or (r gt 1.1)) then begin
      ; too much variation in the radial component
      lat_out = !values.f_nan
      lon_out = !values.f_nan
      error = -32
      return
    endif
 
    z /= r
    x /= r
    y /= r

    ; SGS - this is for cases where abs(z) > 1.
    if (z ge 1.) then colat_temp = 0 $ 
    else if (z lt -1.) then colat_temp = !dpi $
    else colat_temp = acos(z)
  endelse 

  ; SGS - check these values
  if ((abs(x) lt 1e-8) and (abs(y) lt 1e-8)) then $
    lon_temp = 0 $
  else $
    lon_temp = atan(y,x)

  lon_output = lon_temp

  if keyword_set(at_alt) and (flag eq 0) then begin
    lat_alt = 90 - colat_temp/DTOR
    AACGM_v2_Alt2CGM, height_in,lat_alt, lat_adj
    colat_output = (90. - lat_adj) * DTOR; * !pi/180.0
  endif else $
    colat_output = colat_temp

  lat_out = 90. - colat_output/DTOR ;*180.0/!pi
  lon_out = lon_output/DTOR ;*180.0/!pi

  error = 0  
  return
end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Convert
;
; PURPOSE:
;
;       High-level routine that loops over each input array element or just
;       calls the transformation function for the input scalars. Several
;       keywords can be used to modify the behavior and are passed through to
;       subsequent functions.
;
; CALLING SEQUENCE:
;       s = AACGM_v2_Convert(in_lat, in_lon, height, out_lat, out_lon, r, $
;                             geo=geo, trace=trace)
;
;     Input Arguments:  
;       in_lat        - input latitude (degrees)
;       in_lon        - input longitude (degrees)
;       height        - input altitude (km)
;
;     Output Arguments:  
;       out_lat       - transformed latitude (degrees)
;       out_lon       - transformed longitude (degrees)
;       r             - distance from origin (Re)
;       s             - non-zero integer code used to indicate failure.
;
;     Keywords:
;       geo           - perform inverse transformation: AACGM -> GEO
;       trace         - perform fieldline tracing instead of using coefficients
;       verbose       - debugging for fieldline tracing of inverse
;                       transformation. Should disable this feature for release
;       debug         - set keyword to use built-in IDL Legendre functions
;                       instead of Rylm procedure. Included for debugging of
;                       Rylm procedure. Remove for release.
;
; HISTORY:
;
; added keywords to pass through, otherwise unmodified from original.
; 20140827 SGS removed myear and fyear as arguments
;     
;
; NOTES:
;
;       All AACGM-v2 conversions are done in geocentric coordinates using a
;           value of 6371.2 km for the Earth radius.
;
;       For G2A conversion inputs are geographic latitude, longitude and
;           height (glat,glon,height), specified as either geocentric or
;           geodetic (default). For geodetic inputs a conversion to geocentric
;           coordinates is performed, which changes the values of
;           glat,glon,height. The output is AACGM-v2 latitude, longitude and
;           the geocentric radius (mlat,mlon,r) using the geocentric height
;           in units of RE.
;
;        For A2G conversion inputs are AACGM-v2 latitude, longitude and the
;            geocentric height (mlat,mlon,height). The latter can be obtained
;            from the r output of the G2A conversion. The output is geographic
;            latitude, longitude and height (glat,glon,height). If the
;            gedodetic option is desired (default) a conversion of the outputs
;            is performed, which changes the values of glat,glon,height.
;
;+-----------------------------------------------------------------------------
;

function AACGM_v2_Convert, in_lat,in_lon,height, out_lat,out_lon,r, geo=geo, $
                            trace=trace, bad_idea=bad_idea, $
                            eps=eps, allow_trace=allow_trace, $
                            gcentric=gcentric, $
                            verbose=verbose, debug=debug

  common IGRF_v2_Com

  geo = keyword_set(geo)
  if n_elements(in_lat) ne 1 then begin
    n       = n_elements(in_lat)
    sze     = size(in_lat)
    out_lat = dblarr(sze[1:sze[0]])
    out_lon = dblarr(sze[1:sze[0]])
    r       = dblarr(sze[1:sze[0]])
    tmp_lat = 0.d
    tmp_lon = 0.d
    for i=0, n-1 do begin 

      ; convert geodetic to geocentric
      if not keyword_set(gcentric) and not keyword_set(geo) then begin
        rtp = geod2geoc(in_lat[i], in_lon[i], height[i])

        ;* modify lat/lon/alt to geocentric values */
        in_lat[i] = 90.d - rtp[1]/DTOR
        in_lon[i] = rtp[2]/DTOR
        height[i] = (rtp[0]-1.d)*RE
      endif

      AACGM_v2_ConvertGeoCoord, in_lat[i],in_lon[i],height[i], $
                                tmp_lat,tmp_lon, error, geo=geo, $
                                trace=trace, bad_idea=bad_idea, $
                                allow_trace=allow_trace, eps=eps, $
                                verbose=verbose, debug=debug
      out_lat[i] = tmp_lat
      out_lon[i] = tmp_lon

      if not keyword_set(geo) then begin
        r[i] = (height[i] + RE)/RE    ; geocentric radial distance in RE
      endif else begin
        if not keyword_set(gcentric) then begin
          p = geoc2geod(out_lat[i],out_lon[i],(RE+height[i])/RE)
          out_lat[i] = p[0]
          height[i] = p[2]
        endif
        r[i] = height[i]
      endelse
    endfor
  endif else begin
    out_lat = 0.d
    out_lon = 0.d

    ; convert geodetic to geocentric
    if not keyword_set(gcentric) and not keyword_set(geo) then begin
      rtp = geod2geoc(in_lat, in_lon, height)

      ;* modify lat/lon/alt to geocentric values */
      in_lat = 90.d - rtp[1]/DTOR
      in_lon = rtp[2]/DTOR
      height = (rtp[0]-1.d)*RE
    endif

    AACGM_v2_ConvertGeoCoord, in_lat,in_lon,height, $
                              out_lat,out_lon, error, geo=geo, $
                              trace=trace, bad_idea=bad_idea, $
                              allow_trace=allow_trace, eps=eps, $
                              verbose=verbose, debug=debug

    if not keyword_set(geo) then begin
      r = (height + RE)/RE    ; geocentric radial distance in RE
    endif else begin
      if not keyword_set(gcentric) then begin
        p = geoc2geod(out_lat,out_lon,(RE+height)/RE)

        out_lat = p[0]
        height = p[2]
      endif
      r = height
    endelse
  endelse

  return, error
end

;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
; fieldline tracing routines are now located in igrflib.pro
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Trace
;
; PURPOSE:
;
;       Perform field line tracing to determine AACGM coordinates
;
; CALLING SEQUENCE:
;       AACGM_v2_Trace, lat_in,lon_in,height_in, lat_out,lon_out, error
;
;     Input Arguments:  
;       lat_in        - latitude specified in geographic degrees
;       lon_in        - longitude specified in geographic degrees
;       height_in     - altitude above surface of Earth in kilometers
;
;     Output Arguments:  
;       lat_out       - latitude specified in AACGM degrees
;       lon_out       - longitude specified in AACGM degrees
;       error         - non-zero error code
;
;     Keywords:
;       ds            - starting stepsize in km
;       fixed         - set this keyword to do RK4 method with stepsize ds
;       eps           - global error in units of RE; CAREFUL using too small
;       max_ds        - maximum stepsize that is allowed, in units of Re
;
; HISTORY:
;
; new function
;
; 20180511 Added a check for when tracing goes below altitude so as not to
;          contiue tracing beyond what is necessary.
;
;          Also making sure that stepsize does not go to zero
;     
;+-----------------------------------------------------------------------------
;

pro AACGM_v2_Trace, lat_in,lon_in,height_in, lat_out,lon_out, error, $
                    fixed=fixed, ds=ds, eps=eps, max_ds=max_ds
  common AACGM_v2_Com
  common IGRF_v2_Com

  IGRF_SetDateTime, aacgm_v2_datetime.year, aacgm_v2_datetime.month, $
                    aacgm_v2_datetime.day,  aacgm_v2_datetime.hour,  $
                    aacgm_v2_datetime.minute, aacgm_v2_datetime.second, $
                    err=error
  if (error ne 0) then return

  if not keyword_set(ds) then $
    ds  = 1.                  ; 1 km default starting stepsize
  dsRE  = ds/RE
  dsRE0 = dsRE
  if not keyword_set(eps) then $
    eps = 1e-4/RE             ; global error (RE)

  ; if user wants to fix maximum step size then let them by turning off
  ; radial step size dependence that is default
  if keyword_set(max_ds) then RRds = 0 else RRds = 1

  rtp = dblarr(3)
  rtp[0] = (RE + height_in)/RE  ; 1.0 is surface of Earth
  rtp[1] = (90.-lat_in)*DTOR    ; colatitude in radians
  rtp[2] = lon_in*DTOR          ; longitude  in radians

  ; convert position to Cartesian coords
  xyzg = sph2car(rtp)

  ; convert to magnetic Dipole coordinates
  xyzm = geo2mag(xyzg)

  if xyzm[2] gt 0 then idir = -1 else idir = 1    ; N or S hemisphere

  dsRE = dsRE0

  ; trace to magnetic equator
  ;
  ; Note that there is the possibility that the magnetic equator lies
  ; at an altitude above the surface of the Earth but below the starting
  ; altitude. I am not certain of the definition of CGM, but these
  ; fieldlines map to very different locations than the solutions that
  ; lie above the starting altitude. I am considering the solution for
  ; this set of fieldlines as undefined; just like those that lie below
  ; the surface of the Earth.

  ; Added a check for when tracing goes below altitude so as not to contiue
  ; tracing beyond what is necessary.
  ;
  ; Also making sure that stepsize does not go to zero

  below = 0
  niter = 0
  while (~below && (idir*xyzm[2] lt 0)) do begin

    xyzp = xyzg

    ; x,y,z are passed by reference and modified here...
    AACGM_v2_RK45, xyzg, idir, dsRE, eps

    ; make sure that stepsize does not go to zero
    if (dsRE*RE lt 1e-2) then dsRE = 1e-2/RE

    ; convert to magnetic Dipole coordinates
    xyzm = geo2mag(xyzg)

    below = (total(xyzg*xyzg) lt (RE+height_in)*(RE+height_in)/(RE*RE))

    niter++
  endwhile

  xyzc = xyzp
  if (~below && niter gt 1) then begin
    ; now bisect stepsize (fixed) to land on magnetic equator w/in 1 meter

    while dsRE gt 1e-3/RE do begin
      dsRE *= .5
      xyzp = xyzc
      AACGM_v2_RK45, xyzc, idir, dsRE, eps, /fixed
      xyzm = geo2mag(xyzc)

      ; Is it possible that resetting here causes a doubling of the tol?
      if idir * xyzm[2] gt 0 then xyzc = xyzp
    endwhile
  endif

  ; 'trace' back to surface along Dipole field lines
  Lshell = sqrt(total(xyzc*xyzc))
  if Lshell lt (RE+height_in)/RE then begin
    ; Magnetic equator is below your...
    lat_out = !values.f_nan
    lon_out = !values.f_nan
    error   = 1
  endif else begin
    xyzm = geo2mag(xyzc)
    rtp  = car2sph(xyzm)

    lat_out = -idir*acos(sqrt(1./Lshell))/DTOR
    lon_out = rtp[2]/DTOR
    if lon_out gt 180 then lon_out -= 360   ; SGS - make consistent with output
                                            ; from coefficient functions
    error   = 0
  endelse

end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Trace_inv
;
; PURPOSE:
;
;       Perform field line tracing to determine geographic coordinates from
;       AACGM coordinates
;
; CALLING SEQUENCE:
;       AACGM_v2_Trace_inv, lat_in,lon_in,height_in, lat_out,lon_out, error
;
;     Input Arguments:  
;       lat_in        - latitude specified in AACGM degrees
;       lon_in        - longitude specified in AACGM degrees
;       height_in     - altitude above surface of Earth in kilometers
;
;     Output Arguments:  
;       lat_out       - latitude specified in geographic degrees
;       lon_out       - longitude specified in geographic degrees
;       error         - non-zero error code
;
;     Keywords:
;       ds            - starting stepsize in km
;       fixed         - set this keyword to do RK4 method with stepsize ds
;       eps           - global error in units of RE; CAREFUL using too small
;       max_ds        - maximum stepsize that is allowed, in units of Re
;
; HISTORY:
;
; new function
;
; 20180511 Added a check for when tracing goes below altitude so as not to
;          contiue tracing beyond what is necessary.
;
;          Also making sure that stepsize does not go to zero
;     
;+-----------------------------------------------------------------------------
;

pro AACGM_v2_Trace_inv, lat_in,lon_in,height_in, lat_out,lon_out, error, $
                    fixed=fixed, ds=ds, eps=eps, max_ds=max_ds, verbose=verbose
  common AACGM_v2_Com
  common IGRF_v2_Com

  IGRF_SetDateTime, aacgm_v2_datetime.year, aacgm_v2_datetime.month, $
                    aacgm_v2_datetime.day,  aacgm_v2_datetime.hour,  $
                    aacgm_v2_datetime.minute, aacgm_v2_datetime.second, $
                    err=error
  if (error ne 0) then return

  if not keyword_set(ds) then $
    ds  = 1.                  ; 1 km default starting stepsize
  dsRE  = ds/RE
  dsRE0 = dsRE
  if not keyword_set(eps) then $
    eps   = 1e-4/RE           ; global error (RE)

  ; if user wants to fix maximum step size then let them by turning off
  ; radial step size dependence that is default
  if keyword_set(max_ds) then RRds = 0 else RRds = 1

  ; INV: for inverse we must first map AACGM to magnetic equator along
  ;      the dipole field line that passes through the Earth at lat/lon
  if (abs(lat_in - 90.d) lt 1e-6) then lat_in = 90-1e-6
  Lshell = 1.d/(cos(lat_in*DTOR)*cos(lat_in*DTOR))

  if keyword_set(verbose) then print, Lshell

  if Lshell lt (RE + height_in)/RE then begin
    ; solution does not exist, the starting position at the magnetic
    ; equator is below the altitude of interest
    lat_out = !values.f_nan
    lon_out = !values.f_nan
    error   = 1
  endif else begin
    phim = lon_in

    ; magnetic Cartesian coordinates of fieldline trace starting point
    xyzm = dblarr(3)
    xyzm[0] = Lshell*cos(phim*DTOR)
    xyzm[1] = Lshell*sin(phim*DTOR)
    xyzm[2] = 0.d

    ; geographic Cartesian coordinates of starting point
    xyzg = mag2geo(xyzm)

    ; geographic spherical coordinates of starting point
    rtp = car2sph(xyzg)

    ; direction of trace is determined by the starting hemisphere?
    if lat_in gt 0 then idir = 1 else idir = -1   ; N or S hemisphere

    dsRE = dsRE0

    ; trace back to altitude above Earth
    niter = 0
    while rtp[0] gt (RE + height_in)/RE do begin

      xyzp = xyzg
      if keyword_set(verbose) then print, 'xyz: ', xyzg, dsRE
      AACGM_v2_RK45, xyzg, idir, dsRE, eps, $
                              fixed=fixed, max_ds=max_ds, RRds=RRds, $
                              verbose=verbose
      if keyword_set(verbose) then print, 'xyz: ', xyzg, dsRE

      ; make sure that stepsize does not go to zero
      if (dsRE*RE lt 5e-1) then dsRE = 5e-1/RE

      rtp = car2sph(xyzg)

      niter++
;     if keyword_set(verbose) then stop
    endwhile

    ; now bisect stepsize (fixed) to land on magnetic equator w/in 1 meter
    xyzc = xyzp

    if niter gt 1 then begin
      while dsRE gt 1e-3/RE do begin
        dsRE *= .5
        xyzp = xyzc
        AACGM_v2_RK45, xyzc, idir, dsRE, eps, /fixed

        rtp = car2sph(xyzc)

        if rtp[0] lt (RE + height_in)/RE then xyzc = xyzp
      endwhile
    endif

    ; record lat/lon and xyz
    lat_out = 90. - rtp[1]/DTOR
    lon_out = rtp[2]/DTOR
    if lon_out gt 180 then lon_out -= 360   ; SGS - make consistent with output
                                            ; from coefficient functions
    error   = 0
  endelse

end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_TimeInterp
;
; PURPOSE:
;       Determine whether coefficients need to be loaded and whether
;       interpolation of the newly loaded or existing coefficients need to
;       occur.
;   
; CALLING SEQUENCE:
;       AACGM_v2_TimeInterp
;
; HISTORY:
;
; Revision 1.0  14/08/27 SGS initial version
; 
;+-----------------------------------------------------------------------------
;
function AACGM_v2_TimeInterp

  common AACGM_v2_Com

  ; set 5-year epoch based on the current year
  myear_v2 = fix(aacgm_v2_datetime.year/5)*5
  ; set the floating-point year based on the current date and time
  fyear_v2 = aacgm_v2_datetime.year + ((aacgm_v2_datetime.dayno-1) + $ 
          (aacgm_v2_datetime.hour + (aacgm_v2_datetime.minute + $
          aacgm_v2_datetime.second/60.d)/60.d)/24.d) / $
          aacgm_v2_datetime.daysinyear

  must_load = 0
  if (n_elements(myear_v2_old) eq 0) then begin
    ; first time, so need to load new coefficients
    must_load = 1
  endif else if (myear_v2_old ne myear_v2) then begin
    ; 5-year epoch year has changed, so need to load new coefficients
    must_load = 1
  endif

  ; load the sets of coefficients
  if (must_load ne 0) then begin
    modyr  = myear_v2
    prefix = getenv('AACGM_v2_DAT_PREFIX')
    if (strlen(prefix) eq 0) then begin
      ; prefix does not exist...
      print, 'Environment variable AACGM_v2_DAT_PREFIX is not set.'
      print, 'You must set this variable to the location and prefix, e.g., '
      print, "'aacgm_coeffs-11-', of the coefficient files on your system."
      return, -1
    endif

    ;   prefix = 'aacgm_coeffs-leo-11-'
    ;print, 'Loading new coefficients: ', modyr, modyr+5
    fnamea = prefix+string(format='(i4.4)',modyr)+'.asc'
    fnameb = prefix+string(format='(i4.4)',modyr+5)+'.asc'
    openr, ua, fnamea, /get_lun,/stdio
    openr, ub, fnameb, /get_lun,/stdio
    s      = AACGM_v2_LoadCoef([ua,ub])
    free_lun, ua,ub

    ; new coefficients so force both interpolations
    myear_v2_old = myear_v2
    fyear_v2_old = -1         ; force time interpolation
    height_old_v2 = [-1.,-1.] ; force height interpolation
  endif

  if (fyear_v2 ne fyear_v2_old) then begin
    ; here is the linear interpolation of the two bounding sets of coefficients
    coef_v2 = reform(coefs_v2[0,*,*,*,*]) + (fyear_v2 - myear_v2) * $
                reform(coefs_v2[1,*,*,*,*]-coefs_v2[0,*,*,*,*])/5

    height_old_v2[0] = -1.        ; force height interpolation because coeffs
    height_old_v2[1] = -1.        ; have changed

    fyear_v2_old = fyear_v2
  endif

  return, 0
end

