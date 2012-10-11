pro draw_colorbar,vel_max,vel_min,title,units,loc,ct=ct
  ;loc=[x0,y0,x1,y1]
;   loc = [0.1, 0.35, 0.13, 0.65]
  if(keyword_set(ct)) then $
    loadct,ct,ncolors=256 $
  else loadct,34,ncolors=256
  ncolors = 256
  bar = replicate(1B, 10) # bindgen(256)
  xsize = loc(2) - loc(0)
  ysize = loc(3) - loc(1)
  xstart = loc(0)
  ystart = loc(1)
  bar = BYTSCL(bar, TOP=ncolors-1)
  TV, bar,xstart,ystart,XSIZE=xsize,YSIZE=ysize,/normal
  loadct,0
  plots, [loc(0), loc(0), loc(2), loc(2), loc(0)],[loc(1), loc(3), loc(3), loc(1), loc(1)], /NORMAL
  xyouts,xsize/2.+loc(0),loc(3)+.025,title,alignment=0.5,/normal,charthick=3
  xyouts,xsize/2.+loc(0),loc(1)-.02,units,alignment=0.5,/normal,charsize=0.7,charthick=3
  xyouts,loc(2)+.01,loc(1)-.005,strmid(strtrim(float(vel_min),2),0,7),alignment=0.,/normal,charthick=3
  xyouts,loc(2)+.01,loc(3)-.005,strmid(strtrim(float(vel_max),2),0,7),alignment=0.,/normal,charthick=3
  xyouts,loc(2)+.01,loc(1)+ysize/4.-.005,$
        strmid(strtrim(float(vel_min + (vel_max-vel_min)*.25),2),0,7),$
        alignment=0.,/normal,charthick=3
  plots,[loc(0),loc(2)],[loc(1)+ysize/4.,loc(1)+ysize/4.],/normal
  xyouts,loc(2)+.01,loc(1)+ysize/2.-.005,$
        strmid(strtrim(float(vel_min + (vel_max-vel_min)*.5),2),0,7),$
        alignment=0.,/normal,charthick=3
  plots,[loc(0),loc(2)],[loc(1)+ysize/2.,loc(1)+ysize/2.],/normal
  xyouts,loc(2)+.01,loc(1)+3*ysize/4.-.005,$
        strmid(strtrim(float(vel_min + (vel_max-vel_min)*.75),2),0,7),$
        alignment=0.,/normal,charthick=3
  plots,[loc(0),loc(2)],[loc(1)+3*ysize/4.,loc(1)+3*ysize/4.],/normal
end

pro draw_colorbar_noround,vel_max,vel_min,title,units,loc,ct=ct
  ;loc=[x0,y0,x1,y1]
;   loc = [0.1, 0.35, 0.13, 0.65]
  if(keyword_set(ct)) then $
    loadct,ct,ncolors=256 $
  else loadct,34,ncolors=256
  ncolors = 256
  bar = replicate(1B, 10) # bindgen(256)
  xsize = loc(2) - loc(0)
  ysize = loc(3) - loc(1)
  xstart = loc(0)
  ystart = loc(1)
  bar = BYTSCL(bar, TOP=ncolors-1)
  TV, bar,xstart,ystart,XSIZE=xsize,YSIZE=ysize,/normal
  loadct,0
  plots, [loc(0), loc(0), loc(2), loc(2), loc(0)],[loc(1), loc(3), loc(3), loc(1), loc(1)], /NORMAL
  xyouts,xsize/2.+loc(0),loc(3)+.025,title,alignment=0.5,/normal,charthick=3
  xyouts,xsize/2.+loc(0),loc(1)-.02,units,alignment=0.5,/normal,charsize=0.7,charthick=3
  xyouts,loc(2)+.01,loc(1)-.005,strtrim(vel_min,2),alignment=0.,/normal,charthick=3
  xyouts,loc(2)+.01,loc(3)-.005,strtrim(vel_max,2),alignment=0.,/normal,charthick=3
  xyouts,loc(2)+.01,loc(1)+ysize/4.-.005,$
        strtrim(vel_min + (vel_max-vel_min)*.25,2),$
        alignment=0.,/normal,charthick=3
  plots,[loc(0),loc(2)],[loc(1)+ysize/4.,loc(1)+ysize/4.],/normal
  xyouts,loc(2)+.01,loc(1)+ysize/2.-.005,$
        strtrim(vel_min + (vel_max-vel_min)*.5,2),$
        alignment=0.,/normal,charthick=3
  plots,[loc(0),loc(2)],[loc(1)+ysize/2.,loc(1)+ysize/2.],/normal
  xyouts,loc(2)+.01,loc(1)+3*ysize/4.-.005,$
        strtrim(vel_min + (vel_max-vel_min)*.75,2),$
        alignment=0.,/normal,charthick=3
  plots,[loc(0),loc(2)],[loc(1)+3*ysize/4.,loc(1)+3*ysize/4.],/normal
