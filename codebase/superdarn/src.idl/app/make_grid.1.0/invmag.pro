; invmag.pro
; ===========
; Author: E.G.Thomas (2016)
:
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
; IGRFMagCmp
; sphtocar
; norm_vec
; glbthor
; fldpnt_sph
; fldpnt_azm
; RPosRngBmAzmElv
; RPosInvMag
;
;
; Routines:
; ---------
;



;------------------------------------------------------------------------------
;+
; NAME:
;   IGRFMagCmp
;
; PURPOSE:
;
;   *Note: This function is based on the C RST function IGRFMagCmp in magcmp.c*
;   *Note: This function is based on the IDL RST function igrfcall in igrflib.pro*
;
; INPUTS:
;   date        -
;   frho        - radius to field point in geocentric spherical coordinates [km]
;   flat        - latitude in geocentric spherical coordinates [deg]
;   flon        - longitude in geocentric spherical coordinates [deg]
;
; OUTPUT:
;   bx          - south component of the magnetic field vector
;   by          - east component of the magnetic field vector
;   bz          - vertical component of the magnetic field vector
;   b           - magnitude of the magnetic field vector
;
; CALLING SEQUENCE:
;   ret = IGRFMagCmp(date,frho,flat,flon,bx,by,bz)
;
;------------------------------------------------------------------------------
;
function IGRFMagCmp, date, frho, flat, flon, bx, by, bz, b

    if n_params() ne 8 then begin
        print, 'Invalid number of parameters in call to IGRFMagCmp'
        print, 'USE: ret=IGRFMagCmp(date,frho,flat,flon,bx,by,bz,b)'
        return, -1
    endif

    ; *Note: height in IGRFModelCall is treated as radial distance from center
    ; of Earth and not altitude*
    height = frho

    ; Make call to IGRF DLM to calculate magnetic field components at field point
    ; in local north/east/down coordinates [nT]
    ret = IGRFModelCall(date, flat, flon, height, bx, by, bz)

    ; Calculate magnitude of magnetic field vector [nT]
    b = sqrt( bx*bx + by*by + bz*bz )

    ; Convert to local south/vertical coordinates (rather than north/down)
    bx = -1.*bx
    bz = -1.*bz

    return, ret

end



;------------------------------------------------------------------------------
;+
; NAME:
;       sphtocar
;
; PURPOSE:
;   Converts from global geocentric spherical coordinates (r,theta,phi) to 
;   global Cartesian coordinates (x,y,z). The input is assumed to be in degrees
;   but can be specified as in radians by setting the rad keyword.
;   *Note: This function is based on the C RST function sphtocar in invmag.c*
;
; INPUTS:
;   r       - radius in global geocentric spherical coordinates [km]
;   theta   - latitude in global local geocentric spherical coordinates [deg]
;   phi     - longitude in global geocentric spherical coordinates [deg]
;
; OUTPUT:
;   x       - x position in global Cartesian coordinates
;   y       - y position in global Cartesian coordinates
;   z       - z position in global Cartesian coordinates
;
; KEYWORDS:
;   rad     - set this keyword to specify input in radians instead of degrees
;
; CALLING SEQUENCE:
;   ret = sphtocar(r,theta,phi,x,y,z,rad=rad)
;
;------------------------------------------------------------------------------
;
function sphtocar, r, theta, phi, x, y, z, rad=rad

    if n_params() ne 6 then begin
        print, 'Invalid number of parameters in call to sphtocar'
        print, 'USE: ret = sphtocar(r,theta,phi,x,y,z,rad=rad)'
        return, -1
    endif

    ; If rad keyword is set then input is in radians, otherwise assume is in degrees
    if keyword_set(rad) then begin
        x = r*sin(!pi/2.-theta)*cos(phi)
        y = r*sin(!pi/2.-theta)*sin(phi)
        z = r*cos(!pi/2.-theat)
    endif else begin
        x = r*sin(!pi/2.-theta*!pi/180.)*cos(phi*!pi/180.)
        y = r*sin(!pi/2.-theta*!pi/180.)*sin(phi*!pi/180.)
        z = r*cos(!pi/2.-theta*!pi/180.)
    endelse

    ; Return zero if successful
    return, 0

