;provide two points, azimuth is
;returned in azm variable
pro getazm,lat1,lon1,lat2,lon2,azm
  dlon = lon2-lon1
  y = sin(dlon/!radeg)*cos(lat2/!radeg)
  x = cos(lat1/!radeg)*sin(lat2/!radeg)-sin(lat1/!radeg)*cos(lat2/!radeg)*cos(dlon/!radeg)
  azm = atan(y,x)
end

;provide a point, an azm, and a distance(v)
;endpoint is returned in lat2,lon2
pro getendpoint,glat,glon,gazm,v,lat2,lon2
  lat1 = glat/!radeg
  lon1 = glon/!radeg
  azm = gazm/!radeg
  Re = 6378.
  alpha = v/Re
  dlat = alpha*cos(azm)
  lat2 = lat1+dlat
  dphi = alog(tan(lat2/2.+!pi/4)/tan(lat1/2.+!pi/4))
  if(dphi eq 0) then q = cos(lat1) $
  else q = dlat/dphi
  dlon = alpha*sin(azm)/q
  lon2 = ((lon1+dlon+!pi) MOD (2.*!pi))-!pi
  lat2 = lat2*!radeg
  lon2 = lon2*!radeg
end

;provide 2 points, distance between
;the points is returned in km in d
pro distance,lat1,lon1,lat2,lon2,d
  Re = 6378.
  dlat = (lat2-lat1)/!radeg
  dlon = (lon2-lon1)/!radeg
  a = sin(dlat/2.)*sin(dlat/2.)+cos(lat1/!radeg)*cos(lat2/!radeg)*sin(dlon/2.)*sin(dlon/2.)
  c = 2.*atan(sqrt(a),sqrt(1.-a))
  d = Re*c
end

;code to trace along an azimuth, originally written by Mike
pro getvec,vel,vel_scl,lat,lon,azm,n_xy_s,n_xy_e
    xlen = 0.2
    scx_len   =  (vel/vel_scl * xlen)
    grho      =   6378.
    rx  =  grho*cos(lat/!radeg) * cos(lon/!radeg)
    ry  =  grho*cos(lat/!radeg) * sin(lon/!radeg)
    rz  =  grho*sin(lat/!radeg)
    hx  = -45 * cos(0/!radeg) * cos(azm/!radeg)
    hy  =  45 * cos(0/!radeg) * sin(azm/!radeg)
    hz  =  45 * sin(0/!radeg)
    lax =  90 - lat
    ax  =  cos(lax/!radeg)*hx + sin(lax/!radeg)*hz
    ay  =  hy
    az  = -sin(lax/!radeg)*hx + cos(lax/!radeg)*hz
    sx  =  cos(lon/!radeg)*ax - sin(lon/!radeg)*ay
    sy  =  sin(lon/!radeg)*ax + cos(lon/!radeg)*ay
    sz  =  az
    tx  =  rx + sx
    ty  =  ry + sy
    tz  =  rz + sz
    xrho =  sqrt(tx^2+ty^2+tz^2)
    xlat =  asin(tz/xrho)   * !radeg
    ang = atan(ty/tx)
    if (tx lt 0) then ang = ang + !pi
    xlon = ang * !radeg
    n_xy_s    =  convert_coord(lon,lat,/data,/to_normal)
    n_xy_t    =  convert_coord(xlon,xlat,/data,/to_normal)
    dlon = n_xy_s(0)
    dlat = n_xy_s(1)
    elon = n_xy_t(0)
    elat = n_xy_t(1)
    eazm = atan((elon - dlon),(elat - dlat))
    flon = sin(eazm)*scx_len+dlon
    flat = cos(eazm)*scx_len+dlat
    n_xy_e = [flon,flat]
end