end

pro draw_colorbar2,vel_max,vel_min
  ;loc=[x0,y0,x1,y1]
  loc = [0.02, 0.35, 0.05, 0.65]
  loadct,34,ncolors=256
  ncolors = 256
  bar = replicate(1B, 10) # bindgen(256)
  xsize = loc(2) - loc(0)
  ysize = loc(3) - loc(1)
  xstart = loc(0)
  ystart = loc(1)
  bar = BYTSCL(bar, TOP=ncolors-1)
  TV, bar,xstart,ystart,XSIZE=xsize,YSIZE=ysize,/normal
  loadct,0
  plots, [loc(0), loc(0), loc(2), loc(2), loc(0)],[loc(1), loc(3), loc(3), loc(1), loc(1)], /NORMAL
  xyouts,xsize/2.+loc(0),loc(3)+.01,'Corot Factor',alignment=0.5,/normal
  xyouts,loc(2)+.01,loc(1)-.005,strtrim((vel_min),2),alignment=0.,/normal
  xyouts,loc(2)+.01,loc(3)-.005,strtrim(vel_max,2),alignment=0.,/normal
  xyouts,loc(2)+.01,loc(1)+ysize/4.-.005,strtrim((vel_min + (vel_max-vel_min)*.25),2),alignment=0.,/normal
  plots,[loc(0),loc(2)],[loc(1)+ysize/4.,loc(1)+ysize/4.],/normal
  xyouts,loc(2)+.01,loc(1)+ysize/2.-.005,strtrim((vel_min + (vel_max-vel_min)*.5),2),alignment=0.,/normal
  plots,[loc(0),loc(2)],[loc(1)+ysize/2.,loc(1)+ysize/2.],/normal
  xyouts,loc(2)+.01,loc(1)+3*ysize/4.-.005,strtrim((vel_min + (vel_max-vel_min)*.75),2),alignment=0.,/normal
  plots,[loc(0),loc(2)],[loc(1)+3*ysize/4.,loc(1)+3*ysize/4.],/normal
end


pro getendpoints,vel,type,vel_max,xlen,lat,lon,azm,n_xy_e
  if(type ne 3) then scx_len   =  (vel/vel_max * xlen)*(-1.) $
  else scx_len   =  (vel/vel_max * xlen)
  grho      =   6378
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

pro draw_map_mlt,title_str

  maxlat = 72
  minlat = 38
  maxlon = 180
  minlon = -180
  meanlon = 180

  map_set,90,0,meanlon,limit=[minlat,minlon,maxlat,maxlon],/orthographic,/isotropic,$
          /grid,position=[.1,.1,.9,.9],/noerase,latdel=5
  xyouts,.5,.95,title_str+', Mlat-MLT',/normal,alignment=.5,charthick=3.,charsize=2
  xyouts,.5,.91,'12',/normal,alignment=.5,charthick=3.,charsize=1.2
  xyouts,.5,.08,'0',/normal,alignment=.5,charthick=3.,charsize=1.2
  xyouts,.18,.5,'18',/normal,alignment=.5,charthick=3.,charsize=1.2
  xyouts,.81,.5,'6',/normal,alignment=.5,charthick=3.,charsize=1.2
  xyouts,.782,.5,'40',/normal,alignment=.5,charthick=3.
  xyouts,.735,.5,'50',/normal,alignment=.5,charthick=3.
  xyouts,.684,.5,'60',/normal,alignment=.5,charthick=3.
  xyouts,.626,.5,'70',/normal,alignment=.5,charthick=3.