end



;------------------------------------------------------------------------------
;+
; NAME:
;   norm_vec
;
; PURPOSE:
;   This function normalizes the components of an input vector (x,y,z) as a
;   new output vector (nx,ny,nz).
;   *Note: This function is based on the C RST function norm_vec in invmag.c*
;
; INPUTS:
;   x       - x component of input vector
;   y       - y component of input vector
;   z       - z component of input vector
;
; OUTPUT:
;   nx      - x component of normalized vector
;   ny      - y component of normalized vector
;   nz      - z component of normalized vector
;
; CALLING SEQUENCE:
;   ret = norm_vec(x,y,z,nx,ny,nz)
;
;------------------------------------------------------------------------------
;
function norm_vec, x, y, z, nx, ny, nz

    if n_params() ne 6 then begin
        print, 'Invalid number of parameters in call to norm_vec'
        print, 'USE: ret=norm_vec(x,y,z,nx,ny,nz)'
        return, -1
    endif

    ; Calculate the vector magnitude
    r = sqrt(x*x + y*y + z*z)

    ; Calculate the normalized x, y, and z components
    nx = x/r
    ny = y/r
    nz = z/r

    ; Return zero if successful
    return, 0

end



;------------------------------------------------------------------------------
;+
; NAME:
;   glbthor
;
; PURPOSE:
;   Converts a vector from global Cartesian coordinates (rx,ry,rz) to
;   local south/east/vertical (horizontal) coordinates (tx,ty,tz) at a 
;   given geocentric latitude and longitude (lat,lon).
;   *Note: This function is based on the C RST function glbthor in invmag.c*
;
; INPUTS:
;   lat     - geocentric latitude [deg]
;   lon     - geocentric longitude [deg]
;   rx      - x component of input vector (Cartesian)
;   ry      - y component of input vector (Cartesian)
;   rz      - z component of input vector (Cartesian)
;
; OUTPUT:
;   tx      - local south component of rotated vector
;   ty      - local east component of rotated vector
;   tz      - local vertical component of rotated vector
;
; KEYWORDS:
;   inverse - perform the inverse transformation (south/east/vertical to XYZ)
;
; CALLING SEQUENCE:
;   ret = glbthor(lat,lon,rx,ry,rz,tx,ty,tz,inverse=inverse)
;
;------------------------------------------------------------------------------
;
function glbthor, lat, lon, rx, ry, rz, tx, ty, tz, inverse=inverse

    if n_params() ne 8 then begin
        print, 'Invalid number of parameters in call to glbthor'
        print, 'USE: ret=glbthor(lat,lon,rx,ry,rz,tx,ty,tz,inverse=inverse)'
        return, -1
    endif

    if keyword_set(inverse) then begin
        ; Convert the input vector from local south/east/vertical to Cartesian XYZ
        ; Calculate the co-latitude
        lax = 90. - lat

        ; Rotate the input vector about the east-axis by the latitude
        sx = tx*cos(lax*!pi/180.) + tz*sin(lax*!pi/180.)
        sy = ty
        sz = -1.*tx*sin(lax*!pi/180.) + tz*cos(lax*!pi/180.)

        ; Rotate the vector about the z-axis by the longitude
        rx = sx*cos(lon*!pi/180.) - sy*sin(lon*!pi/180.)
        ry = sx*sin(lon*!pi/180.) + sy*cos(lon*!pi/180.)
        rz = sz
    endif else begin
        ; Convert the input vector from Cartesian XYZ to local south/east/vertical
        ; Rotate the input vector about the z-axis by the longitude
        sx = rx*cos(lon*!pi/180.) + ry*sin(lon*!pi/180.)
        sy = -1.*rx*sin(lon*!pi/180.) + ry*cos(lon*!pi/180.)
        sz = rz

        ; Calculate the co-latitude in degrees
        lax = 90. - lat

        ; Rotate the vector about the east-axis by the co-latitude
        tx = sx*cos(lax*!pi/180.) - sz*sin(lax*!pi/180.)
        ty = sy
        tz = sx*sin(lax*!pi/180.) + sz*cos(lax*!pi/180.)
    endelse

    ; Return zero if successful
    return, 0

end



function fldpnt_sph, frho, flat, flon, az, r, xlat, xlon

    ; This function uses the Haversine formula to calculate a destination
    ; point (xlat,xlon) given distance (r) and bearing (az) from a start
    ; point (frho,flat,flon)

    api = 4*atan(1.0)
    cside = 90.0 - flat
    Aangle = az

    if Aangle gt 180 then $
        Aangle = Aangle - 360

    bside = r/frho*(180.0/api)

    arg = cos(bside*api/180.0)*cos(cside*api/180.0) + $
        sin(bside*api/180.0)*sin(cside*api/180.0)*cos(Aangle*api/180.0)

    if arg le -1.0 then $
        arg = -1.0

    if arg ge 1.0 then $
        arg = 1.0

    aside = acos(arg) * 180.0/api

    arg2 = (cos(bside*api/180.0)-cos(aside*api/180.)*cos(cside*api/180.))/(sin(aside*api/180.0)*sin(cside*api/180.0))

    if arg2 le -1.0 then $
        arg2 = -1.0

    if arg2 ge 1.0 then $
        arg2 = 1.0

    Bangle = acos(arg2) * 180.0/api

    if Aangle lt 0 then $
        Bangle = -Bangle

    xlat = 90 - aside
    xlon = flon + Bangle

    if xlon lt 0 then $
        xlon = xlon + 360

    if xlon gt 360 then $
        xlon = xlon - 360

    ; Return zero if successful
    return, 0

end



function fldpnt_azm, mlat, mlon, nlat, nlon, az

    ; This function uses the Haversine formula to calculate bearing (az)
    ; from a start point (mlat,mlon) to an end point (nlat,nlon) assuming
    ; a spherical Earth

    api = 4*atan(1.0)

    aside = 90 - nlat

    cside = 90 - mlat

    Bangle = nlon - mlon

    arg = cos(aside*api/180.0)*cos(cside*api/180.0) + $
        sin(aside*api/180.0)*sin(cside*api/180.0)*cos(Bangle*api/180.0)

    bside = acos(arg) * 180.0/api

    arg2 = (cos(aside*api/180.0)-cos(bside*api/180.0)*cos(cside*api/180.0))/(sin(bside*api/180.0)*sin(cside*api/180.0))

    Aangle = acos(arg2) * 180.0/api

    if Bangle lt 0 then $
        Aangle = -Aangle

    az = Aangle

    ; Check for cases when az = NAN rather than zero
    if ~finite(az) then az = 0.0

    ; Return zero if successful
    return, 0

end