end

pro draw_vel_colorbar,colors,scales,stx,sty,grayflg,gsflg
  loadct,0
  xyouts,stx+.01,sty-.05,'m/s',alignment=0.5,size=1,/normal,charsize=2,charthick=4
  if(grayflg) then $
    xyouts,stx+.01,.775,'Velocity',alignment=0.5,size=1.25,/normal,charsize=2,charthick=4 $
  else $
    xyouts,stx+.01,.725,'Velocity',alignment=0.5,size=1.25,/normal,charsize=2,charthick=4
  

  for i=0,n_elements(colors)-1 do begin
    if(grayflg eq 1 AND i eq 4) then loadct,0 $
    else loadct,34
    polyfill,[stx,stx+.02,stx+.02,stx],[sty+.05*i,sty+.05*i,sty+.05*(i+1),sty+.05*(i+1)],col=colors(i),/normal
    loadct,0
    plots,[stx,stx],[sty+.05*i,sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx+.02,stx+.02],[sty+.05*i,sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx,stx+.02],[sty+.05*(i+1),sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx,stx+.02],[sty+.05*i,sty+.05*i],col=0,/normal,thick=4.
  endfor

  for i=1,n_elements(scales) do begin
    xyouts,stx+.025,sty+.05*i,strtrim(fix(scales(i-1)),2),alignment=0,size=1.,/normal,charsize=2,charthick=4
  endfor

  if(gsflg eq 1) then begin
    i = n_elements(colors)+3
    loadct,0
    polyfill,[stx,stx+.02,stx+.02,stx],[sty+.05*i,sty+.05*i,sty+.05*(i+1),sty+.05*(i+1)],col=150,/normal
    plots,[stx,stx],[sty+.05*i,sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx+.02,stx+.02],[sty+.05*i,sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx,stx+.02],[sty+.05*(i+1),sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx,stx+.02],[sty+.05*i,sty+.05*i],col=0,/normal,thick=4.
    xyouts,stx+.025,sty+.05*i+.02,'gs',alignment=0,size=1.,/normal,charsize=2,charthick=4
  endif
  
end

pro draw_other_colorbar,colors,scales,stx,sty,ctype
  loadct,0
  case ctype of
    'pow' : begin
      units = 'dB'
      title = 'Power'
    end
    'wid' : begin
      units = 'm/s'
      title = 'Spec. Width'
    end
    'elv' : begin
      units = 'deg'
      title = 'Elv Angle'
    end
  endcase

  xyouts,.93,sty-.05,units,alignment=0.5,size=1,/normal,charsize=2,charthick=4
  xyouts,.93,.695,title,alignment=0.5,size=1.25,/normal,charsize=2,charthick=4
  

  for i=0,n_elements(colors)-1 do begin
    loadct,34
    polyfill,[stx,stx+.02,stx+.02,stx],[sty+.05*i,sty+.05*i,sty+.05*(i+1),sty+.05*(i+1)],col=colors(i),/normal
    loadct,0
    plots,[stx,stx],[sty+.05*i,sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx+.02,stx+.02],[sty+.05*i,sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx,stx+.02],[sty+.05*(i+1),sty+.05*(i+1)],col=0,/normal,thick=4.
    plots,[stx,stx+.02],[sty+.05*i,sty+.05*i],col=0,/normal,thick=4.
  endfor

  for i=0,n_elements(scales)-1 do begin
    xyouts,.95,sty+.05*i,strtrim(fix(scales(i)),2),alignment=0,size=1.,/normal,charsize=2,charthick=4
  endfor
  
end