function RPosRngBmAzmElv, beam, range, year, site, frang, rsep, rxrise, height, azm, elv, chisham=chisham

    ; Get geodetic latitude/longitude from radar hardware info [deg]
    gdlat = site.geolat
    gdlon = site.geolon

    ; Get receiver rise time from site information if not provided
    if rxrise eq 0 then $
        rxrise = site.recrise

    ; Convert center of range/beam cell to geocentric spherical latitude/longitude (flat,flon) and distance from
    ; the center to the surface of the oblate spheroid (ie not constant w/latitude) plus virtual height (frho)
    center = 1
    ret = RadarPos(center, beam, range, site, frang, rsep, rxrise, height, frho, flat, flon, chisham=chisham)

    ; Convert range/beam position from geocentric spherical coordinates (frho,flat,flon) to global
    ; Cartesian coordinates (fx,fy,fz)
    ret = sphtocar(frho, flat, flon, fx, fy, fz)

    ; Convert radar site geodetic latitude and longitude (gdlat,gdlon) to geocentric spherical
    ; coordinates (glat,glon) and distance from the center to the surface of the oblate spheroid (gdrho)
    RadarGeoTGC, 1, gdlat, gdlon, gdrho, glat, glon, del

    ; Convert radar geocentric spherical coordinates (gdrho,glat,glon) to global Cartesian
    ; coordinates (gbx,gby,gbz)
    ret  = sphtocar(gdrho, glat, glon, gbx, gby, gbz)

    ; Calculate vector from the radar to center of range/beam cell (gx,gy,gz)
    gx = fx - gbx
    gy = fy - gby
    gz = fz - gbz

    ; Normalize the vector from the radar to center of range/beam cell (ngx,ngy,ngz)
    ret = norm_vec(gx, gy, gz, ngx, ngy, ngz)

    ; Convert the normalized vector from the radar-to-range/beam cell (ngx,ngy,ngz) into
    ; local south/east/vertical (horizontal) coordinates (ghx,ghy,ghz)
    ret = glbthor(flat, flon, ngx, ngy, ngz, ghx, ghy, ghz)

    ; Normalize the local horizontal radar-to-range/beam cell vector (nghx,nghy,nghz)
    ret = norm_vec(ghx, ghy, ghz, nghx, nghy, nghz)

    ; Calculate the magnetic field vector (bx,by,bz) at the geocentric spherical
    ; range/beam position (frho,flat,flon) in local south/east/vertical coordinates
    ret = IGRFMagCmp(year, frho, flat, flon, bx, by, bz, b)

    ; Error check present in C RST code
    if ret eq -1 then $
        return, -1

    ; Normalize the magnetic field vector (nbx,nby,bz)
    ret = norm_vec(bx, by, bz, nbx, nby, nbz)

    ; Calculate a new local vertical component such that the radar-to-range/beam vector
    ; becomes orthogonal to the magnetic field at the range/beam position (ngh dot nb = 0)
    nghz_perp = -1.*(nbx*nghx + nby*nghy)/nbz

    ; Normalize the new radar-to-range/beam vector (which is now orthogonal to B)
    ret = norm_vec(nghx, nghy, nghz_perp, nnghx, nnghy, nnghz)

    ; Calculate the elevation angle of the orthogonal radar-to-range/beam vector
    elv = atan( nnghz, sqrt(nnghx*nnghx + nnghy*nnghy) )*180./!pi

    ; Calculate the azimuth of the orthogonal radar-to-range/beam vector
    azm = atan( nnghy, -1.*nnghx)*180./!pi

    ; Return zero if successful
    return, 0

end



function RPosInvMag, beam, range, year, site, frang, rsep, rxrise, height, mlat, mlon, mazm, gazm=gazm, chisham=chisham, old_aacgm=old_aacgm

    ; Get geodetic latitude/longitude from radar hardware info [deg]
    gdlat = site.geolat
    gdlon = site.geolon

    ; Get receiver rise time from site information if not provided
    if rxrise eq 0 then $
        rxrise = site.recrise

    ; Convert center of range/beam cell to geocentric spherical latitude/longitude (flat,flon) and distance from
    ; the center to the surface of the oblate spheroid (ie not constant w/latitude) plus virtual height (frho)
    center = 1
    ret = RadarPos(center, beam, range, site, frang, rsep, rxrise, height, frho, flat, flon, chisham=chisham)

    ; Convert range/beam position from geocentric spherical coordinates (frho,flat,flon) to global
    ; Cartesian coordinates (fx,fy,fz)
    ret = sphtocar(frho, flat, flon, fx, fy, fz)

    ; Convert radar site geodetic latitude and longitude (gdlat,gdlon) to geocentric spherical
    ; coordinates (glat,glon) and distance from the center to the surface of the oblate spheroid (gdrho)
    RadarGeoTGC, 1, gdlat, gdlon, gdrho, glat, glon, del

    ; Convert radar geocentric spherical coordinates (gdrho,glat,glon) to global Cartesian
    ; coordinates (gbx,gby,gbz)
    ret  = sphtocar(gdrho, glat, glon, gbx, gby, gbz)

    ; Calculate vector from the radar to center of range/beam cell (gx,gy,gz)
    gx = fx - gbx
    gy = fy - gby
    gz = fz - gbz

    ; Normalize the vector from the radar to center of range/beam cell (ngx,ngy,ngz)
    ret = norm_vec(gx, gy, gz, ngx, ngy, ngz)

    ; Convert the normalized vector from the radar-to-range/beam cell (ngx,ngy,ngz) into
    ; local south/east/vertical (horizontal) coordinates (ghx,ghy,ghz)
    ret = glbthor(flat, flon, ngx, ngy, ngz, ghx, ghy, ghz)

    ; Normalize the local horizontal radar-to-range/beam cell vector (nghx,nghy,nghz)
    ret = norm_vec(ghx, ghy, ghz, nghx, nghy, nghz)

    ; Calculate the magnetic field vector (bx,by,bz) at the geocentric spherical
    ; range/beam position (frho,flat,flon) in local south/east/vertical coordinates
    ret = IGRFMagCmp(year, frho, flat, flon, bx, by, bz, b)

    ; Error check present in C RST code
    if ret eq -1 then $
        return, -1

    ; Normalize the magnetic field vector (nbx,nby,bz)
    ret = norm_vec(bx, by, bz, nbx, nby, nbz)

    ; Calculate a new local vertical component such that the radar-to-range/beam vector
    ; becomes orthogonal to the magnetic field at the range/beam position (ngh dot nb = 0)
    nghz_perp = -1.*(nbx*nghx + nby*nghy)/nbz

    ; Normalize the new radar-to-range/beam vector which is now orthogonal to B
    ret = norm_vec(nghx, nghy, nghz_perp, nnghx, nnghy, nnghz)

    ; Calculate the elevation angle of the orthogonal radar-to-range/beam vector (***NOT USED***)
    elv = atan( nnghz, sqrt(nnghx*nnghx + nnghy*nnghy) )*180./!pi

    ; Calculate the (geographic) azimuth of the orthogonal radar-to-range/beam vector
    gazm = atan( nnghy, -1.*nnghx)*180./!pi

    ; Convert range/beam position from geocentric (flat,flon) to geodetic latitude/longitude 
    ; (gdlat,gdlon) and calculate distance from center to surface of oblate spheroid (fdrho)
    RadarGeoTGC, -1, fdlat, fdlon, fdrho, flat, flon, dummy

    ; Calculate virtual height of range/beam position
    tmp_height = frho - fdrho

    ; Convert range/beam position from geocentric latitude/longitude (flat,flon) at virtual height
    ; (tmp_height) to AACGM magnetic latitude/longitude coordinates (mlat,mlon)
    if keyword_set(old_aacgm) then $
        ret = AACGMConvert(flat, flon, tmp_height, mlat, mlon, dummy) $
    else $
        ret = AACGM_v2_Convert(flat, flon, tmp_height, mlat, mlon, dummy)

    ; Calculate pointing direction latitude/longitude (xlat,xlon) given
    ; distance (rsep) and bearing (gazm) from the radar position (flat,flon)
    ; at the field point radius (frho)
    ret = fldpnt_sph(frho, flat, flon, gazm, rsep, xlat, xlon)

    ; Convert pointing direction position from geocentric latitude/longitude
    ; (xlat,xlon) at virtual height (tmp_height) to AACGM magnetic latitude/longitude
    ; coordinates (nlat,nlon)
    if keyword_set(old_aacgm) then $
        ret = AACGMConvert(xlat, xlon, tmp_height, nlat, nlon, dummy) $
    else $
        ret = AACGM_v2_Convert(xlat, xlon, tmp_height, nlat, nlon, dummy)

    ; Error check present in C RST code
    if ret eq -1 then $
        return, -1

    ; Make sure nlon varies between +/-180 degrees
    if (nlon - mlon) gt 180 then $
        nlon = nlon - 360

    ; Make sure nlon varies between +/-180 degrees
    if (nlon-mlon) lt -180 then $
        nlon = nlon + 360

    ; Calculate bearing (mazm) to pointing direction latitude/longitude
    ; (nlat,nlon) from the radar position (mlat,mlon) in magnetic coordinates
    ret = fldpnt_azm(mlat, mlon, nlat, nlon, mazm)

    ; Return zero if successful
    return, 0

end
